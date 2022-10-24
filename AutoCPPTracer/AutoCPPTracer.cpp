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
#include <iomanip>
#include <set>
#include <vector>
#include <map>
#include <unordered_map>
#include <sstream>
#include <cstdlib>
#include "Utils.h"
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
// Set to non-zero to use a file for logging
static string ENV_MHTRACER_FILE = "MHTRACER_USEFILE";
// Should probably be disabled during product build
// Otherwise, generated binaries used during build will emit tracing information
static string ENV_MHTRACER_ENABLE = "MHTRACER_ENABLE";
// Count of filtered function calls (increased if call encounters !MHTRACER_ENABLE
static string ENV_MHTRACER_FILTERED_CALLS_COUNT = "MHTRACER_FILTERED_CALLS_COUNT";
// Set to avoid pringing PID-TID as output, useful to do diff between different runs
static string ENV_MHTRACER_DONT_PRINT_PID_DASH_TID = "MHTRACER_DONT_PRINT_PID_DASH_TID";
// fileName1|functionName1|lineNumber1|true/false;fileName2|functionName2|lineNumber2|true/false
// true == print summary string
static string ENV_MHTRACER_FILTER_RUNTIME = "MHTRACER_FILTER_RUNTIME";
// fileName1|functionName1;fileName1|functionName1
static string ENV_MHTRACER_FILTER_COMPILETIME = "MHTRACER_FILTER_COMPILETIME";

static string MHTRACER_FILENAME_PREFIX = "/tmp/mhtracer_";

// ****************************************************************
vector<pair<size_t,string>> archiveCommentsAndStringLiterals;
//const char charArchive = '@';


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

