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
#include "CPPUtils.h"
#include "CPPIndentWriter.h"

// TODO: Fixes and extensions
// - Check CSRSparseMatrix is not skipFunction filtered causing metadata_{} errors
//   tensorflow/tensorflow/core/kernels/sparse/sparse_matrix.h
//   Observe however, that you intercept metadata_() non-function in other CSRSparseMatrix constructors
//   Probably need a way to parse member initializers after :...
//   Related to: Also gives support for others, e.g "IdAllocator" ': id1{}, id2{} {...
//   (tensorflow/tensorflow/core/summary/summary_db_writer.cc)
// - Python does not skip comments and whitespaces, could it lead to errors?

static string MHTRACER_LINENUMBER_TAG = "MHTRACER_LN_TAG";
static string ENV_MHTRACER_INDENT = "MHTRACER_INDENT_";  // Tied to PID
static string PATH_ENV_MHTRACER_FILE = "MHTRACER_USEFILE";   // Set this a part of PATH variable to write to file
static string PATH_ENV_MHTRACER_DISABLE = "MHTRACER_DISABLE";   // Set this a part of PATH variable to disable tracing
static string MHTRACER_FILENAME_PREFIX = "/tmp/mhtracer_";

// ****************************************************************
vector<pair<size_t,string>> archiveCommentsAndStringLiterals;
const char charArchive = '@';
// const char charArchive = 0x02;


int getLineNumber(string in, size_t idx_end);
string generateClass(CPPIndentWriter& iw, string fileName);
void encodeCommentsAndStringLiterals(string&);
void decodeCommentsAndStringLiterals(string& fin);
string genPrintf(const string& functionName, const string& prefix, const string& fileName);


string washFileName(string fname) {
  string fileName = CPPUtils::strReplace(fname, "tensorflow", "tf");
  fileName = CPPUtils::strReplace(fileName, "framework", "fw");
  return fileName;
}

string generatePYCode(int indent, int lineNumber, string functionName, string fileName) {
  string printGuts  = functionName + " [PY @ " + to_string(lineNumber) + " @ " + functionName + " @ " + fileName + "]";

  CPPIndentWriter iw;
  string istr;
  istr.assign(indent, ' ');
  iw.println(istr + "env_indent_variable = '" + ENV_MHTRACER_INDENT + "' + str(os.getpid())");
  iw.println(istr + "indent = os.environ.get(env_indent_variable)");
  iw.println(istr + "if indent is None:");
  iw.println(istr + "   indent = 0");
  iw.println(istr + "indent = int(indent) * \" \"");
//  iw.println(istr + "indent = \"\"");
  iw.println(istr + "envPath = os.environ.get('PATH')");
  iw.println(istr + "if envPath is not None and '" + PATH_ENV_MHTRACER_DISABLE + "' in envPath:");
  iw.println(istr + "   pass");
  iw.println(istr + "elif envPath is None or '" + PATH_ENV_MHTRACER_FILE + "' not in envPath:");
//  iw.println(istr + "isFile = os.environ.get('" + ENV_MHTRACER_FILE + "')");
//  iw.println(istr + "if isFile is None:");
  iw.println(istr + "   print(indent, \"" + printGuts + "\")");
  iw.println(istr + "elif envPath is not None and '" + PATH_ENV_MHTRACER_FILE + "' in envPath:");
  iw.println(istr + "   fileName = \"" + MHTRACER_FILENAME_PREFIX + "\" + str(os.getpid()) + \".log\"");
  iw.println(istr + "   fp = open(fileName, \"a\");");
  iw.println(istr + "   fp.write(indent + \"" + printGuts + "\")");
  iw.println(istr + "   fp.close()");
  return iw.getString();
}

bool isWhitespace(char c);

