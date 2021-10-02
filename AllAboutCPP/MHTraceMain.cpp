//-----------------------------------------
//
//  MHTraceMain.cpp
//  FundLifter
//
//  Created by Magnus Hyttsten on 1/28/21.
//  Copyright © 2021 Magnus Hyttsten. All rights reserved.
//

#include <cctype>
#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <unordered_map>
#include <sstream>
#include "CPPUtils.h"
#include "CPPIndentWriter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

// TODO: Fixes and extensions
// - Inlining C++ code in headers, and the force non-inlining?
//   Only add class definitions in case any code was added in the file
// - Make skipFunctions/skipFiles also an environment variable alternative
// - Detect and warn against multiple thread-ids active
// - Add support for tensorflow::string (what is that?)
//
// - Make Python work, does it use trace indention
//   Alt, selectively compile CC or C for headers depending on some classification
// - Make C/C-h work
// - Can we get this work for genfiles
// - Check CSRSparseMatrix is not skipFunction filtered causing metadata_{} errors
//   tensorflow/tensorflow/core/kernels/sparse/sparse_matrix.h
//   Observe however, that you intercept metadata_() non-function in other CSRSparseMatrix constructors
//   Probably need a way to parse member initializers after :...
//   Related to: Also gives support for others, e.g "IdAllocator" ': id1{}, id2{} {...
//   (tensorflow/tensorflow/core/summary/summary_db_writer.cc)


// PATH: MHTRACER_ENABLE must be part of it for anything to be emitted

static string MHTRACER_LINENUMBER_TAG = "MHTRACER_LN_TAG";
static string ENV_MHTRACER_INDENT = "MHTRACER_INDENT_";  // Tied to PID-TID
static string ENV_PID_DASH_TID = "MHTRACER_PID_DASH_TID";
static string PATH_ENV_MHTRACER_FILE = "MHTRACER_USEFILE";   // Set this a part of PATH variable to write to file
static string PATH_ENV_MHTRACER_ENABLE = "MHTRACER_ENABLE";   // Set this a part of PATH variable to enable tracing
static string MHTRACER_FILENAME_PREFIX = "/tmp/mhtracer_";

// ****************************************************************
vector<pair<size_t,string>> archiveCommentsAndStringLiterals;
//const char charArchive = '@';
const char charArchive = 0x02;


// ****************************************************************
struct MHReduction {
  int countOccurence = 1;
  string functionName;
  string prefix;
  vector<string> params;  // E.g. [str: "hello", fileSystem: "gcs"]
  string location;

  bool isComment = false;
  string commentLine;
  
  MHReduction(const string& in) {
    
    if (CPPUtils::strStartsWith(CPPUtils::strTrim(in), "//")) {
      isComment = true;
      commentLine = in;
      return;
    }
    
    size_t idx_lb = in.find("[");
    functionName = in.substr(0, idx_lb);
    size_t idx_rb = in.find("]", idx_lb);
    string countRepeatStr = in.substr(idx_lb+1, idx_rb-1-idx_lb);
    countOccurence = stoi(countRepeatStr);
    
    idx_lb = in.find("[", idx_rb);
    idx_rb = in.find("]", idx_lb);
    prefix = CPPUtils::strTrim(in.substr(idx_lb+1, idx_rb-1-idx_lb));
    
    idx_lb = in.find("[", idx_rb);
    idx_rb = in.find("]", idx_lb);
    string param = in.substr(idx_lb+1, idx_rb-1-idx_lb);
    if (param.size() > 0) {
      params.push_back(param);
    }
    
    idx_lb = in.find("[", idx_rb);
    idx_rb = in.find("]", idx_lb);
    location = in.substr(idx_lb+1, idx_rb-1-idx_lb);
  }

  bool operator != (const MHReduction& rv) {
    if (isComment && rv.isComment) {
      return true;
    }
    return (functionName != rv.functionName
            || location != rv.location
            || prefix != rv.prefix);
  }
  
  void cleanParams() {
    unordered_map<string, void*> um;
    for (auto e: params) {
      if (CPPUtils::strEndsWith(e, ", ")) {
        e = e.substr(0, e.size()-2);
      }
      um[e] = nullptr;
    }
    params.clear();
    for (auto p: um) {
      params.push_back(p.first);
    }
  }
  
  string getString() {
    stringstream ss;
    
    if (isComment) {
      ss << commentLine << "\n";
      return ss.str();
    }
    
    ss << functionName + " [" + to_string(countOccurence) + "] [" + prefix + "]";
    
    cleanParams();
    if (params.size() == 0 || params.size() > 1) {
      ss << " []";
    }
    else if (params.size() == 1) {
      ss << " [" << params[0] << "]";
    }
    ss << " [" + location + "]";

    int sc = 0;
    while (sc < functionName.size() && functionName[sc] == ' ') {
      sc++;
    }
    string spaces(sc, ' ');
    bool didEmitNL = false;
    if (params.size() > 1) {
      ss << "\n";
      for (auto i=0; i < params.size(); i++) {
        ss << spaces << to_string(i) << ": [" << params[i] << "]\n";
      }
      didEmitNL = true;
    }
    if (!didEmitNL) {
      ss << "\n";
    }
    return ss.str();
  }
};

int getLineNumber(string in, size_t idx_end);
string generateClass(CPPIndentWriter& iw, string fileName);
void encodeCommentsAndStringLiterals(string&);
void decodeCommentsAndStringLiterals(string& fin);
string genPrintf(const string& functionName, const string& prefix, const string& fileName);
string assignLineNumbers(const string& s);
size_t constructorScanPastInitializers(string in, size_t idx_start);
pair<bool, string> analyzeReturnType(const string& in, size_t idx_start);
size_t scanBeyondNesting(string in, size_t idx_start, char nestCharStart, char nextCharEnd);
string codeGenGetClassName(const string& fileName);
string codeGenGenerateClass(CPPIndentWriter& iw, const string& fileName);
tuple<string, string, string> scanCPPVariableAndCode(int variableIndex, const string& fin, size_t idx_lparen);
vector<MHReduction> modeReduce(int windowSize, int& countReductions, int& countReducedLines, const vector<MHReduction>& in);

string washFileName(string fname) {
//  string fileName = CPPUtils::strReplace(fname, "tensorflow", "tf");
//  fileName = CPPUtils::strReplace(fileName, "framework", "fw");
  return fname;
}

//string generatePYCode(int indent, string functionName, string fileName) {
//  string printGuts  = functionName + " [PY @ " + MHTRACER_LINENUMBER_TAG + " @ " + fileName + "]";
//
//  CPPIndentWriter iw;
//  string istr;
//  istr.assign(indent, ' ');
//
//  iw.println(istr + "env_indent_variable = '" + "MHTRACER_INDENT_" + "' + str(os.getpid())");
//  iw.println(istr + "indent = os.environ.get(env_indent_variable)");
//  iw.println(istr + "if indent is None:");
//  iw.println(istr + "   indent = 0");
//  iw.println(istr + "indent = int(indent) * \" \"");
////  iw.println(istr + "indent = \"\"");
//  iw.println(istr + "envPath = os.environ.get('PATH')");
//  iw.println(istr + "if envPath is not None and '" + PATH_ENV_MHTRACER_DISABLE + "' in envPath:");
//  iw.println(istr + "   pass");
//  iw.println(istr + "elif envPath is None or '" + PATH_ENV_MHTRACER_FILE + "' not in envPath:");
////  iw.println(istr + "isFile = os.environ.get('" + ENV_MHTRACER_FILE + "')");
////  iw.println(istr + "if isFile is None:");
//  iw.println(istr + "   print(\"P \", indent, \"" + printGuts + "\")");
//  iw.println(istr + "elif envPath is not None and '" + PATH_ENV_MHTRACER_FILE + "' in envPath:");
//  iw.println(istr + "   fileName = \"" + MHTRACER_FILENAME_PREFIX + "\" + str(os.getpid()) + \".log\"");
//  iw.println(istr + "   fp = open(fileName, \"a\");");
//  iw.println(istr + "   fp.write(indent + \"" + printGuts + "\")");
//  iw.println(istr + "   fp.close()");
//  return iw.getString();
//}

bool isWhitespace(char c);

void foo(int) {
  cout << "Hello\n";
}

// Returns [idx_start, index] of the matching element
pair<size_t, int> findFirst(string in,
                            const vector<string>& v,
                            size_t idx_start,
                            bool ignoreEscape=false,
                            bool scanBeyondMatch=false) {
  pair<size_t, int> winner(string::npos, -1);
  
  // Check that we have descending order, needed to get best match
  assert(v.size() > 0);
  int longest = -1;
  for (auto s: v) {
    if (longest == -1)
      longest = (int)s.size();
    else if (longest < s.size()){
      longest = (int)s.size();
    }
  }
  
  // Now scan for best match
  size_t idx_v = 0;
  while (idx_v < v.size()) {
    size_t idx = in.find(v[idx_v], idx_start);
    if (idx == string::npos) {
      // Not found, then it's not the winner
      idx_v++;
      continue;
    }
    // Track back any escape situations
    if (!ignoreEscape && in[idx-1] == '\\') {
      int count = 0;
      size_t tmp = idx-1;
      char c = in[tmp];
      while (tmp >= 0 && c == '\\') {
        tmp--;
        count++;
        if (tmp > 0) {
          c = in[tmp];
        }
      }
      // There is an escape situation
      if (count % 2 != 0) {
        idx_start = idx+1;
        continue;  // We continue with current, since it might come immediately after escaped char
      }
    }
    if (winner.first == string::npos) {
      // No winner assigned yet, then assign it
      winner =  pair<size_t, int>(idx, idx_v);
      idx_v++;
      continue;
    }
    // We have a true match, does it win?
    if (winner.first > idx
        || (winner.first == idx && v[winner.second].size() < v[idx_v].size())) {
      winner = pair<size_t, int>(idx, idx_v);
    }
    idx_v++;
  }
  if (scanBeyondMatch) {
    if (winner.first != string::npos) {
        winner.first += v[winner.second].size();
    } else {
      winner.first = in.size();
    }
  }
  return winner;
}

