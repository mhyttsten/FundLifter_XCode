#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <string>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <unistd.h>
class MHTracer_AutoCPPTracerTestDTcc {
public:
   std::string _indentStr;
   std::string _traceStr;
   int _indent = 0;
   std::string _functionName;
   std::string _fileName;
   std::string _envMHIndent;
   int _lineNumber;
   bool _filtered = false;
   bool _filteredPrintSummary = false;
   bool _otherThread = false;
   MHTracer_AutoCPPTracerTestDTcc(std::vector<std::string> params, int lineNumber, std::string prefix, std::string fileName, std::string functionName) {
      _fileName = fileName;
      _functionName = functionName;
      _lineNumber = lineNumber;

      // Check if tracing is enabled
      const char* env_enable = std::getenv("MHTRACER_ENABLE");
      if (env_enable == nullptr || std::stoi(std::string(env_enable)) == 0) {
         int filtered_count = 1;
         char* filtered_count_str = std::getenv("MHTRACER_FILTERED_CALLS_COUNT");
         if (filtered_count_str != nullptr) {
            filtered_count = std::stoi(std::string(filtered_count_str));
            filtered_count++;
         }
         setenv("MHTRACER_FILTERED_CALLS_COUNT", std::to_string(filtered_count).c_str(), 1);
         return;
      }
// *** Create log string
      // Assign indent spaces (tied to PID and TID)
      pid_t pid = getpid();
      std::thread::id tid = std::this_thread::get_id();
      std::stringstream pid_dash_tid_ss;
      pid_dash_tid_ss << pid << "-" << tid;
      std::string pid_dash_tid_str = pid_dash_tid_ss.str();
      _envMHIndent = "MHTRACER_INDENT_";
      char* env_indent = std::getenv(_envMHIndent.c_str());
      if (env_indent != nullptr) {
         _indent = std::stoi(std::string(env_indent));
      }
      // Check that reporting matches pid/tid
      const char* env_pid_dash_tid = std::getenv("MHTRACER_PID_DASH_TID");
      if (env_pid_dash_tid != nullptr) {
         std::string env_pid_dash_tid_str(env_pid_dash_tid);
         if (env_pid_dash_tid_str != pid_dash_tid_str) {
            _otherThread = true;
         }
      }
      else {  // PID-THREAD not set, set it for the first time (starter thread)
         setenv("MHTRACER_PID_DASH_TID", pid_dash_tid_str.c_str(), 1);
      }
      // Process parameters
      std::string paramStr;
      for (int i=0; i < params.size(); i++) {
         auto e = params[i];
         while (e.find("\n") != std::string::npos) {
            size_t pos = e.find("\n");
            e = e.erase(pos, 1);
            e = e.insert(pos, "<NL>");
         }
         while (e.find("[") != std::string::npos) {
            size_t pos = e.find("[");
            e = e.erase(pos, 1);
            e = e.insert(pos, "<LB>");
         }
         while (e.find("]") != std::string::npos) {
            size_t pos = e.find("]");
            e = e.erase(pos, 1);
            e = e.insert(pos, "<RB>");
         }
         paramStr += e;
         if ((i+1) < params.size()) {
            paramStr += ", ";
         }
      }
      // Process if PID-TID should be printed
      const char* env_dont_print_pid_dash_tid = std::getenv("MHTRACER_DONT_PRINT_PID_DASH_TID");
      if (env_dont_print_pid_dash_tid != nullptr) {
         pid_dash_tid_str = "";
      }
      if (_otherThread) {
         functionName = "MHOT_" + functionName;
      }
      // Create printout
      _indentStr.assign(_indent, ' ');
      _traceStr += functionName
         + " [1]"
         + " [" + prefix + "]"
         + " [" + paramStr + "]"
         + " [" + pid_dash_tid_str + " "
         +    std::to_string(lineNumber)
         +    " @ " + fileName + "]\n";

      // Should we trace or filter?
      const char* env_filter = std::getenv("MHTRACER_FILTER_RUNTIME");
      if (env_filter != nullptr) {
         std::string sfilter = std::string(env_filter);
         std::string sLineNumber = std::to_string(lineNumber);
         while (true) {
            std::size_t ioE = sfilter.find(";");
            if (sfilter.size() == 0) {
               break;
            }
            std::string cfs = sfilter.substr(0, ioE);
            std::size_t ioFileName = cfs.find("|");
            std::string fFileName  = cfs.substr(0, ioFileName);
            std::size_t ioFunctionName = cfs.find("|", ioFileName+1);
            std::string fFunctionName  = cfs.substr(ioFileName+1, ioFunctionName-ioFileName-1);
            std::size_t ioLineNumber = cfs.find("|", ioFunctionName+1);
            std::string fLineNumber    = cfs.substr(ioFunctionName+1, ioLineNumber-ioFunctionName-1);
            std::string fPrintSummaryStr  = cfs.substr(ioLineNumber+1, cfs.size()-ioLineNumber-1);
            if (  (fFileName == "*" || fFileName == fileName)
               && (fFunctionName == "*" || fFunctionName == functionName)
               && (fLineNumber == "*" || fLineNumber == sLineNumber)) {
               _filtered = true;
               _filteredPrintSummary = fPrintSummaryStr == "true";
               setenv("MHTRACER_FILTERED_CALLS_COUNT", "1", 1);
               setenv("MHTRACER_ENABLE", "0", 1);
               return;
            }
            if (ioE == std::string::npos) {
               sfilter = "";
            } else {
               sfilter = sfilter.substr(ioE+1, sfilter.size()-ioE-1);
            }
         }
      }
      // Check if tracing is enabled
      const char* env_file = std::getenv("MHTRACER_USEFILE");
      if (env_file != nullptr && std::stoi(std::string(env_file)) != 0) {
         std::string logFileName = "/tmp/mhtracer_" + pid_dash_tid_str + ".log";
         std::ofstream os;
         os.open(logFileName, std::ofstream::out | std::ofstream::app);
         os << _indentStr << _traceStr << "";
         os.close();
      }
      // Log to stdout
      else {
         std::cout << _indentStr << _traceStr << "";
      }

      // Increment indent spaces
      if (_otherThread) {
         return;
      }
      _indent += 3;
      setenv(_envMHIndent.c_str(), std::to_string(_indent).c_str(), 1);
   }
   ~MHTracer_AutoCPPTracerTestDTcc() {

      // Don't update indent if from another thread
      if (_otherThread) {
         return;
      }
      if (_filteredPrintSummary) {
         int filtered_count = 1;
         char* filtered_count_str = std::getenv("MHTRACER_FILTERED_CALLS_COUNT");
         if (filtered_count_str != nullptr) {
            filtered_count = std::stoi(std::string(filtered_count_str));
         }
         std::cout << _indentStr << "FILTERED_COUNT_" << filtered_count << "_" << _traceStr;
         setenv("MHTRACER_FILTERED_CALLS_COUNT", "0", 1);
      }
      if (_filtered) {
         setenv("MHTRACER_ENABLE", "1", 1);
      }
      else {
         const char* env_enable = std::getenv("MHTRACER_ENABLE");
         if (env_enable != nullptr && std::stoi(std::string(env_enable)) == 1) {
            _indent -= 3;
            setenv(_envMHIndent.c_str(), std::to_string(_indent).c_str(), 1);
         }
      }
   }
};