// Returns [idx_start, index] of the matching element
pair<size_t, int> findFirst(string in, const vector<string>& v, size_t idx_start, bool scanBeyondMatch=false) {
  pair<size_t, int> winner(string::npos, -1);
  
  // Check that we have descending order, needed to get best match
  assert(v.size() > 0);
  int longest = -1;
  for (auto s: v) {
    if (longest == -1)
      longest = (int)s.size();
    else {
      assert(longest >= s.size());
      longest = (int)s.size();
    }
  }
  
  // Now scan for best match
  size_t idx_v = 0;
  while (idx_v < v.size()) {
    size_t idx = in.find(v[idx_v], idx_start);
    if (idx == string::npos) {
      idx_v++;
      continue;
    }
    if (winner.first == string::npos) {
      winner =  pair<size_t, int>(idx, idx_v);
      idx_v++;
      continue;
    }
    // If first that happens ever, then it's the winner (we wouldn't start from 0 multiple times)
    if (idx == 0) {
      winner = pair<size_t, int>(idx, idx_v);
      idx_v++;
      continue;
    }
    // Track back any escape situations
    if (in[idx-1] == '\\') {
      int count = 0;
      size_t tmp = idx;
      while (tmp >= 0 && in[tmp] == '\\') {
        idx--;
        count++;
      }
      // There is an escape situation
      if (count % 2 != 0) {
        idx_start = idx+1;
        continue;  // We continue with current, since it might come immediately after escaped char
      }
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
string compilePython(string fin, string fileName) {
  fileName = washFileName(fileName);

  size_t idx_current = 0;

  // Replace all comments
  idx_current = 0;
  vector<string> kws = {"\"\"\"", "\'\'\'", "#", "\"", "\'"};
  while (true) {
    pair<size_t, int> idx_start = findFirst(fin, kws, idx_current, false);
    if (idx_start.first == string::npos) break;
    pair<size_t, int> idx_end = pair<size_t, int>(string::npos, 0);

    unsigned long size_end = 0;
    switch (idx_start.second) {
      case 0:
        idx_end = findFirst(fin, {kws[0]}, idx_start.first+kws[0].size(), true);
        size_end = kws[0].size();
        break;
      case 1:
        idx_end = findFirst(fin, {kws[1]}, idx_start.first+kws[1].size(), true);
        size_end = kws[1].size();
        break;
      case 2:
        idx_end = findFirst(fin, {"\n"}, idx_start.first+1, true);
        size_end = string("\n").size();
        break;
      case 3:
        idx_end = findFirst(fin, {"\""}, idx_start.first+1, true);
        size_end = string("\"").size();
        break;
      case 4:
        idx_end = findFirst(fin, {"\'"}, idx_start.first+1, true);
        size_end = string("\'").size();
        break;
      default:
        assert(false);
    }
    string data = fin.substr(idx_start.first, idx_end.first-idx_start.first);
    if (data.find("# this is the") == 0) {
      cout << "Opportunity\n";
    }
    idx_current = archiveAndMoveForward(fin, idx_start.first, idx_end.first);
    if (idx_current == string::npos || idx_current >= fin.size())
      break;
  } // End-of replace all comments and string literals
  
  // First, import os after from __future__ statements
  cout << "Archive [" << "]\n";
  idx_current = 0;
  string fout;
  string c_from_future = "from __future__ ";
  size_t idx_c = fin.rfind(c_from_future);
//  cout << "[" << fin.substr(idx_c-5, 20) << "]\n";
  if (idx_c != string::npos) {
    size_t idx_c2 = fin.find("\n", idx_c);
//    cout << "[" << fin.substr(idx_c, idx_c2-idx_c) << "]\n";
//    cout << "[" << fin.substr(idx_c2+1, 20) << "]\n";
    if (idx_c2 != string::npos) {
      fout += fin.substr(0, idx_c2+1);
      idx_current = idx_c2+1;
    } else {
      fout += fin.substr(0, fin.size());
      idx_current = fin.size();
    }
    fout += "import os\n";
  }
    
  while (idx_current < fin.size()) {
    size_t idx_def = fin.find("def ", idx_current);
    if (idx_def > 0 && (idx_def == string::npos || !isWhitespace(fin[idx_def-1]))) {
      fout += fin.substr(idx_current, fin.size()-idx_current);
      idx_current = fin.size();
      break;
    }
//    cout << "[" << fin.substr(idx_def-10, 20) << "]\n";
    
    size_t idx_lparen = fin.find("(", idx_def+4);
    assert(idx_lparen != string::npos);
    string functionName = fin.substr(idx_def+4, idx_lparen-(idx_def+4));
    functionName = CPPUtils::strTrim(functionName);
//    cout << "functionName: " << functionName << endl;
        
    size_t idx_colon = fin.find(":", idx_lparen);
    assert(idx_colon != string::npos);
    size_t idx_nl = fin.find("\n", idx_colon+1);
    assert(idx_nl != string::npos);
    size_t idx_1code_line = fin.find("\n", idx_nl+1);
    if (idx_1code_line == string::npos) {
      idx_1code_line = fin.size();
    }
    string firstLine = fin.substr(idx_nl+1, idx_1code_line - (idx_nl+1));
   
    int indent = 0;
    bool generate = true;
    size_t idx_indent = idx_nl+1;
    while (true) {
      assert(fin[idx_indent] != '\t');
      if (fin[idx_indent] == '\n') {
        generate = false;
        break;
      }
      if (fin[idx_indent] != ' ') break;
      indent++;
      idx_indent++;
    }
    
    string pyCode;
    fout += fin.substr(idx_current, idx_nl+1-idx_current);
    idx_current = idx_nl + 1;
    if (generate) {
      int lineNumber = getLineNumber(fin, idx_current);
      pyCode = generatePYCode(indent, lineNumber, functionName, fileName);
      fout += pyCode;
    }
  }
  
  
  
  
  return fout;
}

void testPython() {
  string s;
  s += "def  hello (a,\n";
  s += "            b)  :  \n";
  s += "   print(\"Hello\")";
  
  cout << "Python code:\n";
  cout << compilePython(s, "Just Testing") << endl;
}

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
  return c == ' ' || c == '\n' || c == '\t';
}

bool isIdentifierCharacter(char c) {  // ':'=constructor/destructor, "<>"=templates
  return isalnum(c) || c == ':' || c == '_' || c == '<' || c == '>';
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
    idx--;
  }
  // Move up to last found alnum
  if (found) idx++;
  // cout << "backwardsToNonIdentifier: " << in.substr(idx, idx-orig_idx) << "\n";
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
    string c = fin.substr(idx_scanner, idx_scanner_end-idx_scanner);
    m[c] = nullptr;
    idx_scanner = idx_scanner_end;
//    cout << "Added class: [" << c << "]\n";
  }
}