size_t archiveAndMoveForward(string& fin, size_t start, size_t end);
//string compilePython(string fin, string fileName) {
//  fileName = washFileName(fileName);
//
//  size_t idx_current = 0;
//
//  // Replace all comments
//  idx_current = 0;
//  vector<string> kws = {"\"\"\"", "\'\'\'", "#", "\"", "\'"};
//  while (true) {
//    pair<size_t, int> idx_start = findFirst(fin, kws, idx_current);
////    cout << "p.second: " << idx_start.second << endl;
//    if (idx_start.first == string::npos) break;
//
//    // Check if we have an rstring (in which escape characters are ignored)
//    bool supressEscape = false;
//    if (idx_start.first > 0 && fin[idx_start.first] != '#' && fin[idx_start.first-1] == 'r') {
//      supressEscape = true;
//    }
//
//    pair<size_t, int> idx_end = pair<size_t, int>(string::npos, 0);
//
//    switch (idx_start.second) {
//      case 0:
//        idx_end = findFirst(fin, {kws[0]}, idx_start.first+kws[0].size(), supressEscape, true);
//        break;
//      case 1:
//        idx_end = findFirst(fin, {kws[1]}, idx_start.first+kws[1].size(), supressEscape, true);
//        break;
//      case 2:
//        idx_end = findFirst(fin, {"\n"}, idx_start.first+1, true, false);  // No escape for '#', do not move behond '\n'
//        break;
//      case 3:
//        idx_end = findFirst(fin, {"\""}, idx_start.first+1, supressEscape, true);
//        break;
//      case 4:
//        idx_end = findFirst(fin, {"\'"}, idx_start.first+1, supressEscape, true);
//        break;
//      default:
//        assert(false);
//    }
//    string data = fin.substr(idx_start.first, idx_end.first-idx_start.first);
////    cout << "data: [" << data << "]\n";
//    if (idx_end.first == string::npos) {
//      idx_end.first = fin.size();
//    }
//    idx_current = archiveAndMoveForward(fin, idx_start.first, idx_end.first);
////    cout << "now: [" << fin.substr(idx_current, 5) << "]\n";
//    if (idx_current == string::npos || idx_current >= fin.size())
//      break;
//  } // End-of replace all comments and string literals
//
//  // First, import os after from __future__ statements
////  cout << "Archive [" << "]\n";
//  idx_current = 0;
//  string fout;
//  string c_from_future = "from __future__ ";
//  size_t idx_c = fin.rfind(c_from_future);
////  cout << "[" << fin.substr(idx_c-5, 20) << "]\n";
//  if (idx_c != string::npos) {
//    size_t idx_c2 = fin.find("\n", idx_c);
////    cout << "[" << fin.substr(idx_c, idx_c2-idx_c) << "]\n";
////    cout << "[" << fin.substr(idx_c2+1, 20) << "]\n";
//    if (idx_c2 != string::npos) {
//      fout += fin.substr(0, idx_c2+1);
//      idx_current = idx_c2+1;
//    } else {
//      fout += fin.substr(0, fin.size());
//      idx_current = fin.size();
//    }
//    fout += "import os\n";
//  }
//
//  // Insert trace code into all functions
//  while (idx_current < fin.size()) {
////    cout << "idx_current: " << idx_current << ", fin.size(): " << fin.size() << "\n";
//
//    // Find: 'def'
//    size_t idx_def = fin.find("def ", idx_current);
////    if (idx_def > 0 && (idx_def == string::npos || !isWhitespace(fin[idx_def-1]))) {
//    if (idx_def == string::npos || (!isWhitespace(fin[idx_def-1]) && fin[idx_def-1] != charArchive)) {
//      fout += fin.substr(idx_current, fin.size()-idx_current);
//      idx_current = fin.size();
//      break;
//    }
////    cout << "[" << fin.substr(idx_def-10, 20) << "]\n";
//
//    // Find: '(', giving function name
//    size_t idx_lparen = fin.find("(", idx_def+4);
//    assert(idx_lparen != string::npos);
//    string functionName = fin.substr(idx_def+4, idx_lparen-(idx_def+4));
//    functionName = CPPUtils::strTrim(functionName);
////    cout << "functionName: " << functionName << endl;
////    if (functionName == "space_to_batch") {
////      cout << "Opportunity\n";
////    }
//
//    // Find: ':', giving end of arguments
//    size_t idx_colon = fin.find(":", idx_lparen);
//    assert(idx_colon != string::npos);
//    size_t idx_nl = fin.find("\n", idx_colon+1);
//    assert(idx_nl != string::npos);
//    size_t idx_1code_line = fin.find("\n", idx_nl+1);
//    if (idx_1code_line == string::npos) {
//      idx_1code_line = fin.size();
//    }
//    string firstLine = fin.substr(idx_nl+1, idx_1code_line - (idx_nl+1));
//    string firstLineEdit = CPPUtils::strReplace(firstLine, " ", ".");
////    cout << "[" << firstLineEdit << "]\n";
//
//    // Count indent spaces of first line
//    int indent = 0;
//    bool generate = true;
//    size_t idx_indent = idx_nl+1;
//    while (true) {
//      assert(fin[idx_indent] != '\t');
//      if (fin[idx_indent] == '\n') {
//        generate = false;
//        break;
//      }
//      if (fin[idx_indent] != ' ') break;
//      indent++;
//      idx_indent++;
//    }
//
//    // Insert tracer code
//    string pyCode;
//    fout += fin.substr(idx_current, idx_nl+1-idx_current);
//    idx_current = idx_nl + 1;
//    if (generate) {
//      int lineNumber = getLineNumber(fin, idx_current);
//      pyCode = generatePYCode(indent, functionName, fileName);
//      fout += pyCode;
//    }
//    // Back to finding next def
//  }
//  return fout;
//}

//void testPython() {
//  string s;
//  s += "def  hello (a,\n";
//  s += "            b)  :  \n";
//  s += "   print(\"Hello\")";
//
//  cout << "Python code:\n";
//  cout << compilePython(s, "Just Testing") << endl;
//}

int getLineNumber(string in, size_t idx_end) {
  int lineNumber = 1;
  size_t idx_current = 0;
  while (true) {
    size_t idx_nextEOL = in.find("\n", idx_current);
    if (idx_nextEOL != string::npos && idx_nextEOL < idx_end) {
      idx_current = idx_nextEOL + string("\n").size();
      lineNumber++;
    } else {
      break;
    }
  }
  return lineNumber;
}

//-------------------------------------------
bool isWhitespace(char c) {
  bool r = c == ' ' || c == '\n' || c == '\t' || c == charArchive;  // 0x02 as that is our encoded value
  return r;
}

bool isIdentifierCharacter(char c) {  // ':'=constructor/destructor, "<>"=templates
  return isalnum(c) || c == ':' || c == '_' || c == '<' || c == '>' || c == '~';
}

bool isReturnTypeCharacter(char c) {
  return isIdentifierCharacter(c) || c == '*' || c == '&';
}

//-------------------------------------------
size_t backwardsSkippingWhiteSpaces(string in, size_t idx) {
  while (isWhitespace(in[idx])) idx--;
  return idx;
}

//-------------------------------------------
size_t backwardsToNonIdentifier(string in, size_t idx) {
  // Allow '::' to include class name, if any
  // size_t orig_idx = idx;
  bool found = false;
  // ':' for scoping, '<', '>' for template specification
  while (isIdentifierCharacter(in[idx]) || in[idx] == '~') {  // '~' to support destructor
    found = true;
    if (idx == 0) return 0;
    idx--;
  }
  // Move up to last found alnum
  if (found) idx++;
  // cout << "backwardsToNonIdentifier: " << in.substr(idx, idx-orig_idx) << "\n";
  return idx;
}
size_t forwardToNonIdentifier(string in, size_t idx) {
  bool found = false;
  while (isIdentifierCharacter(in[idx]) || in[idx] == '~') {  // '~' to support destructor
    found = true;
    idx++;
  }
  if (!found) return string::npos;
  return idx;
}
size_t forwardToNonWhiteSpace(string in, size_t idx) {
  while (isWhitespace(in[idx])) {
    idx++;
  }
  return idx;
}



