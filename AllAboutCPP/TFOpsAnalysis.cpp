//
//  TFOpsAnalysis.cpp
//  FundLifter
//
//  Created by Magnus Hyttsten on 12/28/20.
//  Copyright © 2020 Magnus Hyttsten. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include "CPPUtils.h"

vector<string> find(vector<string> v, string strToFind) {
  vector<string> r;
  for (string s: v) {
    if (s.find(strToFind) != string::npos) {
      r.push_back(s);
    }
  }
  return r;
}

string opCategory2PyWrapTxtFile(string s) { return "gen_" + s + "_py_wrappers_cc.register_op.txt"; }
string opCategory2CCTxtFile(string s)     { return s + "_gen_cc.register_op.txt"; }
string opCategory2PyWrapBin(string s) { return "gen_" + s + "_py_wrappers_cc"; }
string opCategory2CCBin(string s)     { return s + "_gen_cc"; }

bool areAllRegisterOpFilesEquivalent(vector<string> files, set<string>& opNames) {
  for (int i=0; i < files.size(); i++) {
    // cout << "Doing file: " << vAll[i] << endl;
    vector<string> flines = CPPUtils::fsFileReadToLines(files[i]);
    for (int j=0; j < flines.size(); j++) {
      vector<string> v = CPPUtils::strSplit(flines[j], " ");
      string opName = v[v.size()-1];
      if (i == 0) {
        if (opNames.count(opName) != 0) return false;
        opNames.insert(opName);
      }
      else if (opNames.count(opName) != 1) {
        return false;
      }
    }
  }
  return true;
}

bool doesArg1ContainArg2(const set<string>& arg1, const set<string>& arg2) {
  unordered_map<string,int> um1;
  for (auto i=arg1.begin(); i != arg1.end(); i++) {
    if (arg2.find(*i) == arg2.end()) return false;
  }
  return true;
}