//-------------------------------------------
int main(int argc, char* argv[]) {
//  if (argc < 2) {
//    cerr << "Usage: <file_to_augment>\n";
//    return 1;
//  }
    
  string fileName = "";
  string prefix = "";
  bool useModifiedFilename = false;
  if (argc >=2) {
    fileName = string(argv[1]);
  } else {
    useModifiedFilename = true;
    fileName = "/Users/magnushyttsten/tmp/array_ops.py";
  }
  if (argc >= 3) {
    prefix = string(argv[2]);
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
    "IdAllocator",  // TODO: Support ': id1{}, id2{} {... (tensorflow/tensorflow/core/summary/summary_db_writer.cc)
    "RunMetadata",   // TODO: Same as IdAllocator
    "SeriesWriter",  // TODO: Same as IdAllocator
    "RunWriter",  // TODO: Same as IdAllocator
    "SummaryDbWriter",  // TODO: Same as IdAllocator
    "CachedInterpolationCalculator",  // TODO: Same as above tensorflow/tensorflow/core/kernels/image/resize_bicubic_op.cc
    "CSRSparseMatrix",  // TODO: As above, tensorflow/tensorflow/core/kernels/sparse/sparse_matrix.h
    "mutex::lock",
    "mutex::unlock",
    "mu_cast"
  };

  string skipFiles[] {
    "tensorflow/tensorflow/c/c_test.",
    "tensorflow/tensorflow/compiler/xla/python/tpu_driver/client/libtpu_client.",
    "tensorflow/tensorflow/lite/c/common.",
    "tensorflow/tensorflow/lite/c/c_test.",
    "tensorflow/tensorflow/lite/c/c_test.",
    "tensorflow/tensorflow/lite/experimental/microfrontend/",
    "tensorflow/tensorflow/lite/micro/examples/micro_speech/",
    "tensorflow/tensorflow/lite/micro/examples/person_detection/",
    "tensorflow/tensorflow/lite/micro/kernels/xtensa_hifimini_staging/xa_nnlib/algo/kernels/",
    "tensorflow/tensorflow/lite/micro/tools/make/targets/ecm3531/",
    "tensorflow/tensorflow/tools/lib_package/",
    "tensorflow/tensorflow/python/saved_model/constants.py",  // Has a # ... def ... which confuses py parser to think it's a function
    "tensorflow/tensorflow/python/saved_model/signature_constants.py",  // As above
    "tensorflow/tensorflow/magnus"
  };
  for (auto s: skipFiles) {
    if (fileName.find(s) == 0) {
      return -1;
    }
  }
  
  string warnings;
  cout << "MHTraceMaker, input file: " << fileName << "\n";
  string fin = CPPUtils::fsFileReadToString(fileName);
  if (fin.size() == 0) {
    return -1;
  }
  string fout;

//  CPPIndentWriter classDef2;
//  string className2 = generateClass(classDef2, fileName);
//  cout << classDef2.getString() << endl;
//  if (true) return 0;
  
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
    fout = compilePython(fin, fileName);
    cout << "[" << fout.substr(0, 200) << "]\n";
    if (useModifiedFilename) {
      fileName += ".mod" + fileExtension;
    }
    decodeCommentsAndStringLiterals(fout);
    CPPUtils::fsFileWrite(fileName, fout);
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
//  for (auto c: fin) {
//    if (c == charArchive) { c = '@'; }
//    cout << c;
//  }
//  decodeCommentsAndStringLiterals(fin);
//  cout << fin;
//  if (true) return -1;

  // Phase 0.1: Keep all class & struct ids
  // We use this to see if a function name (within a class/struct declaration) is a constructor
  unordered_map<string, void*> classNames;
  populateIdentifiersAfter(classNames, fin, "class ");
  populateIdentifiersAfter(classNames, fin, "struct ");
  
  CPPIndentWriter classDef;
  string className = generateClass(classDef, fileName);
  
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
  size_t added_characters = 0;  // Use this to calculate line number
  fout += fin.substr(0, idx_current);
  if (isCC) {
    s = "\n" + classDef.getString() + "\n";
    fout += s;
    added_characters += s.size();
  }
  else if (isH || isC) {
    s = "\n#include <stdio.h>\n";
    s += "#include <stdlib.h>\n";
    s += "#include <string.h>\n";
    s += "#include <sys/types.h>\n";
    s += "#include <unistd.h>\n";
    fout += s;
    added_characters += s.size();
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
//      cout << "\n\n*****************\n";
//      cout << "Processing candidbate\n";
//      cout << "..." << fin.substr(idx_lparen-10, 20) << "...\n";
      
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
      string functionName = "[lambda]";
      if (fin[idx_functionName_last] != ']') {
        isLambda = false;
        idx_functionName_first = backwardsToNonIdentifier(fin, idx_functionName_last);
        functionName = fin.substr(idx_functionName_first, idx_functionName_last-idx_functionName_first+1);
        if (functionName.size() == 0 || reserved.count(functionName) > 0) {
          valid = false;
        }
      }
      if (!valid) continue;
//      cout << "...functionName: [" << functionName << "]\n";
//      cout << "[" << fin.substr(idx_functionName_first-5, 50) << "]\n";
      if (skipFunctions.count(functionName) > 0) {
//        cout << "A skip function: " << functionName << endl;
        continue;
      }
//      if (functionName == "RunMetadata") {
//        cout << "Breakpoint\n";
//        cout << "Exists in skipFunctions: " << skipFunctions.count(functionName) << endl;
//        cout << fin.substr(idx_functionName_last-20, 40) << endl;
//      }

      // Detect if functionNamed is a constructor or destructor
      bool isConstructorOrDestructor = false;
      size_t coloncolon = functionName.find("::");
      if (coloncolon != string::npos) {
        string p1 = functionName.substr(0, coloncolon);
        string p2 = functionName.substr(coloncolon+2, functionName.size() - coloncolon -2);
        if (p1 == p2 || p1 == "~"+p2) {
          isConstructorOrDestructor = true;
        }
      }
      if (classNames.count(functionName) > 0 && functionName.find("~") == 0) {
        isConstructorOrDestructor = true;
      }
      
      // *** 3. Check if we have a constructor initializer with ':'
      //    CPUSubAllocator(CPUAllocator* cpu_allocator) : SubAllocator({}, {}), cpu_allocator_(cpu_allocator) {}
      size_t idx_lcb_scanner = idx_rparen_p1;
//      cout << "Before: [" << fin.substr(idx_lcb_scanner, 20) << "]\n";
      bool maybeConstructorInitializer = false;  // A ':' is only maybe, could also be: foo() ? a : b;
//      cout << "[";
      while(true) {
        char c = fin[idx_lcb_scanner];
//        cout << c;
        if (isWhitespace(c))
          idx_lcb_scanner++;
        else if (c == ':')  {
          maybeConstructorInitializer = true;
          break;
        }
        else
          break;
      }
//      cout << "]" << endl;
      // If we have constructor initializer, then scan ({}) pairs until we reach defintion {
      if (maybeConstructorInitializer) {
//        cout << "[";
        while (true) {
          char c = fin[idx_lcb_scanner];
//          cout << c;
          if (c == '{' || c == ';')  // Found termination criteria
            break;
          else if (c == '(') {  // Remove potential nested ({}) before reaching { definition
            int rnestCount = 0;
            while (true) {
              char c2 = fin[idx_lcb_scanner];
//              cout << "..." << c2;
              if (c2 == '(')      rnestCount++;
              else if (c2 == ')') rnestCount--;
              idx_lcb_scanner++;  // Go to next symbol, if last ')' then move to the one after it
              if (rnestCount == 0) {
//                cout << endl;
                break;
              }
            }
          }
          else {
            idx_lcb_scanner++;
          }
//          cout << "]" << endl;
        }
      }
      // Move forward if the lcb_scanning led to anything
      if (idx_rparen_p1 < idx_lcb_scanner) {
        s = fin.substr(idx_rparen_p1, idx_lcb_scanner-idx_rparen_p1);
        fout += s;
        idx_current = idx_lcb_scanner;
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
        // cout << "voidspace: " << voidspace << endl;
        // warnings += "   Warning ) <unknown> {: " + voidspace + " [" + to_string(lineNumber) + " " + functionName + " " + fileName + "]\n";
        continue;
      }
      // Looks good, consume up to {
      s = fin.substr(idx_current, idx_lcb-idx_current+1);
      fout += s;
      idx_current = idx_lcb+1;

      // Finally let's scan back to "};:"  (public:)
//      if (functionName == "min_log_x_output_bits") {
//        cout << "Opportunity\n";
//      }
      // Scan return type
      if (!isLambda && !isConstructorOrDestructor) {
        size_t idx_start = idx_functionName_first-1;
        set<char> terminator {';', '}', ':'};  // : as in public:
        set<char> allowed {'_', '&', '>', '<', ':', '*'};
        while (idx_start > 0
               &&
               (terminator.count(fin[idx_start]) == 0
                || isWhitespace(fin[idx_start])
                || allowed.count(fin[idx_start]) > 0
                || isalnum(fin[idx_start]))) {
          char c = fin[idx_start];
          idx_start--;
        }
        // Good if a terminator if found, but this could also be the first function definition in file (idx_start)
        if (terminator.count(fin[idx_start]) > 0 || idx_start == 0) {
          if (idx_functionName_first - idx_start < 3) {  // Need at least space: "public:T foo() { ... }
            continue;  // No return type, this is not a function definition
          }
          else {
            string s = fin.substr(idx_start+1, idx_functionName_first-1-(idx_start+1));
//            cout << "Return type: [" << s << "]\n";
            if (s.find("constexpr") != string::npos) {
              // Cannot trace constexpr functions
              //    tf/tf/lite/kernels/internal/common.h, tf/tf/lite/portable_type_to_tflitetype.h
              continue;  // Cannot trace constexpr functions (tensorflow/tensorflow/lite/kernels/internal/common.h)
            }
            else if (CPPUtils::strTrim(s) == "return") {
              continue;  // Not a function definition either
            }
          }
        }
      }  // End if !lambda && !constructor
        
      // All seems ok: Insert trace code
      string outstr;
      if (isCC){
        outstr += "\n   " + className + " mht_" + to_string(variableIndex) + "(, \"" + prefix + "\", \"" + fileName + "\", \"" + functionName + "\");\n";
        variableIndex++;
      }
      else if (isH || isC) {
        outstr = "\n" + genPrintf(functionName, prefix, fileName) + "\n";
      }
      else {
        assert(false);
      }
      added_characters += outstr.size();
      fout += outstr;
    }
  }
  
  // Entire file processed, result in fout
  // Overwrite original file