void populateIdentifiersAfter(unordered_map<string, void*>& m, string fin, string key) {
  size_t idx_scanner = 0;
  while(true) {
    size_t idx_class  = fin.find(key, idx_scanner);
    if (idx_class == string::npos) return;
//    cout << "Parsing: [" << fin.substr(idx_class, (fin.size()-idx_class > 20? 20 : fin.size()-idx_class)) << "]\n";
    idx_scanner = idx_class + key.size();
    while (isWhitespace(fin[idx_scanner])) idx_scanner++;
    size_t idx_scanner_end = idx_scanner;
    while (isalnum(fin[idx_scanner_end]) || fin[idx_scanner_end] == '_') idx_scanner_end++;
    
    // Ok, now backtrack, if character before is '<' or ',' then no class, a template thing
    size_t idx_trace_before = idx_scanner;
    idx_trace_before = fin.rfind(key, idx_trace_before);
    assert(idx_trace_before != string::npos);
    // Backtrack if not the beginning of string
    if (idx_trace_before > 0) {
      idx_trace_before--;
      // Backtrack until no whitespace
      while (isWhitespace(fin[idx_trace_before])) {
        idx_trace_before--;
      }
      // Real character before class declraration
      char tchar = fin[idx_trace_before];
      if (tchar != '<' && tchar != ',') {
        string c = fin.substr(idx_scanner, idx_scanner_end-idx_scanner);
        if (c == "T") {
          cout << "Opportunity\n";
          cout << fin.substr(idx_scanner-20, 40) << endl;
        }
        m[c] = nullptr;
      }
    }
    idx_scanner = idx_scanner_end;
//    cout << "Added class: [" << c << "]\n";
  }
}