#include <iostream>

using namespace std;

class Foo {
public:
   void foo1();
   void foo2();
   void foo3();
   void foo4();
};

int main(int argc, char* argv[]) {
   std::vector<std::string> mht_0_v;
   MHTracer_AutoCPPTracerTestDTcc mht_0(mht_0_v, 204, "", "AutoCPPTracerTest.cc", "main");

   Foo foo;

   cout << "==-----" << endl << "Round 1" << endl;
   foo.foo1();
   cout << "==-----" << endl << "Round 2" << endl;
   foo.foo1();
   cout << "==-----" << endl << "Round 3" << endl;
   foo.foo1();
   cout << "==-----" << endl << "Round 4" << endl;
   foo.foo1();
}

void Foo::foo1() {
   std::vector<std::string> mht_1_v;
   MHTracer_AutoCPPTracerTestDTcc mht_1(mht_1_v, 220, "", "AutoCPPTracerTest.cc", "Foo::foo1");


   cout << "Foo:foo1() ENTRY" << endl;
   foo2();
   cout << "Foo:foo1() EXIT" << endl;
}

void Foo::foo2() {
   std::vector<std::string> mht_2_v;
   MHTracer_AutoCPPTracerTestDTcc mht_2(mht_2_v, 230, "", "AutoCPPTracerTest.cc", "Foo::foo2");


   cout << "Foo:foo2() ENTRY" << endl;
   foo3();
   cout << "Foo:foo2() EXIT" << endl;
}

void Foo::foo3() {
   std::vector<std::string> mht_3_v;
   MHTracer_AutoCPPTracerTestDTcc mht_3(mht_3_v, 240, "", "AutoCPPTracerTest.cc", "Foo::foo3");


   cout << "Foo:foo3() ENTRY" << endl;
   foo4();
   cout << "Foo:foo3() EXIT" << endl;
}

void Foo::foo4() {
   std::vector<std::string> mht_4_v;
   MHTracer_AutoCPPTracerTestDTcc mht_4(mht_4_v, 250, "", "AutoCPPTracerTest.cc", "Foo::foo4");


   cout << "Foo:foo4() ENTRY" << endl;
   cout << "Foo:foo4() EXIT" << endl;
}