//  if (warnings.size() > 0) {
//    cerr << "\nFile: " << fileName << "\n";
//    cerr << warnings << "\n";
//  }

  decodeCommentsAndStringLiterals(fout);
  // Add lineNumber to generated code
  idx_current = 0;
  if (isCC) {
    while (true) {
      idx_current = fout.find(className + " mht_", idx_current);
      if (idx_current == string::npos) break;
      idx_current = fout.find("(,", idx_current);
      int lineNumber = getLineNumber(fout, idx_current)-1;
      fout.insert(idx_current+1, to_string(lineNumber));
    }
  } else if (isH || isC) {
    string fout2;
    while (true) {
      size_t idx_match = fout.find(MHTRACER_LINENUMBER_TAG, idx_current);
      if (idx_match == string::npos) {
        fout2 += fout.substr(idx_current, fout.size()-idx_current);
        break;
      } else {
        fout2 += fout.substr(idx_current, idx_match-idx_current);
      }
      int lineNumber = getLineNumber(fout, idx_match)-1;
      string lineNumberStr = to_string(lineNumber);
      fout2 += lineNumberStr;
      idx_current = idx_match + MHTRACER_LINENUMBER_TAG.size();
    }
    fout = fout2;
  }
  
  if (useModifiedFilename) {
    fileName += ".mod" + fileExtension;
  }
  CPPUtils::fsFileWrite(fileName, fout);
}