//-------------------------------------------
const string MODE_TRACE = "TRACE";    // Insert trace code
const string MODE_TFBZL = "TFBZL";    // Modify tf/tf/tf.bzl to insert trace code in CC generation
const string MODE_REDUCE = "REDUCE";  // Take input and remove duplication
bool   codeGenOutputEmitted = false;
size_t codeGenInsertOffset = string::npos;
int main(int argc, char* argv[]) {

  string prefix = "";
  string fileName;
//  string mode(MODE_TRACE);
  string mode(MODE_REDUCE);
  bool useModifiedFilename = false;

  if (argc <= 1) {
    useModifiedFilename = true;
    fileName = "/Users/magnushyttsten/tmp/tmp.txt";
//    fileName = "/Users/magnushyttsten/tmp/python_start.txt";
//    fileName = "/Users/magnushyttsten/tmp/tmp.cc";
  }
  else {
    string usage = "Usage: [TRACE | TFBZL | REDUCE] <filename> [prefix-if-TRACE]\n";
    if (argc < 2) {
      cerr << usage;
      return 1;
    }
    mode = string(argv[1]);
    if (mode != MODE_TFBZL && mode != MODE_TRACE && mode != MODE_REDUCE) {
      assert(false);
    }
    
    if (argc >=3) {
      fileName = string(argv[2]);
    }
    
    if (argc >= 4) {
      prefix = string(argv[3]);
    }
  }

//  testPython();
 //  if (true) return -1;

  
  // TODO: Gets tricked to say function TF_LOCKS_EXCLUDED
  // Status SetGraph(Sqlite* db, uint64 now, double computed_time,
  //    std::unique_ptr<GraphDef> g) SQLITE_TRANSACTIONS_EXCLUDED(*db)
  //    TF_LOCKS_EXCLUDED(mu_) {
  // MHTracer_tensorflow_tensorflow_core_summary_summary_db_writer_cc mht_20(478,
  //    "tensorflow/tensorflow/core/summary/summary_db_writer.cc",
  //    "TF_LOCKS_EXCLUDED");

  set<string> skipFunctions {
    "TF_GUARDED_BY",
//    "IdAllocator",  // TODO: Support ': id1{}, id2{} {... (tensorflow/tensorflow/core/summary/summary_db_writer.cc)
//    "RunMetadata",   // TODO: Same as IdAllocator
//    "SeriesWriter",  // TODO: Same as IdAllocator
//    "RunWriter",  // TODO: Same as IdAllocator
//    "SummaryDbWriter",  // TODO: Same as IdAllocator
//    "CachedInterpolationCalculator",  // TOxDO: Same as above tensorflow/tensorflow/core/kernels/image/resize_bicubic_op.cc
//    "CSRSparseMatrix",  // TODO: As above, tensorflow/tensorflow/core/kernels/sparse/sparse_matrix.h
    "mutex::mutex",
    "mutex::lock",
    "mutex::unlock",
    "mutex::lock_shared",
    "mutex::unlock_shared",
    "mu_cast"
  };

  string skipFiles[] {
    "tensorflow/c/c_test.",
    "tensorflow/compiler/xla/python/tpu_driver/client/libtpu_client.",
    "tensorflow/lite/c/common.",
    "tensorflow/lite/c/c_test.",
    "tensorflow/lite/c/c_api_types.h",
    "tensorflow/lite/experimental/microfrontend/",
    "tensorflow/lite/micro/examples/micro_speech/",
    "tensorflow/lite/micro/examples/person_detection/",
    "tensorflow/lite/micro/kernels/xtensa_hifimini_staging/xa_nnlib/algo/kernels/",
    "tensorflow/lite/micro/tools/make/targets/ecm3531/",
    "tensorflow/lite/kernels/internal/types.h",  // Weird type for a non-constructor function named as a class
    "tensorflow/lite/micro/kernels/ceva/types.h",  // Strange copy of tf/lite/kernels/internal/types.h
    "tensorflow/tools/lib_package/",
    "tensorflow/python/saved_model/constants.py",  // Has a # ... def ... which confuses py parser to think it's a function
    "tensorflow/python/saved_model/signature_constants.py",  // As above
    "tensorflow/magnus",
    "tensorflow/core/util/bcast_test.cc",  // Declares a function named BCast, which is also a class :(
    "tensorflow/lite/delegates/gpu/common/task/serialization_base_generated.h"  // Weird: enum class . etc
  };
  
  string warnings;
//  cout << "MHTraceMaker, input file: " << fileName << "\n";
  for (auto s: skipFiles) {
    if (fileName.find(s) != string::npos) {
//      cout << "...SKIPPED: " << fileName << "\n";
      return -1;
    }
  }
  string fin = CPPUtils::fsFileReadToString(fileName);
  if (fin.size() == 0) {
    return -1;
  }

  string fout;
  if (mode == MODE_TFBZL) {
    string matcher = "str(include_internal_ops) + \" \" + api_def_args_str";
    size_t idx_matcher = fin.find(matcher);
    assert(idx_matcher != string::npos);
    fout = fin.substr(0, idx_matcher+matcher.size());
    fout += " +\n\" && \" +\n";
    fout += "\"MHTraceMaker " + MODE_TRACE + " $(location :\" + out_ops_file + \".h) GEN_CC\"";
    fout += " +\n\" && \" +\n";
    fout += "\"MHTraceMaker " + MODE_TRACE + " $(location :\" + out_ops_file + \".cc) GEN_CC\"";
    fout += fin.substr(idx_matcher+matcher.size(), fin.size() - (idx_matcher + matcher.size()));
    CPPUtils::fsFileWrite(fileName, fout);
    return 0;
  }
//  else if (mode == MODE_REDUCE) {
  else if (mode == MODE_TRACE) {
  }
  else {
    cout << "Reading rows from file\n";
    istringstream iss(fin);
    vector<string> v = CPPUtils::getLines(iss);
    iss.str("");
    iss.clear();
    cout << "...Done\n";
    cout << "Creating data structure\n";
    vector<MHReduction> vr;
    for (auto i=0; i < v.size(); i++) {
      auto e = v[i];
      string s = CPPUtils::strTrim(e);
      if (s.size() > 0) {
        vr.push_back(MHReduction(e));
      }
    }
    unsigned long countOriginalLines = vr.size();
    cout << "...Done\n";
    v.clear();

    cout << "Total number of lines in file: " << countOriginalLines << "\n";
    cout << "Processing reductions\n";
    int countReduction = 0;
    int countReducedLines = 0;
    for (int i=1; i < 100; i++) {
      vr = modeReduce(i, countReduction, countReducedLines, vr);
      cout << "...Did: " << i << ", countReduction: " << countReduction << ", countReducedLines: " << countReducedLines << "\n";
    }
//    vr = modeReduce(1, countReduction, countReducedLines, vr);
    cout << "...Done\n";
    cout << "Reductions: " << countReduction << endl;
    cout << "Calls eliminated: " << countReducedLines << ", from original: "  << countOriginalLines << "\n";
    
    stringstream ss;
    for (auto v: vr) {
      ss << v.getString();
    }
    string ss_str = ss.str();
    ss.str("");
//    cout << ss_str;
    CPPUtils::fsFileWrite("/tmp/python_start_reduced.txt", ss_str);
    return 0;
  }

  set<string> CC_EXT = {".cc", ".cpp"};
  bool isCC = false;
  set<string> H_EXT = {".h", ".hpp", ".hh"};
  bool isH = false;
  set<string> C_EXT = {".c"};
  bool isC = false;
  set<string> PY_EXT = {".py"};
  bool isPy = false;

  size_t idx_dot = fileName.rfind(".");
  assert(idx_dot != string::npos);
  string fileExtension = fileName.substr(idx_dot, fileName.size());
  if (CC_EXT.count(fileExtension) > 0) {
    isCC = true;
  }
  else if (H_EXT.count(fileExtension) > 0) {
    isH = true;
  }
  else if (C_EXT.count(fileExtension) > 0) {
    isC = true;
  }
  else if (PY_EXT.count(fileExtension) > 0){
    isPy = true;
//    fout = compilePython(fin, fileName);
//    decodeCommentsAndStringLiterals(fout);
//    fout = assignLineNumbers(fout);
//    if (useModifiedFilename) {
//      fileName += ".mod" + fileExtension;
//    }
//    CPPUtils::fsFileWrite(fileName, fout);
    return 0;
  }
  else {
    assert(false);
  }
  
  // Phase 0.0: Get rid of string literals and comments
  // While parsing, archive comment and string literals,
  // since they can contain things that confuse parsing
  assert(fin.find(charArchive) == string::npos);
  encodeCommentsAndStringLiterals(fin);

  // Phase 0.1: Keep all class & struct ids
  // We use this to see if a function name (within a class/struct declaration) is a constructor
  unordered_map<string, void*> classNames;
  populateIdentifiersAfter(classNames, fin, "class ");
  populateIdentifiersAfter(classNames, fin, "struct ");
//  cout << "Classnames\n";
//  for (auto e: classNames) {
//    cout << e.first << endl;
//  }
  
  // Add code immediately if CC, but after #ifndef... for headers
  string H_ifndef = "#ifndef ";
  string H_define = "#define ";
  string s;
  size_t idx_current = 0;
  if (isH) {
    string headerId;
    size_t idx_ifndef = fin.find(H_ifndef);
    if (idx_ifndef != string::npos) {
//      s = fin.substr(0, idx_ifndef);
//      cout << "[" << s << "]\n";
      
      idx_ifndef += H_ifndef.size();
      size_t idx_headerId = fin.find("\n", idx_ifndef);
      if (idx_headerId != string::npos) {
        string headerId = fin.substr(idx_ifndef, idx_headerId-idx_ifndef+1);
//        cout << "[" << headerId << "]\n";

        size_t idx_define = fin.find(H_define, idx_headerId+1);
        if (idx_define != string::npos) {
          idx_define += H_define.size();

          size_t idx_headerId2 = fin.find("\n", idx_define);
          if (idx_headerId2 != string::npos) {
            string headerId2 = fin.substr(idx_define, idx_headerId2-idx_define+1);
//            cout << "[" << headerId2 << "]\n";
            // We have ifndef and define of same token then move our defs right after those
            if (headerId == headerId2) {
              idx_current = idx_headerId2+1;
            }
          }
        }
      }
    }
  }
//  size_t added_characters = 0;  // Use this to calculate line number
  fout += fin.substr(0, idx_current);
//  if (isCC) {
  if (isH || isCC) {
    codeGenInsertOffset = fout.size();
//    CPPIndentWriter classDef;
//    string className = generateClass(classDef, fileName);
//    s = "\n" + classDef.getString() + "\n";
//    fout += s;
//    added_characters += s.size();
  }
  //  else if (isH || isC) {
  else if (isC) {
    assert(false);
//    s = "\n#include <stdio.h>\n";
//    s += "#include <stdlib.h>\n";
//    s += "#include <string.h>\n";
//    s += "#include <sys/types.h>\n";
//    s += "#include <unistd.h>\n";
//    fout += s;
//    added_characters += s.size();
  }

  int variableIndex = 0;
  set<string> reserved = { "do", "while", "for", "if", "switch", "return", "defined" "break" };  // #if defined
 
  while (idx_current < fin.size()) {
//    cout << "**********************\n";
//    cout << fout << "\n";
//    cout << "New round, line: "
//    << getLineNumber(fin, idx_current) + getArchivedLineNumbersTo(fin, idx_current)
//    << ", idx_current: " << to_string(idx_current) << ", fin.size: " << to_string(fin.size()) << "\n";
    
    size_t idx_lparen = fin.find("(", idx_current);
    size_t idx_macro = fin.find("#", idx_current);
    
    if (idx_macro != string::npos
        && (idx_lparen == string::npos || idx_macro < idx_lparen)) {
      string s = fin.substr(idx_current, idx_macro-idx_current);
      fout += s;
      idx_current = idx_macro;
      while (true) {
        size_t idx_nl = fin.find("\n", idx_current);
        if (idx_nl == string::npos) {
          s = fin.substr(idx_current, fin.size()-idx_current);
          fout += s;
          idx_current = fout.size();
          break;
        }
        s = fin.substr(idx_current, idx_nl-idx_current+1);
        fout += s;
        idx_current = idx_nl+1;
        size_t idx_nonws = backwardsSkippingWhiteSpaces(fin, idx_nl-1);
        if (fin[idx_nonws] != '\\') {  // Strip multi-line macros
          break;
        }
      }
    }
    // Are there no more '(' in our input, then we're done?
    else if (idx_lparen == string::npos){
      fout += fin.substr(idx_current, fin.size()-idx_current);
      break;
    }
    // Encountered '(', is this a function definition?
    else {
      bool valid = true;
      
      // Move to '('
      
      // *** 1. Find a '(' and scan to ')', even if we enounter nested '(', e.g. function pointer types
      //    valid on:      RegisterValidator(std::function<Status(const OpRegistryInterface&)> validator) { ... }
      //    But !valid on: tensorflow::unwrap(op)->AddInputList({...});
      size_t idx_rparen_p1 = idx_lparen+1;
      int rnestCount = 1;
      while (valid && rnestCount > 0) {
        if (fin[idx_rparen_p1] == '{')      valid = false;  // AddInputList({...});
        else if (fin[idx_rparen_p1] == '(') rnestCount++;
        else if (fin[idx_rparen_p1] == ')') rnestCount--;
        idx_rparen_p1++;  // Go to next symbol, if last ')' then move to the one after it
      }
      // Move beyond ')', or stop at symbol causing error '{'
      s = fin.substr(idx_current, idx_rparen_p1-idx_current);
      fout += s;
      idx_current = idx_rparen_p1;
      if (!valid) continue;
      
      // *** 2. functionName: Analyze functionName before idx_lparen '('
      size_t idx_functionName_last = backwardsSkippingWhiteSpaces(fin, idx_lparen-1);  // Search before '('
      size_t idx_functionName_first = string::npos;
      bool isLambda = true;
      string functionName = "lambda";
      if (fin[idx_functionName_last] != ']') {
        isLambda = false;
        idx_functionName_first = backwardsToNonIdentifier(fin, idx_functionName_last);
        functionName = fin.substr(idx_functionName_first, idx_functionName_last-idx_functionName_first+1);
        if (functionName.size() == 0 || reserved.count(functionName) > 0) {
          valid = false;
        }
      }
      if (!valid) continue;

      if (useModifiedFilename) {
        cout << "...functionName: [" << functionName << "]\n";
        if (functionName == ">IsAligned") {
          cout << "Opportunity\n";
        }
      }
//      if (functionName == "OperandShapeIterator::OperandShapeIterator") {
//        cout << "Opportunity\n";  // : [" << fin.substr(idx_functionName_last, 10) << "]\n";
//      }
      
      if (skipFunctions.count(functionName) > 0) {
        continue;
      }
      
      // *** 1.5: Analyze the return type part
      // Empty return type (or 'explicit') is ok if it's a constructor/destructor
      // Observe, subsequent analysis still needed, e.g: return new Foo();
      bool isConstructorOrDestructor = false;
      if (!isLambda) {
        // Get the return type string
        pair<bool, string> r;
        if (idx_functionName_first == 0) {
          r = {true, ""};
        } else {
          r = analyzeReturnType(fin, idx_functionName_first-1);
        }
        if (!r.first) {
          continue;
        }
        string returnType = r.second;
        
        // Detect if functionName is a constructor or destructor
        string exceptionalCase = "_ops.h";   // bazel-out/darwin-opt/bin/tensorflow/cc/ops/array_ops.h has Shape() function, which is also a class... ;(
        size_t coloncolon = functionName.find("::");
        if (coloncolon != string::npos) {
          string p1 = functionName.substr(0, coloncolon);
          string p2 = functionName.substr(coloncolon+2, functionName.size() - coloncolon -2);
          if (p1 == p2 || p1 == "~"+p2) {
            assert(returnType.size() == 0
                   || returnType.find("EIGEN_") != string::npos
                   || returnType.find("ABSL_") != string::npos
                   || returnType.find("TF_LITE_") != string::npos
                   || returnType.find("__") != string::npos
                   || returnType.find("PHILOX_") != string::npos
                   || returnType.find("TF_ATTRIBUTE_NORETURN") != string::npos
                   );
            isConstructorOrDestructor = true;
          }
        }
        else if ((classNames.count(functionName) > 0 || functionName.find("~") == 0)
                 &&
                 (fileName.find(exceptionalCase) + exceptionalCase.size() != fileName.size()
                     && functionName != "Shape")) {
          // TODO: This is TF specific, a generic product needs to have this through env variables, or accept size()>0
          // We won't skipping EIGEN_ specifications in returnValue, let's do that here
          // __host__ __device__ __forceinline__
          assert(returnType.size() == 0
                 || returnType.find("EIGEN_") != string::npos
                 || returnType.find("ABSL_") != string::npos
                 || returnType.find("TF_LITE_") != string::npos
                 || returnType.find("__") != string::npos
                 || returnType.find("PHILOX_") != string::npos
                 || returnType.find("TF_ATTRIBUTE_NORETURN") != string::npos
                 );
          isConstructorOrDestructor = true;
        }
        else if (returnType.size() == 0) {
          // TODO: Does solve for function mask_ but is there any corner case?
          // class DataTypeSet {
          //    constexpr DataTypeSet(const DataTypeSet& other) : mask_(other.mask_) {}
          // };
          continue;
        }
      }
        
      // *** 3. Check if we have a constructor initializer with ':'
      //    CPUSubAllocator(CPUAllocator* cpu_allocator) : SubAllocator({}, {}), C{}, cpu_allocator_(cpu_allocator) {}
      if (isConstructorOrDestructor && functionName.find("~") == string::npos) {
        size_t idx_lcb_scanner = constructorScanPastInitializers(fin, idx_rparen_p1);

        // Move forward if the lcb_scanning led to anything
        if (idx_rparen_p1 < idx_lcb_scanner) {
          s = fin.substr(idx_rparen_p1, idx_lcb_scanner-idx_rparen_p1);
          fout += s;
          idx_current = idx_lcb_scanner;
        }
      }
      
      // *** 4. Now, does a '{' come before a ';'
      size_t idx_semicolon = fin.find(";", idx_current);
      size_t idx_lcb = fin.find("{", idx_current);
      // Exit: No ';' nor any '{', weird but if so then we're done
      if (idx_semicolon == string::npos && idx_lcb == string::npos) {
        s = fin.substr(idx_current, fin.size() - idx_current);
        fout += s;
        idx_current = fin.size();
        continue;
      }
      // Exit: ';' occurs before '{', then no function definition, continue
      if (idx_semicolon != string::npos && (idx_lcb == string::npos || idx_semicolon < idx_lcb)) {
        s = fin.substr(idx_current, idx_semicolon-idx_current);
        fout += s;
        idx_current = idx_semicolon;
        continue;
      }
      // We might have a function definition
      // Explore dmz space between ')' and '{' to allow things that may be in a function definition
      string voidspace = fin.substr(idx_current, idx_lcb-idx_current);
      voidspace = CPPUtils::strTrim(voidspace);
      if (voidspace.length() > 0
          && voidspace != "override"
          && voidspace != "const"
          && voidspace != "const override"
          && voidspace != "final"
          && voidspace != "const final"
          && voidspace.find(":") != 0  // AConstructor(const char name[]) : builder_(name) {}
          && voidspace.find("TF_EXCLUSIVE_LOCKS_REQUIRED") != 0) {
        warnings += "   Warning ')' <unknown> {: " + voidspace + " [" + functionName + " " + fileName + "]\n";
        continue;
      }
      
      // Looks good, consume up to {
      s = fin.substr(idx_current, idx_lcb-idx_current+1);
      fout += s;
      idx_current = idx_lcb+1;

      // Insert trace code
      string outstr;

      // Scan parameter list: variableName, variableDecl, code
      tuple<string, string, string> variableAndCode = scanCPPVariableAndCode(variableIndex, fin, idx_lparen);
      outstr += "\n" + get<1>(variableAndCode);
      outstr += get<2>(variableAndCode);
      
      if (isH || isCC) {
        codeGenOutputEmitted = true;
        string className = codeGenGetClassName(fileName);
        outstr += "   " + className + " mht_" + to_string(variableIndex) + "(" + get<0>(variableAndCode) + ", " + MHTRACER_LINENUMBER_TAG + ", \"" + prefix + "\", \"" + fileName + "\", \"" + functionName + "\");\n";
        variableIndex++;
      }
      else if (isC) {
        assert(false);
        // outstr = "\n" + genPrintf(functionName, prefix, fileName) + "\n";
      }
      else {
        assert(false);
      }
      fout += outstr;
    }
  }
  
  // Only generate class information if at least 1 trace statement was generated
  if (codeGenOutputEmitted) {
    CPPIndentWriter classDef;
    string className = codeGenGenerateClass(classDef, fileName);
    s = classDef.getString() + "\n";
    fout.insert(codeGenInsertOffset, s);
  }
  decodeCommentsAndStringLiterals(fout);
  fout = assignLineNumbers(fout);
  if (useModifiedFilename) {
    fileName += ".mod" + fileExtension;
  }
  CPPUtils::fsFileWrite(fileName, fout);
}

