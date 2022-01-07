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
#include <set>
#include <unistd.h>
#include <filesystem>
using namespace std;

vector<string> CPPUtils::getLines(istream& is) {
  vector<string> r;
  std::string line;
  while (getline(is, line)) {
    r.push_back(line);
  }
  return r;
}
vector<string> CPPUtils::strSplit(const string& argStr, const string& delim, bool trim) {
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

string CPPUtils::strTrim(const string& inStr, set<char> trim) {
  trim.insert({' ', '\n', '\t', '\r'});  // Trim is a copy (default empty) so ok
  string s = inStr;
  bool c = false;
  do {
    if (s.size() == 0) break;
    c = false;
    if (trim.count(s[0]) > 0) {
      s.erase(s.begin());
      c = true;
    }
  } while (c);
  c = false;
  do {
    c = false;
    if (s.length() == 0) return "";
    if (trim.count(s[s.size()-1]) > 0) {
      s.erase(s.end()-1);
      c = true;
    }
  } while (c);
  return s;
}

string CPPUtils::dateNowAs_YYMMDDspcHHMMSS() {
  std::time_t t = std::time(0);
  std::tm tm = *std::localtime(&t);
  char timeNow[14];
  std::strftime(timeNow, sizeof(timeNow), "%y%m%d:%H%M%S", &tm);
  std::string timeNowStr(timeNow);
  return timeNow;
}

tm CPPUtils::date_YYYYcolMMcolDDspcHHcolMMcolSS_To_tm(string str) {
   // Initialize from localtime to get current DST settings, etc
   time_t rawtime;
   time (&rawtime);
   tm* tml = localtime(&rawtime);
   
   vector<string> csDateTime = strSplit(str, " ");
   vector<string> csDate = strSplit(csDateTime[0], ":");
   vector<string> csTime = strSplit(csDateTime[1], ":");
   
   tml->tm_year = stoi(strTrim(csDate[0]))-1900;
   tml->tm_mon = stoi(strTrim(csDate[1])) - 1;
   tml->tm_mday = stoi(strTrim(csDate[2]));
   tml->tm_hour = stoi(strTrim(csTime[0]));
   tml->tm_min = stoi(strTrim(csTime[1]));
   tml->tm_sec = stoi(strTrim(csTime[2]));
   
   // Magic, need to adjust DST based on date & time, otherwise hour might change later to accomodate for DST
   tm tmDetectIsDST = *tml;
   mktime(&tmDetectIsDST);
   tml->tm_isdst = tmDetectIsDST.tm_isdst;
   
   return *tml;
}

tm CPPUtils::date_UnixlslT_to_tm(string str) {
   // Initialize from localtime to get current DST settings, etc
   time_t rawtime;
   time (&rawtime);
   tm* tml = localtime(&rawtime);
   
   vector<string> csDateTime = strSplit(str, " ");
   vector<string> csTime = strSplit(csDateTime[7], ":");
   string mm = csDateTime[5];
   string dd = csDateTime[6];
   string yyyy = csDateTime[8];
   
   int mmInt = -1;
   if (mm == "Jan") mmInt = 1;
   else if (mm == "Feb") mmInt = 2;
   else if (mm == "Mar") mmInt = 3;
   else if (mm == "Apr") mmInt = 4;
   else if (mm == "May") mmInt = 5;
   else if (mm == "Jun") mmInt = 6;
   else if (mm == "Jul") mmInt = 7;
   else if (mm == "Aug") mmInt = 8;
   else if (mm == "Sep") mmInt = 9;
   else if (mm == "Oct") mmInt = 10;
   else if (mm == "Nov") mmInt = 11;
   else if (mm == "Dec") mmInt = 12;
   else {
      cerr << "ERROR, count not match month string from Unix timestamp (ls -lT): " << mm << endl;
      exit(-1);
   }

   tml->tm_year = stoi(strTrim(yyyy))-1900;
   tml->tm_mon = mmInt - 1;
   tml->tm_mday = stoi(dd);
   tml->tm_hour = stoi(strTrim(csTime[0]));
   tml->tm_min = stoi(strTrim(csTime[1]));
   tml->tm_sec = stoi(strTrim(csTime[2]));
   
   // Magic, need to adjust DST based on date & time, otherwise hour might change later to accomodate for DST
   tm tmDetectIsDST = *tml;
   mktime(&tmDetectIsDST);
   tml->tm_isdst = tmDetectIsDST.tm_isdst;
   
   return *tml;
}

string CPPUtils::date_tm_To_YYYYcolMMcolDDspcHHcolMMcolSS(const tm& tme) {
   char buffer[20];
   std::strftime(buffer, 20, "%Y:%m:%d %H:%M:%S", &tme);
   string dateStr = string(buffer);
   return dateStr;
}

int CPPUtils::strCountOccurrences(const string& s, const string& occ) {
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

string CPPUtils::strReplace(const string& inputStr, const string& strOld, const string& strNew) {
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

vector<string> CPPUtils::fsGetDirectoryFiles(const string& directory) {
  vector<string> r;
  for (const auto & entry : std::__fs::filesystem::directory_iterator(directory)) {
    r.push_back(entry.path());
  }
  return r;
}

vector<string> CPPUtils::fsFileReadToLines(const string& file) {
  ifstream infile(file);
  assert(infile);
  string line;
  vector<string> flines;
  while (std::getline(infile, line)) {
    flines.push_back(line);
  }
  return flines;
}

bool CPPUtils::strEndsWith(const string& s, const string& match) {
  size_t idx = s.find(match);
  if (idx != string::npos && idx+match.size() == s.size()) {
    return true;
  }
  return false;
}
bool CPPUtils::strStartsWith(const string& s, const string& match) {
  size_t idx = s.find(match);
  if (idx != string::npos && idx == 0) {
    return true;
  }
  return false;
}

string CPPUtils::fsFileReadToString(const string& file) {
  std::ifstream f(file);
  std::stringstream buffer;
  buffer << f.rdbuf();
  return buffer.str();
}

void CPPUtils::fsFileWrite(const string& file, const string& content) {
  ofstream os(file);
  os.write(content.c_str(), sizeof(char)*content.size());
  os.close();
}

bool CPPUtils::fsFileRemove(const string& file) {
   int r = remove(file.c_str());
   if (r == 0)
      return true;
   return false;
}