//-----------------------------------------
string genPrintf(const string& functionName, const string& prefix, const string& fileName1) {
  string fileName = washFileName(fileName1);
  CPPIndentWriter iw;
  iw.println("char mhtracer_s[200];");
  iw.println("char mhtracer_fn[40];");
  iw.println("int mhtracer_indent = 0;");
  iw.println("char* mhtracer_env_path = getenv(\"PATH\");");
  iw.println("char mhtracer_env_indent_name[20];");
  iw.println("sprintf(mhtracer_env_indent_name, \"%s%d\", \"" + ENV_MHTRACER_INDENT + "\", getpid());");
  iw.println("if (getenv(mhtracer_env_indent_name)) {");
  iw.println("   mhtracer_indent = atoi(mhtracer_env_indent_name) + 3;");
  iw.println("}");
  iw.println("char mhtracer_space[mhtracer_indent+1];");
  iw.println("mhtracer_space[0] = '\\0';");
  iw.println("int mhtracer_idx = 0;");
  iw.println("while (mhtracer_idx < mhtracer_indent) {");
  iw.println("   mhtracer_space[mhtracer_idx] = ' ';");
  iw.println("   mhtracer_idx++;");
  iw.println("   if (mhtracer_idx >= mhtracer_indent) mhtracer_space[mhtracer_idx] = '\\0';");
  iw.println("}");
  iw.println("if (!strstr(mhtracer_env_path, \"" + PATH_ENV_MHTRACER_DISABLE + "\")) {");
  // We cannot support indents here, because we cannot track exit to decrement indent (as with C++ destructor)
  iw.println("   sprintf(mhtracer_s, \"%s%s\", mhtracer_space, \"" + functionName + " " + prefix + " [h/c: " + MHTRACER_LINENUMBER_TAG + " @ " + fileName + "]\\n\");");
  iw.println("   if (mhtracer_env_path && strstr(mhtracer_env_path, \"" + PATH_ENV_MHTRACER_FILE + "\")) {");
  iw.println("      sprintf(mhtracer_fn, \"" + MHTRACER_FILENAME_PREFIX + "%d.log\", getpid());");
  iw.println("      FILE* mhtracer_fp = fopen(mhtracer_fn, \"a\");");
  iw.println("      fprintf(mhtracer_fp, \"%s\", mhtracer_s);");
  iw.println("      fclose(mhtracer_fp);");
  iw.println("   } else {");
  iw.println("      printf(\"%s\", mhtracer_s);");
  iw.println("   }");
  iw.println("}");
  return iw.getString();
}