//-----------------------------------------
string assignLineNumbers(const string& s) {
  string fout2;
  size_t idx_current = 0;
  while (true) {
    size_t idx_match = s.find(MHTRACER_LINENUMBER_TAG, idx_current);
    if (idx_match == string::npos) {
      fout2 += s.substr(idx_current, s.size()-idx_current);
      break;
    } else {
      fout2 += s.substr(idx_current, idx_match-idx_current);
    }
    int lineNumber = getLineNumber(s, idx_match)-1;
    string lineNumberStr = to_string(lineNumber);
    fout2 += lineNumberStr;
    idx_current = idx_match + MHTRACER_LINENUMBER_TAG.size();
  }
  return fout2;
}

////-----------------------------------------
//string genCTraceCode(const string& functionName, const string& prefix, const string& fileName1) {
//  string fileName = washFileName(fileName1);
//  CPPIndentWriter iw;
//  iw.println("char mhtracer_s[200];");
//  iw.println("char mhtracer_fn[40];");
//  iw.println("int mhtracer_indent = 0;");
//  iw.println("char* mhtracer_env_path = getenv(\"PATH\");");
//  iw.println("char mhtracer_env_indent_name[20];");
//  iw.println("sprintf(mhtracer_env_indent_name, \"%s%d\", \"" + ENV_MHTRACER_INDENT + "\", getpid());");
//  iw.println("if (getenv(mhtracer_env_indent_name)) {");
//  iw.println("   mhtracer_indent = atoi(mhtracer_env_indent_name) + 3;");
//  iw.println("}");
//  iw.println("char mhtracer_space[mhtracer_indent+1];");
//  iw.println("mhtracer_space[0] = '\\0';");
//  iw.println("int mhtracer_idx = 0;");
//  iw.println("while (mhtracer_idx < mhtracer_indent) {");
//  iw.println("   mhtracer_space[mhtracer_idx] = ' ';");
//  iw.println("   mhtracer_idx++;");
//  iw.println("   if (mhtracer_idx >= mhtracer_indent) mhtracer_space[mhtracer_idx] = '\\0';");
//  iw.println("}");
//  iw.println("if (!strstr(mhtracer_env_path, \"" + PATH_ENV_MHTRACER_DISABLE + "\")) {");
//  // We cannot support indents here, because we cannot track exit to decrement indent (as with C++ destructor)
//  iw.println("   sprintf(mhtracer_s, \"%s%s\", mhtracer_space, \"" + functionName + " " + prefix + " [h/c: " + MHTRACER_LINENUMBER_TAG + " @ " + fileName + "]\\n\");");
//  iw.println("   if (mhtracer_env_path && strstr(mhtracer_env_path, \"" + PATH_ENV_MHTRACER_FILE + "\")) {");
//  iw.println("      sprintf(mhtracer_fn, \"" + MHTRACER_FILENAME_PREFIX + "%d.log\", getpid());");
//  iw.println("      FILE* mhtracer_fp = fopen(mhtracer_fn, \"a\");");
//  iw.println("      fprintf(mhtracer_fp, \"%s\", mhtracer_s);");
//  iw.println("      fclose(mhtracer_fp);");
//  iw.println("   } else {");
//  iw.println("      printf(\"%s\", mhtracer_s);");
//  iw.println("   }");
//  iw.println("}");
//  return iw.getString();
//}