// Read compile-time filter environment variable
vector<pair<string,string>> getFilterCompileTime() {
   vector<pair<string,string>> r;
   return r;
}
bool getShouldFilterCompileTimeEntry(const string& fileName,
                                     const string& functionName) {
   // Get the environment variable content
   vector<pair<string,string>> filter = getFilterCompileTime();
   const char* envCStr = getenv(ENV_MHTRACER_FILTER_COMPILETIME.c_str());
   if (envCStr == nullptr) {
      return false;
   }
   string envStr(envCStr);
   
   vector<string> es = CPPUtils::strSplit(envStr, ";", /*trim=*/true);
   for (auto e : es) {
      vector<string> es = CPPUtils::strSplit(e, "|", /*trim=*/true);
      filter.push_back(pair<string,string>(es.at(0), es.at(1)));
   }
   
   for (auto e: filter) {
      // fname|* match?
      if (e.second == "*" && fileName.find(e.first) == 0) {
         // cout << "Skipping fileName: " << fileName << endl;
         return true;
      }
      
      // *|functionName match?
      if (e.first == "*" && e.second == functionName) {
         // cout << "Skipping functionName: " << functionName << endl;
         return true;
      }
      
      // fileName|functionName match?
      if (e.first != "*" && e.second != "*") {
         if (fileName.find(e.first) != string::npos &&
             functionName == e.second) {
            // cout << "Skipping fileName|functionName: " << fileName << "|" << functionName << endl;
            return true;
         }
      }
   }
   return false;
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

//-------------------------------------------
const string MODE_TRACE = "TRACE";    // Insert trace code
bool   codeGenOutputEmitted = false;
size_t codeGenInsertOffset = string::npos;
string fileName;

int main(int argc, char* argv[]) {
  
//   std::stringstream sss;
//   sss << std::setfill('0') << std::setw(4) << 10 << endl;
//   cout << sss.str();
//   if (true) return -1;
   
//   char* env_dont_print_pid_dash_tid = std::getenv("MHTRACER_DONT_PRINT_PID_DASH_TID");
//   if (env_dont_print_pid_dash_tid != nullptr) {
//      cout << ENV_MHTRACER_DONT_PRINT_PID_DASH_TID << " IS SET" << endl;
//   } else {
//      cout << ENV_MHTRACER_DONT_PRINT_PID_DASH_TID << " WAS NULLPTR" << endl;
//   }
   
   //cout << "AutoCPPTracer: main entered" << endl;
      
   string prefix = "";
   //  string mode(MODE_TRACE);
   string mode;
   bool useModifiedFilename = false;
   
   string usage = "Usage: [TRACE | --help] <filename> [prefix]";
   if (argc < 2) {
      cout << usage << endl;
      return 1;
   }
   mode = string(argv[1]);
   if (mode == "--help") {
      cout << "MHTraceMain v2.1.0 221022_123800" << endl;
      return 0;
   }
   if (mode != MODE_TRACE || argc < 3) {
      cout << usage << endl;
      cout << "Error: Did not see \"TRACE\" or no fileName was given" << endl;
      return 1;
   }
   fileName = string(argv[2]);
   
   if (argc >= 4) {
      prefix = string(argv[3]);
   }
   
   string warnings;

   // Should entire be filtered out?
   if (getShouldFilterCompileTimeEntry(fileName, "*")) {
      // cout << "Filtering out file: " << fileName << endl;
      return -1;
   }
   
   //cout << "...Processing fileName: " << fileName << endl;
   string fin = CPPUtils::fsFileReadToString(fileName);
   if (fin.size() == 0) {
      return -1;
   }
         
   set<string> CC_EXT = {".cc", ".cpp"};
   bool isCC = false;
   set<string> H_EXT = {".h", ".hpp", ".hh"};
   bool isH = false;
   
   size_t idx_dot = fileName.rfind(".");
   
   if (idx_dot == string::npos) {
      cout << "*********** ERROR for file: " << fileName << endl;
      assert(idx_dot != string::npos);
   }
   
   string fileExtension = fileName.substr(idx_dot, fileName.size());
   if (CC_EXT.count(fileExtension) > 0) {
      isCC = true;
   }
   else if (H_EXT.count(fileExtension) > 0) {
      isH = true;
   }
   else {
      cout << "**** Error unknown extension" << endl;
      assert(false);
   }
   
   //  cout << "Processing: " << fileName << endl;
   
   // Phase 0.0: Get rid of string literals and comments
   // While parsing, archive comment and string literals,
   // since they can contain things that confuse parsing
   size_t charArchivePos = fin.find(charArchive);
   if (charArchivePos != string::npos) {
      cout << "Found charArchive character in fileName: " << fileName << endl;
      assert(charArchivePos == string::npos);
   }
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
   string fout;
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
         if (getShouldFilterCompileTimeEntry(fileName, functionName)) {
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
                  bool assertCond = returnType.size() == 0
                  || returnType.find("EIGEN_") != string::npos
                  || returnType.find("ABSL_") != string::npos
                  || returnType.find("TF_LITE_") != string::npos
                  || returnType.find("__") != string::npos
                  || returnType.find("PHILOX_") != string::npos
                  || returnType.find("TF_ATTRIBUTE_NORETURN") != string::npos;
                  if (!assertCond) {
                     cout << "*** Assert error for: " << fileName << endl;
                  }
                  assert(assertCond);
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
               bool assertCond = returnType.size() == 0
               || returnType.find("EIGEN_") != string::npos
               || returnType.find("ABSL_") != string::npos
               || returnType.find("TF_LITE_") != string::npos
               || returnType.find("__") != string::npos
               || returnType.find("PHILOX_") != string::npos
               || returnType.find("TF_ATTRIBUTE_NORETURN") != string::npos;
               if (!assertCond) {
                  cout << "*** Error, assertcond not met: " << fileName << endl;
               }
               assert(assertCond);
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
         else {
            cout << "*** Error no support for file: " << fileName << endl;
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
   //cout << "...wrote file: " << fileName << endl;
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
   iw.println("   std::string _indentStr;");
   iw.println("   std::string _traceStr;");
   iw.println("   int _indent = 0;");
   iw.println("   std::string _functionName;");
   iw.println("   std::string _fileName;");
   iw.println("   std::string _envMHIndent;");
   iw.println("   int _lineNumber;");
   iw.println("   bool _filtered = false;");
   iw.println("   bool _filteredPrintSummary = false;");
   iw.println("   bool _otherThread = false;");
   // Constructor
   iw.println("   " + className + "(std::vector<std::string> params, int lineNumber, std::string prefix, std::string fileName, std::string functionName) {");
   iw.println("      _fileName = fileName;");
   iw.println("      _functionName = functionName;");
   iw.println("      _lineNumber = lineNumber;");
   iw.println("");
   iw.println("      // Check if tracing is enabled");
   iw.println("      const char* env_enable = std::getenv(\"" + ENV_MHTRACER_ENABLE + "\");");
   iw.println("      if (env_enable == nullptr || std::stoi(std::string(env_enable)) == 0) {");
   iw.println("         int filtered_count = 1;");
   iw.println("         char* filtered_count_str = std::getenv(\"" + ENV_MHTRACER_FILTERED_CALLS_COUNT + "\");");
   iw.println("         if (filtered_count_str != nullptr) {");
   iw.println("            filtered_count = std::stoi(std::string(filtered_count_str));");
   iw.println("            filtered_count++;");
   iw.println("         }");
   iw.println("         setenv(\"" + ENV_MHTRACER_FILTERED_CALLS_COUNT + "\", std::to_string(filtered_count).c_str(), 1);");
   iw.println("         return;");
   iw.println("      }");

   iw.println("// *** Create log string");
   iw.println("      // Assign indent spaces (tied to PID and TID)");
   iw.println("      pid_t pid = getpid();");
   iw.println("      std::thread::id tid = std::this_thread::get_id();");
   iw.println("      std::stringstream pid_dash_tid_ss;");
   iw.println("      pid_dash_tid_ss << pid << \"-\" << tid;");
   iw.println("      std::string pid_dash_tid_str = pid_dash_tid_ss.str();");
   iw.println("      _envMHIndent = \"" + ENV_MHTRACER_INDENT + "\";");
   iw.println("      char* env_indent = std::getenv(_envMHIndent.c_str());");
   iw.println("      if (env_indent != nullptr) {");
   iw.println("         _indent = std::stoi(std::string(env_indent));");
   iw.println("      }");
   iw.println("      // Check that reporting matches pid/tid");
   iw.println("      const char* env_pid_dash_tid = std::getenv(\"" + ENV_PID_DASH_TID + "\");");
   iw.println("      if (env_pid_dash_tid != nullptr) {");
   iw.println("         std::string env_pid_dash_tid_str(env_pid_dash_tid);");
   iw.println("         if (env_pid_dash_tid_str != pid_dash_tid_str) {");
   iw.println("            _otherThread = true;");
   iw.println("         }");
   iw.println("      }");
   iw.println("      else {  // PID-THREAD not set, set it for the first time (starter thread)");
   iw.println("         setenv(\"" + ENV_PID_DASH_TID + "\", pid_dash_tid_str.c_str(), 1);");
   iw.println("      }");
   iw.println("      // Process parameters");
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
   iw.println("      // Process if PID-TID should be printed");
   iw.println("      const char* env_dont_print_pid_dash_tid = std::getenv(\"" + ENV_MHTRACER_DONT_PRINT_PID_DASH_TID + "\");");
   iw.println("      if (env_dont_print_pid_dash_tid != nullptr) {");
   iw.println("         pid_dash_tid_str = \"\";");
   iw.println("      }");
   iw.println("      if (_otherThread) {");
   iw.println("         functionName = \"MHOT_\" + functionName;");
   iw.println("      }");
   iw.println("      // Create printout");
   iw.println("      _indentStr.assign(_indent, ' ');");
   iw.println("      _traceStr += functionName");
   iw.println("         + \" [1]\"");
   iw.println("         + \" [\" + prefix + \"]\"");
   iw.println("         + \" [\" + paramStr + \"]\"");
   iw.println("         + \" [\" + pid_dash_tid_str + \" \"");
   iw.println("         +    std::to_string(lineNumber)");
   iw.println("         +    \" @ \" + fileName + \"]\\n\";");
   iw.println("");
   iw.println("      // Should we trace or filter?");
   iw.println("      const char* env_filter = std::getenv(\"" + ENV_MHTRACER_FILTER_RUNTIME + "\");");
   iw.println("      if (env_filter != nullptr) {");
   iw.println("         std::string sfilter = std::string(env_filter);");
   iw.println("         std::string sLineNumber = std::to_string(lineNumber);");
   iw.println("         while (true) {");
   iw.println("            std::size_t ioE = sfilter.find(\";\");");
   iw.println("            if (sfilter.size() == 0) {");
   iw.println("               break;");
   iw.println("            }");
   iw.println("            std::string cfs = sfilter.substr(0, ioE);");
   iw.println("            std::size_t ioFileName = cfs.find(\"|\");");
   iw.println("            std::string fFileName  = cfs.substr(0, ioFileName);");
   iw.println("            std::size_t ioFunctionName = cfs.find(\"|\", ioFileName+1);");
   iw.println("            std::string fFunctionName  = cfs.substr(ioFileName+1, ioFunctionName-ioFileName-1);");
   iw.println("            std::size_t ioLineNumber = cfs.find(\"|\", ioFunctionName+1);");
   iw.println("            std::string fLineNumber    = cfs.substr(ioFunctionName+1, ioLineNumber-ioFunctionName-1);");
   iw.println("            std::string fPrintSummaryStr  = cfs.substr(ioLineNumber+1, cfs.size()-ioLineNumber-1);");
   iw.println("            if (  (fFileName == \"*\" || fFileName == fileName)");
   iw.println("               && (fFunctionName == \"*\" || fFunctionName == functionName)");
   iw.println("               && (fLineNumber == \"*\" || fLineNumber == sLineNumber)) {");
   iw.println("               _filtered = true;");
   iw.println("               _filteredPrintSummary = fPrintSummaryStr == \"true\";");
   iw.println("               setenv(\"" + ENV_MHTRACER_FILTERED_CALLS_COUNT + "\", \"1\", 1);");
   iw.println("               setenv(\"" + ENV_MHTRACER_ENABLE + "\", \"0\", 1);");
   iw.println("               return;");
   iw.println("            }");
   iw.println("            if (ioE == std::string::npos) {");
   iw.println("               sfilter = \"\";");
   iw.println("            } else {");
   iw.println("               sfilter = sfilter.substr(ioE+1, sfilter.size()-ioE-1);");
   iw.println("            }");
   iw.println("         }");
   iw.println("      }");

   iw.println("      // Check if tracing is enabled");
   iw.println("      const char* env_file = std::getenv(\"" + ENV_MHTRACER_FILE + "\");");
   iw.println("      if (env_file != nullptr && std::stoi(std::string(env_file)) != 0) {");
   iw.println("         std::string logFileName = \"" + MHTRACER_FILENAME_PREFIX + "\" + pid_dash_tid_str + \".log\";");
   iw.println("         std::ofstream os;");
   iw.println("         os.open(logFileName, std::ofstream::out | std::ofstream::app);");
   iw.println("         os << _indentStr << _traceStr << \"\";");
   iw.println("         os.close();");
   iw.println("      }");
   iw.println("      // Log to stdout");
   iw.println("      else {");
   iw.println("         std::cout << _indentStr << _traceStr << \"\";");
   iw.println("      }");
   iw.println("");
   iw.println("      // Increment indent spaces");
   iw.println("      if (_otherThread) {");
   iw.println("         return;");
   iw.println("      }");
   iw.println("      _indent += 3;");
   iw.println("      setenv(_envMHIndent.c_str(), std::to_string(_indent).c_str(), 1);");
   iw.println("   }");
   // Destructor
   iw.println("   ~" + className + "() {");
   iw.println("");
   iw.println("      // Don't update indent if from another thread");
   iw.println("      if (_otherThread) {");
   iw.println("         return;");
   iw.println("      }");
   iw.println("      if (_filteredPrintSummary) {");
   iw.println("         int filtered_count = 1;");
   iw.println("         char* filtered_count_str = std::getenv(\"" + ENV_MHTRACER_FILTERED_CALLS_COUNT + "\");");
   iw.println("         if (filtered_count_str != nullptr) {");
   iw.println("            filtered_count = std::stoi(std::string(filtered_count_str));");
   iw.println("         }");
   iw.println("         std::cout << _indentStr << \"FILTERED_COUNT_\" << filtered_count << \"_\" << _traceStr;");
   iw.println("         setenv(\"" + ENV_MHTRACER_FILTERED_CALLS_COUNT + "\", \"0\", 1);");
   iw.println("      }");
   iw.println("      if (_filtered) {");
   iw.println("         setenv(\"" + ENV_MHTRACER_ENABLE + "\", \"1\", 1);");
   iw.println("      }");
   iw.println("      else {");
   iw.println("         const char* env_enable = std::getenv(\"" + ENV_MHTRACER_ENABLE + "\");");
   iw.println("         if (env_enable != nullptr && std::stoi(std::string(env_enable)) == 1) {");
   iw.println("            _indent -= 3;");
   iw.println("            setenv(_envMHIndent.c_str(), std::to_string(_indent).c_str(), 1);");
   iw.println("         }");
   iw.println("      }");
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
   string s1 = fin.substr(start, end-start);
   archiveCommentsAndStringLiterals.push_back({start, s1});
   string s2;
   s2.assign(end-start, charArchive);
   fin.replace(start, end-start, s2);
   if (s1.size() != s2.size()) {
      cout << "*** Error, size differed: " << fileName << endl;
   }
   assert(s1.size() == s2.size());
   size_t next = end;
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
      if (idx_charAfter == string::npos) {
         cout << "*** Error: index problems: " << fileName << endl;
      }
      assert(idx_charAfter != string::npos);
      //    cout << "CAPUTURED[" << in.substr(posOfStartQuote, idx_charAfter-posOfStartQuote) << "]\n";
      //    cout << "NEXT[" << in.substr(idx_charAfter, 5) << "]\n";
      return  idx_charAfter;
   }
   
   size_t nextQuote = posOfStartQuote+1;
   while (true) {
      nextQuote = in.find("\"", nextQuote);
      if( nextQuote == string::npos) {
         cout << "*** Error, did not find next quote: " << fileName << endl;
      }
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
