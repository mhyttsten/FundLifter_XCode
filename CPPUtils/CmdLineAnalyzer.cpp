//
//  CmdLineAnalyzer.cpp
//  FundLifter
//
//  Created by Magnus Hyttsten on 11/21/20.
//  Copyright © 2020 Magnus Hyttsten. All rights reserved.
//

#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include "CPPUtils.h"

using namespace std;

struct LineResult {
  vector<string> match;
  vector<string> orderWrong;
  vector<string> nomatch;  // includes orderWrong entries
};

LineResult linesContaining(const vector<string>& dataset, const vector<string>& containing);
void printVector(int max, const string& hdr, const vector<string>& v0);

int main(int argc, char* argv[]) {
    
//  cout << "Command-line arguments: " << argc << '\n';
//  for (int i=0; i < argc; i++) {
//    cout << "   [" << i << "]: " << argv[i] << '\n';
//  }
//  cout << '\n';
//  if (argc <= 1) {
//    cout << "Expected file to process as argument\n";
//    return 1;
//  }

//  ifstream infile(fname);
//  if (!infile) {
//    cout << "Could not open file: " << argv[1] << '\n';
//    return 1;
//  }
//
//  string line;
//  vector<string> flines;
//  while (std::getline(infile, line)) {
//    flines.push_back(line);
//  }
//

  
//  string s = "external/local_config_cc/cc_wrapper.sh -U_FORTIFY_SOURCE -fstack-protector -Wall -Wthread-safety -Wself-assign -fcolor-diagnostics -fno-omit-frame-pointer -g '-std=c++0x' -MD -g0 -MF bazel-out/darwin-dbg/bin/external/com_google_absl/absl/base/_objs/base/sysinfo.pic.d '-frandom-seed=bazel-out/darwin-dbg/bin/external/com_google_absl/absl/base/_objs/base/sysinfo.pic.o' -fPIC -D__CLANG_SUPPORT_DYN_ANNOTATION__ -iquote external/com_google_absl -iquote bazel-out/darwin-dbg/bin/external/com_google_absl -w '-std=c++14' -Wall -Wextra -Wcast-qual -Wconversion-null -Wmissing-declarations -Woverlength-strings -Wpointer-arith -Wunused-local-typedefs -Wunused-result -Wvarargs -Wvla -Wwrite-strings -Wno-missing-field-initializers -Wno-sign-compare -no-canonical-prefixes -Wno-builtin-macro-redefined '-D__DATE__=\"redacted\"' '-D__TIMESTAMP__=\"redacted\"' '-D__TIME__=\"redacted\"' -c external/com_google_absl/absl/base/internal/sysinfo.cc -o bazel-out/darwin-dbg/bin/external/com_google_absl/absl/base/_objs/base/sysinfo.pic.o)";
//  flines.clear();
//  flines.push_back(s);
//  LineResult lr1 = linesContaining(flines, {"SUBCOMMAND"});
//  cout << "SC: " << lr1.match.size() << '\n';
//  LineResult lr2 = linesContaining(lr1.match, {".cc"});
//  cout << "SC && .cc:  " << lr2.match.size() << '\n';
//  lr2 = linesContaining(lr2.nomatch, {".cpp"});
//  cout << "SC && .cpp: " << lr2.match.size() << '\n';
//  printVector(10, "SC && !cc && !cpp", lr2.nomatch);
//  LineResult lr10 = linesContaining(flines, {"external/local_config_cc/cc_wrapper.sh", "-g0", "-g"});
//  cout << "Matches: " << lr10.match.size() << '\n';
}

//---------------------------------------------
LineResult linesContaining(const vector<string>& dataset, const vector<string>& containing) {
  LineResult lr;
  for (int i=0; i < dataset.size(); i++) {
    string l = dataset[i];
    bool error = false;
    size_t plastmatch = 0;
    for (string s: containing) {
      size_t pnow = l.rfind(s);
      if (pnow == string::npos) {
        error = true;
        lr.nomatch.push_back(l);
        break;
      }
      else if (pnow < plastmatch) {
        error = true;
        lr.nomatch.push_back(l);
        lr.orderWrong.push_back(l);
      }
      else {
        plastmatch = pnow + s.length();
      }
    }
    if (!error) {
      lr.match.push_back(l);
    }
  }
  return lr;
}

//---------------------------------------------
void printVector(int max, const string& hdr, const vector<string>& v0) {
  if (max == -1) {
    max = (int)v0.size();
  }
  cout << hdr << ": " << v0.size() << '\n';
  for (int i=0; i < v0.size(); i++) {
    if (i == max) {
      break;
    }
    cout << "   [" << i << "]: " + v0[i] << '\n';
  }
}