//-----------------------------------------
string codeGenGetClassName(const string& fileName) {
  string cname = fileName;
  cname = CPPUtils::strReplace(cname, "/", "PS");
  cname = CPPUtils::strReplace(cname, ".", "DT");
  cname = CPPUtils::strReplace(cname, "-", "SL");
  cname = "MHTracer_" + cname;
  return cname;
}
string codeGenGenerateClass(CPPIndentWriter& iw, const string& fileName) {
  iw.println("#include <iostream>"); // std::cout
  iw.println("#include <fstream>");
  iw.println("#include <thread>");   // std::thread, std::thread::id, std::this_thread::get_id
  iw.println("#include <chrono>");   // std::chrono::seconds
  iw.println("#include <string>");
  iw.println("#include <cstdlib>");
  iw.println("#include <sstream>");
  iw.println("#include <string>");
  iw.println("#include <vector>");
  iw.println("#include <stdlib.h>");
  iw.println("#include <unistd.h>");
  
  string className = codeGenGetClassName(fileName);
  iw.println("class " + className + " {");
  iw.println("public:");
  iw.println("   std::string _s;");
  iw.println("   int _indent=0;");
  iw.println("   std::string _functionName;");
  iw.println("   bool _isFile = false;");
  iw.println("   std::string _fileName;");
  iw.println("   std::string _envMHIndent;");
  iw.println("   int _lineNumber;");
  // Constructor
  iw.println("   " + className + "(std::vector<std::string> params, int lineNumber, std::string prefix, std::string fileName, std::string functionName) {");
  iw.println("      _functionName = functionName;");
  iw.println("      _lineNumber = lineNumber;");
  iw.println("");
  iw.println("      // Check if tracing is enabled");
  iw.println("      char* env_path = std::getenv(\"PATH\");");
  iw.println("      if (env_path != nullptr && std::string(env_path).find(\"" + PATH_ENV_MHTRACER_ENABLE + "\") == std::string::npos) {");
  iw.println("         return;");
  iw.println("      }");
  iw.println("");
  iw.println("");
  iw.println("      // Create log string");
  iw.println("      std::string ostr;");
  iw.println("");
  iw.println("      // Assign indent spaces (tied to PID and TID)");
  iw.println("      pid_t pid = getpid();");
  iw.println("      std::thread::id tid = std::this_thread::get_id();");
  iw.println("      std::stringstream pid_dash_tid_ss;");
  iw.println("      pid_dash_tid_ss << pid << \"-\" << tid;");
  iw.println("      std::string pid_dash_tid_str = pid_dash_tid_ss.str();");
  iw.println("      _envMHIndent = \"" + ENV_MHTRACER_INDENT + "\" + pid_dash_tid_str;");
  iw.println("      char* env_indent = std::getenv(_envMHIndent.c_str());");
  iw.println("      if (env_indent != nullptr) _indent = std::stoi(std::string(env_indent));");
  iw.println("      _s.assign(_indent, ' ');");
  iw.println("");
  iw.println("      // Check that reporting matches pid/tid");
  iw.println("      char* env_pid_dash_tid = std::getenv(\"" + ENV_PID_DASH_TID + "\");");
  iw.println("      if (env_pid_dash_tid != nullptr) {");
  iw.println("         std::string env_pid_dash_tid_str(env_pid_dash_tid);");
  iw.println("         if (env_pid_dash_tid_str != pid_dash_tid_str) {");
  iw.println("            ostr += \"* \";");   // ";)**PID/TID mismatch***: \" + env_pid_dash_tid_str + \", \" + pid_dash_tid_str;");
  iw.println("         }");
  iw.println("      }");
  iw.println("      setenv(\"" + ENV_PID_DASH_TID + "\", pid_dash_tid_str.c_str(), 1);");
  iw.println("");
  iw.println("      std::string paramStr;");
  iw.println("      for (int i=0; i < params.size(); i++) {");
  iw.println("         auto e = params[i];");
  iw.println("         while (e.find(\"\\n\") != std::string::npos) {");
  iw.println("            size_t pos = e.find(\"\\n\");");
  iw.println("            e = e.erase(pos, 1);");
  iw.println("            e = e.insert(pos, \"<NL>\");");
  iw.println("         }");
  iw.println("         while (e.find(\"[\") != std::string::npos) {");
  iw.println("            size_t pos = e.find(\"[\");");
  iw.println("            e = e.erase(pos, 1);");
  iw.println("            e = e.insert(pos, \"<LB>\");");
  iw.println("         }");
  iw.println("         while (e.find(\"]\") != std::string::npos) {");
  iw.println("            size_t pos = e.find(\"]\");");
  iw.println("            e = e.erase(pos, 1);");
  iw.println("            e = e.insert(pos, \"<RB>\");");
  iw.println("         }");
  iw.println("         paramStr += e;");
  iw.println("         if ((i+1) < params.size()) {");
  iw.println("            paramStr += \", \";");
  iw.println("         }");
  iw.println("      }");
  iw.println("      ostr += _s + functionName + ");
  iw.println("         + \" [1]\"");
  iw.println("         + \" [\" + prefix + \"]\"");
  iw.println("         + \" [\" + paramStr + \"]\"");
  iw.println("         + \" [\" + pid_dash_tid_str + \" \"");
  iw.println("         +    std::to_string(lineNumber)");
  iw.println("         +    \" @ \" + fileName + \"]\\n\";");
  iw.println("");
  iw.println("      // Log to file");
  iw.println("      if (env_path != nullptr && std::string(env_path).find(\"" + PATH_ENV_MHTRACER_FILE + "\") != std::string::npos) {");
  iw.println("         _isFile = true;");
  iw.println("         _fileName = \"" + MHTRACER_FILENAME_PREFIX + "\" + pid_dash_tid_str + \".log\";");
  iw.println("         std::ofstream os;");
  iw.println("         os.open(_fileName, std::ofstream::out | std::ofstream::app);");
  iw.println("         os << ostr << \"\";");
  iw.println("         os.close();");
  iw.println("      }");
  iw.println("      // Log to stdout");
  iw.println("      else {");
  iw.println("         std::cout << ostr << \"\";");
  iw.println("      }");
  iw.println("");
  iw.println("      // Increment indent spaces");
  iw.println("      _indent += 3;");
  iw.println("      setenv(_envMHIndent.c_str(), std::to_string(_indent).c_str(), 1);");
  iw.println("   }");
  // Destructor
  iw.println("   ~" + className + "() {");
  iw.println("      // Check if tracing is enabled");
  iw.println("      char* env_path = std::getenv(\"PATH\");");
  iw.println("      if (env_path != nullptr && std::string(env_path).find(\"" + PATH_ENV_MHTRACER_ENABLE + "\") == std::string::npos) {");
  iw.println("         return;");
  iw.println("      }");
  iw.println("      _indent -= 3;");
  iw.println("      setenv(_envMHIndent.c_str(), std::to_string(_indent).c_str(), 1);");
//  iw.println("      std::string ostr = _s + \"<-\" + functionName + ");
//  iw.println("         + \" [1]\"");
//  iw.println("         + \" []\"");
//  iw.println("         + \" []\"");
//  iw.println("         + \" []\\n\";");
//  iw.println("      std::cout << ostr << \"\";");
  iw.println("   }");
  // End of class
  iw.println("};");
  
  return className;
}

//-----------------------------------------
void decodeCommentsAndStringLiterals(string& fin) {
  int idx_archive = 0;
  
//  cout << "1. Size of acasl: " << archiveCommentsAndStringLiterals.size() << endl;
//  for (int i=0; i < archiveCommentsAndStringLiterals.size(); i++) {
//    string s = archiveCommentsAndStringLiterals[i].second;
//    int max = s.size();
//    if (max > 15) max = 15;
//    cout << "[" << to_string(i) << "]: " << s.substr(0, max) << endl;
//  }
  
  int idx = 0;
  while (true) {
    while (idx < fin.size() && fin[idx] != charArchive) {
      idx++;
    }
    if (idx == fin.size()) {
      return;
    }
    int start = idx;

    while (idx < fin.size() && fin[idx] == charArchive) {
      idx++;
    }
    int length = idx-start;
    while (length > 0) {
      int clength = (int)archiveCommentsAndStringLiterals[idx_archive].second.size();
      fin.replace(start, clength, archiveCommentsAndStringLiterals[idx_archive].second);
      idx_archive++;
      length -= clength;
      start += clength;
    }
    if (idx == fin.size()) { return; }
  }
}

size_t scanBeyondStringLiteral(string in, size_t posOfStartQuote);
size_t archiveAndMoveForward(string& fin, size_t start, size_t end);
void encodeCommentsAndStringLiterals(string& fin) {
  size_t idx_current = 0;
  while (idx_current < fin.size()) {
    string result;
    size_t idx_cml = fin.find("/*", idx_current);
    size_t idx_csl = fin.find("//", idx_current);
    size_t idx_sl = fin.find("\"", idx_current);
        
    // Multi-line comment
    if (idx_cml != string::npos
        && (idx_csl == string::npos || idx_cml < idx_csl)
        && (idx_sl == string::npos || idx_cml < idx_sl)) {
      size_t start = idx_cml;
      size_t end = fin.find("*/", start);
      idx_current = archiveAndMoveForward(fin, start, end+2);
    }
    // Single-line comment
    else if (idx_csl != string::npos
        && (idx_cml == string::npos || idx_csl < idx_cml)
        && (idx_sl == string::npos || idx_csl < idx_sl)) {
      size_t start = idx_csl;
      size_t end = fin.find("\n", start);
      if (end == string::npos) {
        end = fin.size()-1;
      }
      idx_current = archiveAndMoveForward(fin, start, end+1);
    }
    // String literal
    else if (idx_sl != string::npos
        && (idx_cml == string::npos || idx_sl < idx_cml)
        && (idx_csl == string::npos || idx_sl < idx_csl)) {
//      cout << "---------------------" << endl;
      size_t start = idx_sl;
//      cout << "1: " << fin.substr(start, 20) << endl;
      size_t end = scanBeyondStringLiteral(fin, start);
//      cout << "STRING_LITERAL[" << fin.substr(start, end-start) << "]" << endl;
      idx_current = archiveAndMoveForward(fin, start, end);
    }
    // No more matches for the 3, then we're done
    else {
      break;
    }
  }
}

size_t archiveAndMoveForward(string& fin, size_t start, size_t end) {
//  cout << "-----------\n";
//  if (start > 12640) {
//    cout << "@@: " << fin.substr(start, 10);
//    cout << "This is your chance\n";
//  }
  string s1 = fin.substr(start, end-start);
  archiveCommentsAndStringLiterals.push_back({start, s1});
  string s2;
  s2.assign(end-start, charArchive);
  fin.replace(start, end-start, s2);
  assert(s1.size() == s2.size());
  size_t next = end;
//  if (next < fin.size()) {
//    cout << "Next: " << fin.substr(next, (fin.size()-next>10) ? 10: (fin.size()-next)) << endl;
//  }
  return next;
}