void printOpCategoryAndOps(bool pyWrap, bool cc, string header, vector<pair<string, vector<string>>> l) {
  cout << endl << header << "," << l.size() << " categories" << endl;
  for (auto e: l) {

    string bin = "";
    if (pyWrap && cc) bin = opCategory2PyWrapBin(e.first) + ". " + opCategory2CCBin(e.first);
    else if (pyWrap)  bin = opCategory2PyWrapBin(e.first) + ". N/A";
    else if (cc)      bin = "N/A. " + opCategory2CCBin(e.first);
    else assert(0);

    cout << "   ," << e.first << " (" << e.second.size() << " ops. " << bin << ")" << endl;
    sort(e.second.begin(), e.second.end());
    for (auto e1: e.second) {
      cout << "      ,," << e1 << endl;
    }
  }
}


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

  vector<string> flines = CPPUtils::fsFileReadToLines("/Users/magnushyttsten/201226_dbg_success/op_file.txt");
  cout << "File contained: " << flines.size() << " number of lines (command invokations)" << endl;

  // Create key: "cmd1.cmd2", value: [originalLine]
  // Example lines:
  //   -rw-r--r--  1 magnushyttsten  wheel     609 Dec 26 20:42 tf.dbg.9890.no_op_gen_cc.no_op_gen_cc.txt
  //   -rw-r--r--  1 magnushyttsten  wheel      83 Dec 26 20:42 tf.dbg.9890.no_op_gen_cc.register_op.txt
  //   -rw-r--r--  1 magnushyttsten  wheel    6142 Dec 26 20:41 tf.dbg.7037.gen_tpu_ops_py_wrappers_cc.register_op.txt
  //  So examples of unique keys: o_op_gen_cc.no_op_gen_cc, no_op_gen_cc.register_op, gen_tpu_ops_py_wrappers_cc.register_op
  int countCmd = 0;
  unordered_map<string, vector<string>> hmCmd2File;
  for (auto fentry: flines) {
    vector<string> fes = CPPUtils::strSplit(fentry, " ");
    vector<string> cmdES = CPPUtils::strSplit(fes[8], ".");
    string cmd = cmdES[3] + "." + cmdES[4];
    if (hmCmd2File.count(cmd) == 0) {
      countCmd++;
      hmCmd2File[cmd] = vector<string>();
    }
    hmCmd2File[cmd].push_back(fentry);
  }
  cout << "Unique commands that was executed: " << countCmd << endl;
  
  // For every command, keep count of <python, cc, register_op> files
  unordered_map<string, tuple<int,int,int>> umCmd2Matches;
  vector<string> countExec1;
  vector<string> countExec2;
  vector<string> countExecGT2;
  for  (auto ume: hmCmd2File) {
    assert(ume.second.size() > 0);  // Must be at least one file generating key
    if (ume.second.size() == 1) countExec1.insert(countExec1.end(), ume.second.begin(), ume.second.end());
    if (ume.second.size() == 2) countExec2.insert(countExec2.end(), ume.second.begin(), ume.second.end());
    if (ume.second.size() > 2)  countExecGT2.insert(countExecGT2.end(), ume.second.begin(), ume.second.end());
    
    string cmdFull = ume.first;
    string cmdP1 = CPPUtils::strSplit(cmdFull, ".")[0];
    string cmdP2 = CPPUtils::strSplit(cmdFull, ".")[1];
    vector<string> cmdP1P = CPPUtils::strSplit(cmdP1, "_");
    string op = "";
    if (cmdP1P[0] == "gen"
        && cmdP1P[cmdP1P.size()-1] == "cc"
        && cmdP1P[cmdP1P.size()-2] == "wrappers"
        && cmdP1P[cmdP1P.size()-3] == "py") {
      op = cmdP1P[1];
      for (int i=2; i <= cmdP1P.size()-4; i++) {
        op += "_" + cmdP1P[i];
      }
      // cout << "Py parsed: " << op << ", original: " << cmdP1 << endl;
      if (cmdP1 == cmdP2) {
        if (umCmd2Matches.count(op) == 0)  umCmd2Matches[op] = tuple<int,int,int>(1,0,0);
        else { get<0>(umCmd2Matches[op])++; }
      } else if (cmdP2 == "register_op") {
        if (umCmd2Matches.count(op) == 0)  umCmd2Matches[op] = tuple<int,int,int>(0,0,1);
        else { get<2>(umCmd2Matches[op])++; }
      } else {
        assert(0);
      }
    } else if (cmdP1P[cmdP1P.size()-1] == "cc"
               && cmdP1P[cmdP1P.size()-2] == "gen"){
      op = cmdP1P[0];
      for (int i=1; i <= cmdP1P.size()-3; i++) {
        op += "_" + cmdP1P[i];
      }
      if (cmdP1 == cmdP2) {
        if (umCmd2Matches.count(op) == 0)  umCmd2Matches[op] = tuple<int,int,int>(0,1,0);
        else { get<1>(umCmd2Matches[op])++; }
      } else if (cmdP2 == "register_op") {
        if (umCmd2Matches.count(op) == 0)  umCmd2Matches[op] = tuple<int,int,int>(0,0,1);
        else { get<2>(umCmd2Matches[op])++; }
      } else {
        assert(0);
      }
    } else {
      assert(0);
    }
  }
  int umCount = 0;
  cout << "Showing execution count per op" << endl;
  for (auto e: umCmd2Matches) {
    umCount++;
  }
  
  vector<pair<string,tuple<int,int,int>>> opPyWrap;
  vector<pair<string,tuple<int,int,int>>> opCC;
  vector<pair<string,tuple<int,int,int>>> opPyWrapAndCC;
  vector<pair<string,tuple<int,int,int>>> opOther;
  unordered_map<string,int> umOpCategoryPyWrap;
  unordered_map<string,int> umOpCategoryCC;
  unordered_map<string,int> umOpCategoryPyWrapAndCC;
  for (auto e: umCmd2Matches) {
    if (get<0>(e.second) == 1 && get<2>(e.second) == 1) {
      opPyWrap.push_back({e.first, e.second});
      umOpCategoryPyWrap[e.first] = 0;
    } else if (get<1>(e.second) == 1 && get<2>(e.second) == 1) {
      opCC.push_back({e.first, e.second});
      umOpCategoryCC[e.first] = 0;
    } else if (get<0>(e.second) == 1 && get<1>(e.second) == 1 && get<2>(e.second) == 2) {
      opPyWrapAndCC.push_back({e.first, e.second});
      umOpCategoryPyWrapAndCC[e.first] = 0;
    } else {
      opOther.push_back({e.first, e.second});
    }
  }
  //"   [" + to_string(get<0>(e.second)) + ", " + to_string(get<1>(e.second)) + ", " + to_string(get<2>(e.second)) + "]: " + e.first
  //sort(opPyWrap.begin(), opPyWrap.end());
  //sort(opCC.begin(), opCC.end());
  //sort(opPyWrapAndCC.begin(), opPyWrapAndCC.end());
  //sort(opOther.begin(), opOther.end());

  cout << "Unique commands that were executed" << endl;
  cout << "...Once  during build: " << countExec1.size() << endl;
  cout << "...Twice during build: " << countExec2.size()/2 << endl;
  cout << "...>2    during build: " << countExecGT2.size() << endl;
  cout << "Total keys in um: " << umCount << " (these are unique ops binaries, regardless if generated by py_wrap or cc)" << endl;
 
  vector<string> dfiles = CPPUtils::fsGetDirectoryFiles("/Users/magnushyttsten/201226_dbg_success/ops");
  unordered_map<string, set<string>> umOpCategory2Ops;
  // Test all register_op files for same op are equivalent
  for (auto e: opPyWrapAndCC) {
    string opCategory = e.first;
    umOpCategory2Ops[opCategory] = set<string>();
    vector<string> vAll = find(dfiles, opCategory2PyWrapTxtFile(opCategory));
    vector<string> v2 = find(dfiles, "." + opCategory2CCTxtFile(opCategory));
    vAll.insert(vAll.end(), v2.begin(), v2.end());
    // cout << "Op: " << e.first << endl;
    // for (auto s: vAll) cout << "   " << s << endl;
    
    set<string> opNames;
    assert(areAllRegisterOpFilesEquivalent(vAll, opNames));
           
    umOpCategory2Ops[opCategory] = opNames;
  }

  for (auto e: opPyWrap) {
    assert(umOpCategory2Ops.count(e.first) == 0);
    vector<string> vAll = find(dfiles, opCategory2PyWrapTxtFile(e.first));
    // Only this mlir file is executed once during build process
    if (vAll.size() != 2 && vAll[0].find("gen_gen_mlir_passthrough_op_py_py_wrappers_cc") == string::npos) {
      assert(0);
    }
    set<string> opNames;
    assert(areAllRegisterOpFilesEquivalent(vAll, opNames));
    umOpCategory2Ops[e.first] = opNames;
  }
  
  for (auto e: opCC) {
    assert(umOpCategory2Ops.count(e.first) == 0);
    vector<string> vAll = find(dfiles, "." + opCategory2CCTxtFile(e.first));
    if (vAll.size() != 2) {
      assert(0);
    }
    set<string> opNames;
    assert(areAllRegisterOpFilesEquivalent(vAll, opNames));
    umOpCategory2Ops[e.first] = opNames;
  }

  // Record supersets
  unordered_map<string, vector<string>> categorySupersets;
  vector<pair<string,set<string>>> vCategoryOps;
  for (auto e: umOpCategory2Ops) { vCategoryOps.push_back(e); }
  for (int i=0; i < vCategoryOps.size(); i++) {
    if (i+1 == vCategoryOps.size()) break;
    for (int j=i+1; j < vCategoryOps.size(); j++) {
      if (doesArg1ContainArg2(vCategoryOps[i].second, vCategoryOps[j].second)) {
        if(categorySupersets.count(vCategoryOps[i].first) == 0) {
          categorySupersets[vCategoryOps[i].first] = vector<string>();
        }
        categorySupersets[vCategoryOps[i].first].push_back(vCategoryOps[j].first);
      }
      else if (doesArg1ContainArg2(vCategoryOps[j].second, vCategoryOps[i].second)) {
        if(categorySupersets.count(vCategoryOps[j].first) == 0) {
          categorySupersets[vCategoryOps[j].first] = vector<string>();
        }
        categorySupersets[vCategoryOps[j].first].push_back(vCategoryOps[i].first);
      }
    }
  }
  cout << endl << "Printing categories that are enclosing other categories:" << endl;
  for (auto e: categorySupersets) {
    cout << e.first << endl;
    for (auto e2: e.second) {
      cout << "   " << e2 << endl;
    }
  }
  
  // Map each op to the categories they are defined in
  cout << endl << "Printing ops that exist in multiple categories" << endl;
  unordered_map<string, vector<string>> op2OpCategory;
  for (auto e: vCategoryOps) {
    string category = e.first;
    set<string> ops = e.second;
    for (auto e1: ops) {
      if (op2OpCategory.count(e1) == 0) {
        op2OpCategory[e1] = vector<string>();
      } else {
        // cout << "Op existed in multiple categories: " <<  e1 << endl;
      }
      op2OpCategory[e1].push_back(category);
    }
  }
  for (auto e: op2OpCategory) {
    if (e.second.size() > 1) {
      cout << "   " << e.first << ": ";
      for (auto e1: e.second) {
        cout << e1 << ", ";
      }
      cout << endl;
    }
  }
  cout << endl << "Total number of ops: " << op2OpCategory.size() << endl;

  cout << endl << "Printing categories and ops, total # categories: " << vCategoryOps.size() << endl;
  vector<pair<string, vector<string>>> cPyWrapAndCC;
  vector<pair<string, vector<string>>> cPyWrap;
  vector<pair<string, vector<string>>> cCC;
  // vector<pair<string,set<string>>> vCategoryOps;
  // unordered_map<string,int> umOpCategoryPyWrapAndCC;
  // unordered_map<string,int> umOpCategoryPyWrap;
  // unordered_map<string,int> umOpCategoryCC;
  for (auto e: vCategoryOps) {
    if(umOpCategoryPyWrapAndCC.count(e.first) != 0) {
      umOpCategoryPyWrapAndCC.erase(e.first);
      vector<string> v; v.assign(e.second.begin(), e.second.end());
      cPyWrapAndCC.push_back(pair<string, vector<string>>(e.first, v));
    }
    else if (umOpCategoryPyWrap.count(e.first) != 0) {
      umOpCategoryPyWrap.erase(e.first);
      vector<string> v; v.assign(e.second.begin(), e.second.end());
      cPyWrap.push_back(pair<string, vector<string>>(e.first, v));
    }
    else if (umOpCategoryCC.count(e.first) != 0) {
      umOpCategoryCC.erase(e.first);
      vector<string> v; v.assign(e.second.begin(), e.second.end());
      cCC.push_back(pair<string, vector<string>>(e.first, v));
    }
  }
  assert(umOpCategoryPyWrapAndCC.size() == 0);
  assert(umOpCategoryPyWrap.size() == 0);
  assert(umOpCategoryCC.size() == 0);
  
  sort(cPyWrapAndCC.begin(), cPyWrapAndCC.end(), [](auto v1, auto v2) { return v1.first < v2.first; });
  sort(cPyWrap.begin(), cPyWrap.end(), [](auto v1, auto v2) { return v1.first < v2.first; });
  sort(cCC.begin(), cCC.end(), [](auto v1, auto v2) { return v1.first < v2.first; });
  printOpCategoryAndOps(true, true, "PyWrapAndCC", cPyWrapAndCC);
  printOpCategoryAndOps(true, false, "PyWrap", cPyWrap);
  printOpCategoryAndOps(false, true, "CC", cCC);
}
