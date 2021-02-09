//
//  CPPUtils.cpp
//  FundLifter
//
//  Created by Magnus Hyttsten on 12/19/20.
//  Copyright © 2020 Magnus Hyttsten. All rights reserved.
//

#include "CPPUtils.h"
#include <array>
#include <cstdio>
#include <ctime>
#include <fstream>

#include <iostream>
#include <memory>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <sstream>
#include <time.h>
#include <unistd.h>
#include <filesystem>
using namespace std;

vector<string> CPPUtils::strSplit(string argStr, string delim, bool trim) {
  string inStr = argStr;
  vector<string> r;
  
  if (delim == " " && trim) {
    while (inStr.length() > 0 && inStr.at(0) == ' ') {
      inStr.erase(0, 1);
    }
  }
  auto io = inStr.find(delim);
  while (io != std::string::npos) {
    string s = inStr.substr(0, io);
    if (trim) s = strTrim(s);
    r.push_back(s);
    inStr.erase(0, io+delim.length());
    if (delim == " " && trim) {
      while (inStr.length() > 0 && inStr.at(0) == ' ') {
        inStr.erase(0, 1);
      }
    }
    io = inStr.find(delim);
  }
  if (trim) inStr = strTrim(inStr);
  r.push_back(inStr);
  return r;
}

string CPPUtils::strTrim(string inStr) {
  string s = inStr;
  bool c = false;
  do {
    c = false;
    if (s.find(' ') == 0 || s.find('\n') == 0 || s.find('\t') == 0 || s.find('\r') == 0) {
      s.erase(s.begin());
      c = true;
    }
  } while (c);
  c = false;
  do {
    c = false;
    if (s.length() == 0) return "";
    auto pos = s.length() - 1;
    if (s.rfind(' ') == pos || s.rfind('\n') == pos || s.rfind('\t') == pos || s.rfind('\r') == pos) {
      s.erase(s.end()-1);
      c = true;
    }
  } while (c);
  return s;
}


string CPPUtils::dateNowAs_YYMMDDSHHMMSS() {
  std::time_t t = std::time(0);
  std::tm tm = *std::localtime(&t);
  char timeNow[14];
  std::strftime(timeNow, sizeof(timeNow), "%y%m%d:%H%M%S", &tm);
  std::string timeNowStr(timeNow);
  return timeNow;
}

int CPPUtils::strCountOccurrences(string s, string occ) {
  size_t curr = 0;
  int count = 0;
  while (true) {
    curr = s.find(occ, curr);
    if (curr == string::npos) break;
    curr++;
    count++;
  }
  return count;
}

string CPPUtils::strReplace(string inputStr, string strOld, string strNew) {
   string str = inputStr;
   size_t pos = str.find(strOld);
   if (pos == string::npos) return str;
   string result;
   while (pos != string::npos) {
      result += str.substr(0, pos) + strNew;
      str = str.substr(pos+strOld.length());
      pos = str.find(strOld);
   }
   result += str;
   return result;
}

string CPPUtils::exec(const string& cmd) {
  array<char, 1000000> buffer;
  string result;
  unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
  if (!pipe) {
    throw std::runtime_error("popen() failed!");
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  return result;
}

string CPPUtils::getMyProgramName() {
  string execOutput = exec("ps -ef");
  int myPID = getpid();
  
  istringstream ss(execOutput);
  string line;
  bool first = true;
  string result = "NullCmd";
  while (getline(ss, line)) {
    if (first) {  // This is the non-numeric headers
      first = false;
      continue;
    }
    // cout << "Line: " << line << endl;
    vector<string> vs = strSplit(line, " ");
    int cmdPID = stoi(vs[1], nullptr, 10);
    //    cout << "PID: " << vs[idxPID] << ", CMD: " << vs[idxCmd] << endl;
    if (myPID == cmdPID) {
      // cout << "PID: " << cmdPID << ", CMD: " << vs[7] << endl;
      string cmdResult = vs[7];
      size_t io = cmdResult.rfind("/");
      if (io != string::npos && io+1 < cmdResult.length()-1) {
        result = cmdResult.substr(io+1);
      }
    }
  }
  // cout << "Result is: " << result << endl;
  return result;
}

vector<string> CPPUtils::fsGetDirectoryFiles(string directory) {
  vector<string> r;
  for (const auto & entry : std::__fs::filesystem::directory_iterator(directory)) {
    r.push_back(entry.path());
  }
  return r;
}

vector<string> CPPUtils::fsFileReadToLines(string file) {
  ifstream infile(file);
  assert(infile);
  string line;
  vector<string> flines;
  while (std::getline(infile, line)) {
    flines.push_back(line);
  }
  return flines;
}

string CPPUtils::fsFileReadToString(string file) {
  std::ifstream f(file);
  std::stringstream buffer;
  buffer << f.rdbuf();
  return buffer.str();
}

void CPPUtils::fsFileWrite(string file, string content) {
  ofstream os(file);
  os.write(content.c_str(), sizeof(char)*content.size());
  os.close();
}