size_t scanBeyondStringLiteral(string in, size_t posOfStartQuote) {
  // In case we have '"', then there is no matching ", just encode the single "
  if ((in[posOfStartQuote-1] == '\'' && in[posOfStartQuote+1] == '\'')
      || (in[posOfStartQuote-2] == '\'' && in[posOfStartQuote-1] == '\\' && in[posOfStartQuote+1] == '\'')) {
    return posOfStartQuote+1;
  }
  
  // Multi-line using 'R'
  if (in[posOfStartQuote-1] == 'R') {
    size_t lparen = in.find("(", posOfStartQuote+1);
    string id = in.substr(posOfStartQuote+1, lparen-posOfStartQuote-1);
    string closingString = ")" + id + "\"";
//    cout << "id: " << id << ", closingString: " << closingString << endl;
    size_t idx_charAfter = in.find(closingString, lparen+1) + closingString.size();
//    cout << "after it all: " << in.substr(idx_charAfter, 10) << endl;
    assert(idx_charAfter != string::npos);
//    cout << "CAPUTURED[" << in.substr(posOfStartQuote, idx_charAfter-posOfStartQuote) << "]\n";
//    cout << "NEXT[" << in.substr(idx_charAfter, 5) << "]\n";
    return  idx_charAfter;
  }
  
  size_t nextQuote = posOfStartQuote+1;
  while (true) {
    nextQuote = in.find("\"", nextQuote);
//    if (scanCurr == string::npos) {
//      char scB = in[posOfStartQuote-1];
//      char scA = in[posOfStartQuote+1];
//      assert(scB == '\'' && scA == '\'');
//      return posOfStartQuote+1;
//    }
    assert(nextQuote != string::npos);  // '"' covered at top of function
    int backSlash = 0;
    size_t scanCurr = nextQuote-1;
    char scanChar = in[scanCurr];
    while (scanChar == '\\') {
      scanCurr--;
      backSlash++;
      scanChar = in[scanCurr];
    }
    // Even count, then '\' is escaping itself, not the "
    if ((backSlash % 2) == 0) {
      break;
    } else {
      nextQuote++;
    }
  }
  
  return nextQuote+1;
}

size_t constructorScanPastInitializers(string in, size_t idx_start) {
  // Entering this a constructor is verified, and idx_start is position after closing ')'
  
  // Scan beyond ':' or return
  size_t idx_lcb_scanner = forwardToNonWhiteSpace(in, idx_start);
  if (in[idx_lcb_scanner] != ':') {
    return idx_lcb_scanner;
  }
  idx_lcb_scanner++;

  // Scanned byond ':', now expecting: id(arg), id{arg} { ... } before {
  while (true) {
    idx_lcb_scanner = forwardToNonWhiteSpace(in, idx_lcb_scanner);
    idx_start = idx_lcb_scanner;
    
    // Scan past id, this can be extremely complicated
    // E.g: OperandShapeIterator::OperandShapeIterator(Operation::operand_iterator it) :
    //      llvm::mapped_iterator<Operation::operand_iterator,llvm::Optional<ArrayRef<int64_t>> (*)(Value)>(it, &GetShape)
    //      , A {}, B(C{})
    //      { insert_trace_here; }
    while (true) {
      char c = in[idx_lcb_scanner];
      if (!isIdentifierCharacter(c) && c != '(' && c != '{' && c != '<') {
        break;
      }
      // Resolve a nesting situation
      set<pair<char,char>> nesting = { {'(',')'}, {'{','}'}, {'<','>'} };
      bool nest = false;
      for (auto e: nesting) {
        if (c == e.first) {
          idx_lcb_scanner = scanBeyondNesting(in, idx_lcb_scanner+1, e.first, e.second);
          nest = true;
        }
      }
      // No nesting, then just advance
      if (!nest) {
        idx_lcb_scanner++;
      }
    }
    
    // Done with scanning identifier
    string argument;
    if (idx_start < idx_lcb_scanner) {
      argument = CPPUtils::strTrim(in.substr(idx_start, idx_lcb_scanner-idx_start), {charArchive});
    }
//    cout << "Argument: [" << argument << "]\n";
//    cout << "Next 5:   [" << in.substr(idx_lcb_scanner, 5) << "]\n";

    // Next character must be ','=repeat, or '{'=constructor definition, or ';'=something weird but ok...
    idx_lcb_scanner = forwardToNonWhiteSpace(in, idx_lcb_scanner);
    // End reached
    if (in[idx_lcb_scanner] == '{' || in[idx_lcb_scanner] == ';') {
      return idx_lcb_scanner;
    }
    // If next is a ',', then continue process next argument list
    else if(in[idx_lcb_scanner] == ',') {
//      cout << "Opportunity: [" << in.substr(idx_lcb_scanner, 20) << "]\n";
      idx_lcb_scanner++;
    }
    // Risky assumption: Character is something unknown, then we probably consumed body {} as part of argument list
    // Backtrack to last { and assume it's the construction body
    // E.g: OperandShapeIterator::OperandShapeIterator(Operation::operand_iterator it) :
    //      llvm::mapped_iterator<Operation::operand_iterator,llvm::Optional<ArrayRef<int64_t>> (*)(Value)>(it, &GetShape)
    //      {}
    else {
      idx_lcb_scanner--;
      while (in[idx_lcb_scanner] != '{') {
        idx_lcb_scanner--;
      }
      return idx_lcb_scanner;
    }
  }
}

// Return idx_charEndP1
size_t scanBeyondNesting(string in, size_t idx_charStartP1, char nestCharStart, char nestCharEnd) {
  int count = 1;
  while (true) {
    char c = in[idx_charStartP1];
    if (c == nestCharStart) count++;
    if (c == nestCharEnd) count--;
    idx_charStartP1++;
    if (count == 0 || idx_charStartP1 >= in.size()) {
      break;
    }
  }
  return idx_charStartP1;
}

pair<bool, string> analyzeReturnType(const string& fin, size_t idx_functionName_first_m1) {
  size_t idx_start = idx_functionName_first_m1;
  
  // Scan return type
  // : as in public:, class A { A() {} }, class B { int foo(); }
  set<char> terminator {';', '}', ':', '{'};
  set<char> allowed {'_', '&', '>', '<', ':', '*'};
  bool valid = true;
  while (true) {
    char c = fin[idx_start];
    if (idx_start <= 0 || terminator.count(c) > 0) {
      break;
    }
    if (allowed.count(c) == 0 && !isWhitespace(c) && !isalnum(c)) {
      valid = false;
      break;
    }
    idx_start--;
  }
  
  // Found invalid characters, no function definition
  if (!valid) return {false, ""};
  
  // Found terminator, but this could also be the first function definition in file (idx_start)
  string returnPart;
  if (terminator.count(fin[idx_start]) > 0 || idx_start == 0) {
    idx_start = idx_start == 0 ? idx_start : idx_start+1;
    returnPart = fin.substr(idx_start, idx_functionName_first_m1+1-idx_start);
    returnPart = CPPUtils::strTrim(returnPart, {charArchive});
    
    // Perform some rudimentary discard analysis
    string rpAnalysis = " " + returnPart + " ";
    rpAnalysis = CPPUtils::strReplace(rpAnalysis, "\n", " ");
    set<string> forbidden = {" constexpr ", " return ", " new "};
    for (auto e: forbidden) {
      if (rpAnalysis.find(e) != string::npos) {
        return {false, ""};  // Cannot trace constexpr functions (tensorflow/tensorflow/lite/kernels/internal/common.h)
      }
    }
  }
  
  // Remove some normalities (in particular for constructor/destructor analysis later
  // Dragons can be found here: "struct alignas(8) LargeCounts {" with function name: alignas
  set<string> ignore = {" explicit ", " virtual ", " template ", " inline ", " struct ", " class "};
  string rpAnalysis = " " + returnPart + " ";
  rpAnalysis = CPPUtils::strReplace(rpAnalysis, "\n", " ");
  bool isTemplate = rpAnalysis.find(" template ") != string::npos;
  for (auto e: ignore) {
    rpAnalysis = " " + CPPUtils::strReplace(rpAnalysis, e, "") + " ";
  }
  if (isTemplate) {  // Then remove '<' '>'
    size_t idx_begin = 0;
    while (rpAnalysis[idx_begin] != '<') idx_begin++;
    idx_begin++;
    char c_begin = rpAnalysis[idx_begin];
    size_t idx_end = scanBeyondNesting(rpAnalysis, idx_begin, '<', '>');
    idx_begin--;
    idx_end--;
    string newRPAnalysis;
    for (int i=0; i < rpAnalysis.size(); i++) {
      if (i < idx_begin || i > idx_end) {
        newRPAnalysis += rpAnalysis[i];
      }
    }
    rpAnalysis = newRPAnalysis;
  }
  returnPart = CPPUtils::strTrim(rpAnalysis);

  // Crude: If this ends with > then take that out assuming a template thing
  // TODO: Could be greatly improved though but with a lot of complexity
  // struct Initializer {
  //    template <typename T, typename = typename std::enable_if<
  //                         std::is_arithmetic<T>::value || std::is_convertible<T, string>::value>::type>
  //       Initializer(const T& v, const TensorShape& shape) {
  if (returnPart[returnPart.size()-1] == '>') {
    returnPart = "";
  }
  
  // Observe, returnPart == "" is ok, but if so must be constructor/destructor
  return {true, returnPart};
}