//-----------------------------------------
string generateClass(CPPIndentWriter& iw, string fileName1) {
  string fileName = CPPUtils::strReplace(fileName1, "tensorflow", "tf");
  fileName = CPPUtils::strReplace(fileName, "framework", "fw");

  iw.println("#include <iostream>"); // std::cout
  iw.println("#include <fstream>");
  iw.println("#include <thread>");   // std::thread, std::thread::id, std::this_thread::get_id
  iw.println("#include <chrono>");   // std::chrono::seconds
  iw.println("#include <string>");
  iw.println("#include <cstdlib>");
  iw.println("#include <stdlib.h>");
  iw.println("#include <unistd.h>");
  
  fileName = CPPUtils::strReplace(fileName, "/", "PS");
  fileName = CPPUtils::strReplace(fileName, ".", "DT");
  fileName = CPPUtils::strReplace(fileName, "-", "SL");

  string className = "MHTracer_" + fileName;
  iw.println("class " + className + " {");
  iw.println("public:");
  iw.println("   std::string _s;");
  iw.println("   int _indent=0;");
  iw.println("   std::string _functionName;");
  iw.println("   bool _isFile = false;");
  iw.println("   std::string _fileName;");
  iw.println("   " + className + "(int lineNumber, std::string prefix, std::string fileName, std::string functionName);");
  iw.println("   ~" + className + "();");
  iw.println("};");
  
  iw.println(className + "::" + className + "(int lineNumber, std::string prefix, std::string fileName, std::string functionName) {");
  iw.println("   char* env_path = std::getenv(\"PATH\");");
  iw.println("   if (env_path != nullptr && std::string(env_path).find(\"" + PATH_ENV_MHTRACER_DISABLE + "\") != std::string::npos) {");
  iw.println("      return;");
  iw.println("   }");
  iw.println("   _functionName = functionName;");
  iw.println("   std::string env_pid = std::to_string(getpid());");
  iw.println("   char* env_indent = std::getenv(\"" + ENV_MHTRACER_INDENT + "env_pid\");");
  iw.println("   if (env_indent != nullptr) _indent = std::stoi(std::string(env_indent));");
  iw.println("   _s.assign(_indent, ' ');");
  iw.println("   std::string ostr = _s + functionName + \" \" + prefix + \" [\" + std::to_string(lineNumber) + \" @ \" + fileName + \"]\";");
  iw.println("   if (env_path != nullptr && std::string(env_path).find(\"" + PATH_ENV_MHTRACER_FILE + "\") != std::string::npos) {");
  iw.println("      _isFile = true;");
  iw.println("      _fileName = \"" + MHTRACER_FILENAME_PREFIX + "\" + env_pid + \".log\";");
  iw.println("      std::ofstream os;");
  iw.println("      os.open(_fileName, std::ofstream::out | std::ofstream::app);");
  iw.println("      os << ostr << \"\\n\";");
  iw.println("      os.close();");
  iw.println("   } else {");
  iw.println("      std::cout << ostr << \"\\n\";");
  iw.println("   }");
  iw.println("   _indent += 3;");
  iw.println("   setenv(\"" + ENV_MHTRACER_INDENT + "env_pid\", std::to_string(_indent).c_str(), 1);");
  iw.println("}");
  iw.println(className + "::~" + className + "() {");
  iw.println("   _indent -= 3;");
  iw.println("   std::string env_pid = std::to_string(getpid());");
  iw.println("   setenv(\"" + ENV_MHTRACER_INDENT + "env_pid\", std::to_string(_indent).c_str(), 1);");
  iw.println("}");
  return className;
}

//-----------------------------------------
void decodeCommentsAndStringLiterals(string& fin) {
  int idx_archive = 0;
  
  int idx = 0;
  while (true) {
    while (idx < fin.size() && fin[idx] != charArchive) { idx++; }
    if (idx == fin.size()) { return; }
    int start = idx;

    while (idx < fin.size() && fin[idx] == charArchive) { idx++; }
//    size_t clen = idx-start;
//    int len = archiveCommentsAndStringLiterals[idx_archive].size();
//    cout << "clen: " << clen << ", len: " << len << endl;
//    if (len != clen) cout << archiveCommentsAndStringLiterals[idx_archive] << endl;
//    assert(idx - start == archiveCommentsAndStringLiterals[idx_archive].size());
    int length = idx-start;
    while (length > 0) {
      int clength = archiveCommentsAndStringLiterals[idx_archive].second.size();
//      cout << "Archive: " << archiveCommentsAndStringLiterals[idx_archive].second << "\n";
//      if (archiveCommentsAndStringLiterals[idx_archive].second == "# this is the end") {
//        cout << "Hello\n";
//      }
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