tuple<string, string, string> scanCPPVariableAndCode(int variableIndex, const string& in, size_t idx_lparen) {
  string variableName = "mht_" + to_string(variableIndex) + "_v";
  string variableDecl = "   std::vector<std::string> " + variableName + ";\n";
  string variableAdd = "   " + variableName + ".push_back(";

  size_t idx_lparenP1 = idx_lparen+1;
  size_t idx_end = scanBeyondNesting(in, idx_lparenP1, '(', ')');
  idx_end--;
  if (idx_end == idx_lparenP1) {
    return {variableName, variableDecl, ""};
  }
  string paramList = in.substr(idx_lparenP1, idx_end-(idx_lparenP1));
  paramList = CPPUtils::strTrim(paramList);
  if (paramList.size() == 0) {
    return {variableName, variableDecl, ""};
  }

  // This is a rudimentary scan looking for string based parameters only
  stringstream code;
  stringstream sink;
  vector<string> v = CPPUtils::strSplit(paramList, ",");  // Ignore all kinds of complexities like function pointers
  if (v.size() == 0)  return {variableName, variableDecl, ""};
  for (auto e: v) {
    bool toProcess = true;
    e = CPPUtils::strTrim(e, {charArchive});
    // Remove all charArchive, newlines, and double spaces from e
    int idx_ca = 0;
    string newe = "";
    while (idx_ca < e.size()) {
      if (e[idx_ca] != charArchive && e[idx_ca] != '\n') {
        newe += string().assign(1, e[idx_ca]);
      } else {
        newe += " ";
      }
      idx_ca++;
    }
    e = CPPUtils::strTrim(newe, {0x02});
    idx_ca = 0;
    newe = "";
    bool isSpaceMode = false;
    while (idx_ca < e.size()) {
      if (e[idx_ca] != ' ' || !isSpaceMode) {
        newe += string().assign(1, e[idx_ca]);
      }
      if (e[idx_ca] == ' ') {
        isSpaceMode = true;
      } else {
        isSpaceMode = false;
      }
      idx_ca++;
    }
    e = CPPUtils::strTrim(newe, {0x02});

    // TODO: This is scary int foo(const char /*a*/) { ... }  tf/tf/compiler/mlir/hlo/lib/Dialect/mhlo/transforms/legalize_to_linalg.cc
    
    // Strip default arguments
    size_t idx_assign = e.find("=");
    if (idx_assign != string::npos) {
      e = e.substr(0, idx_assign);
    }
    
    string pname;
    string ptype;
    if (toProcess) {
      size_t idx_pnameFirst = e.size()-1;
      while (idx_pnameFirst > 0
             && e[idx_pnameFirst] != ' '
             && idx_pnameFirst != charArchive
             && isIdentifierCharacter(e[idx_pnameFirst])) {
        idx_pnameFirst--;
      }
      
      // void foo(int) { ... }  exists in TF source code tree
      // void (*l)(char*) { [tensorflow/tensorflow/core/framework/logging.cc]
      if (idx_pnameFirst == 0
          || idx_pnameFirst == e.size()-1) {
        toProcess = false;
      }
      else {
        idx_pnameFirst++;
        pname = CPPUtils::strTrim(e.substr(idx_pnameFirst, e.size()-idx_pnameFirst), {0x02});
      }

      if (toProcess) {
        ptype = CPPUtils::strTrim(e.substr(0, idx_pnameFirst), {0x02});
      }
    }
    // Sanity check pname to avoid: int foo(const char &) { ... }  tf/tf/compiler/mlir/hlo/lib/Dialect/mhlo/transforms/legalize_to_linalg.cc
    if (toProcess) {
      set<string> reserved { "char", "int", "const", "float", "double", "string" };  // Basic protection, many more possible
      if (CPPUtils::strCountOccurrences(pname, "&") > 0
          || CPPUtils::strCountOccurrences(pname, "*") > 0
          || CPPUtils::strCountOccurrences(pname, ">") > 0
          || CPPUtils::strCountOccurrences(pname, ":") > 0
          || reserved.count(pname) >0)
        toProcess = false;
    }
    
    // Rudimentary type analysis
    // Allow string, but not '&' unless also 'const', and never if it contains '*'
    bool processed = false;
    if (toProcess &&
        (CPPUtils::strEndsWith(ptype,    "const string&")
         || CPPUtils::strEndsWith(ptype, "const std::string&")
         || CPPUtils::strEndsWith(ptype, "const string &")
         || CPPUtils::strEndsWith(ptype, "const std::string &")
         || CPPUtils::strEndsWith(ptype, "const string")
         || (CPPUtils::strEndsWith(ptype, "string") && !CPPUtils::strEndsWith(ptype, "tstring"))
         || CPPUtils::strEndsWith(ptype, "std::string"))) {
      code << variableAdd  << "\"" << pname << ": \\\"\" + " << pname << " + \"\\\"\");\n";
      processed = true;
    }
    else if (toProcess &&
        (CPPUtils::strEndsWith(ptype, "const tstring&")
         || CPPUtils::strEndsWith(ptype, "const tstring &")
         || CPPUtils::strEndsWith(ptype, "const tstring")
         || CPPUtils::strEndsWith(ptype, "tstring"))) {
      code << variableAdd << "\"" << pname << ": \\\"\" + (std::string)" << pname << " + \"\\\"\");\n";
      processed = true;
    }
    else if (toProcess &&
             (CPPUtils::strEndsWith(ptype,    "const string_view&")
              || CPPUtils::strEndsWith(ptype, "const std::string_view&")
              || CPPUtils::strEndsWith(ptype, "const string_view &")
              || CPPUtils::strEndsWith(ptype, "const std::string_view &")
              || CPPUtils::strEndsWith(ptype, "const string_view")
              || CPPUtils::strEndsWith(ptype, "const std::string_view")
              || CPPUtils::strEndsWith(ptype, "string_view")
              || CPPUtils::strEndsWith(ptype, "const absl::string_view&")
              || CPPUtils::strEndsWith(ptype, "const absl::string_view &")
              || CPPUtils::strEndsWith(ptype, "const absl::string_view")
              || CPPUtils::strEndsWith(ptype, "absl::string_view"))) {
      code << variableAdd << "\"" << pname << ": \\\"\" + std::string(" << pname << ".data(), " << pname << ".size()) + " << "\"\\\"\");\n";;
      processed = true;
    }
    else if (toProcess) {
      // Allow char* or char
      if ((CPPUtils::strEndsWith(ptype, "const char*")
           || CPPUtils::strEndsWith(ptype, "const char *")
           || CPPUtils::strEndsWith(ptype, "char*")
           || CPPUtils::strEndsWith(ptype, "char *"))
           && !CPPUtils::strEndsWith(ptype, "char **")) {
        code << variableAdd << "\"" << pname << ": \\\"\" + (" << pname << " == nullptr ? std::string(\"nullptr\") : std::string((char*)" << pname << ")) + \"\\\"\");\n";
        processed = true;
      } else if (CPPUtils::strEndsWith(ptype, "char")) {  // Support for char only
        code << variableAdd << "\"" << pname << ": \'\" + std::string(1, " << pname << ") + \"\'\");\n";
        processed = true;
      }
    }
  }
  return {variableName, variableDecl, code.str()};
}

// **********************************************************
vector<MHReduction> modeReduce(int windowSize,
                               int& countReductions,
                               int& countReducedLines,
                               const vector<MHReduction>& vIn2) {
  vector<MHReduction> w;
  vector<MHReduction> vIn = vIn2;
  vector<MHReduction> vOut;

  int idx_v = 0;
  int matchHappenedCount = 0;
  while (true) {
    // Cleanup if we don't have enough data left for a window(*2) pass
    unsigned long needed = windowSize-w.size() + windowSize + idx_v;
    unsigned long available = vIn.size();
    if (needed > available) {
      // Commit window that potentially has reductions
      for (auto e: w) {
        vOut.push_back(e);
      }
      // Commit leftovers in v
      while (idx_v < vIn.size()) {
        auto e = vIn[idx_v];
        vOut.push_back(e);
        idx_v++;
      }
      break;
    }

    // We have data to do a window pass
    // Fill window
    // TODO: Fails if run out of data due to comments
    while (w.size() < windowSize) {
      auto mr = vIn[idx_v];
      w.push_back(mr);
      idx_v++;
    }
    
    // Detect if window matches
    bool matchNow = true;
    vector<MHReduction> testersAgainstW;
    // TODO: Fails if run out of data due to comments
    int idx_0 = 0;
    while (testersAgainstW.size() < windowSize) {
      auto mr = vIn[idx_0+idx_v];
      testersAgainstW.push_back(mr);
      idx_0++;
    }
    for (auto i=0; i < windowSize; i++) {
      MHReduction mrw = w[i];
      MHReduction mrv = testersAgainstW[i];
      if (mrw != mrv) {
        matchNow = false;
        break;
      }
    }
    
    // No match
    if (!matchNow) {
      // If this window never matched, then consume and move forward 1
      if (matchHappenedCount == 0) {
        vOut.push_back(w[0]);
        w.erase(w.begin());
      }
      // Window has matched at least once, then cancel this window and move to next mismatch
      else {
        for (auto e: w) {
          vOut.push_back(e);
        }
        w.clear();
        matchHappenedCount = 0;
      }
    }
    // Match, consume window and retain it
    // We only commit window to out (above) when we have encounter first mismatch or no data left
    else {
      matchHappenedCount++;
      countReductions++;
      countReducedLines += w.size();
      idx_v += windowSize;
      assert(w.size() == testersAgainstW.size());
      // Merge into window object
      for (auto i=0; i < w.size(); i++) {
        w[i].countOccurence += testersAgainstW[i].countOccurence;
        for (auto p: testersAgainstW[i].params) {
          w[i].params.push_back(p);
        }
      }
    }
  }
  return vOut;
}
