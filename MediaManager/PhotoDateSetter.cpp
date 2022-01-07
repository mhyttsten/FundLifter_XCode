#include <stdio.h>      /* puts */
//#include <time.h>       /* time_t, struct tm, time, localtime, strftime */

//#include <cstdio>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include "CPPUtils.h"

// exiftool -AllDates <fileName>
//    Date/Time Original              : 2021:04:16 18:53:41
//    Create Date                     : 2021:04:16 18:53:41
//    Modify Date                     : 2021:04:16 18:53:41
// exiftool -F -AllDates="YYYY:MM:DD HH:MM:SS" <fileName>

// ***************************************************************************
struct FileInfo {
public:
    FileInfo(string fname, string o, string c, string m) { _fname = fname; _o = o; _c = c; _m = m; }
    string to_string()  {
      return _fname + ", o: " + _o + ", c: " + _c + ", m: " + _m;
    }
    string _fname;
    string _o, _c, _m;
    bool getDate_YYYYMMDD(string& result, const string& d, const string& sep) {
    }
    bool getTime_HHMMDD  (string& result, const string& d, const string& sep) {
    }
};

// ***************************************************************************
using namespace std;
FileInfo getFileInfo(string fileName);
int main(int argc, char* argv[]) {
  cout << "Starting" << endl;
  string directory = ".";
  if (argc >= 2) {
    directory = string(argv[1]);
  }
  cout << "Directory is: " << directory << endl;

  vector<string> files = CPPUtils::fsGetDirectoryFiles(directory);
  sort(files.begin(), files.end(), [](const auto& v1, const auto& v2) {
      return (v1 == v2) ? false : v1 < v2  ;
  });
  cout << "Have now sorted the file list for: " << files.size() << " number of files" << endl;

  vector<FileInfo> finfos;
  for (auto fname: files) {
    FileInfo fi = getFileInfo(fname);
    finfos.push_back(fi);
  }

  cout << "Files\n";
  for (auto fi: finfos) {
    cout << fi.to_string() << endl;
  }

//  cout << "Assign dates: [y/N]: ";
//  string answer;
//  cin >> answer;
//  std::transform(answer.begin(), answer.end(), answer.begin(), [](unsigned char c){ return std::tolower(c); });
//  if (answer != "y") {
//    cout << "Exiting without modification" << endl;
//    return 0;
//  }
//
//  int addSeconds = 10;
//  tm tml = CPPUtils::dateTMFrom_YYYYCMMCDDSHHMMSS(argv[1]);
//
//  cout << "Performing date modifications" << endl;
//  for (auto fileName: files) {
//    mktime(&tml);
//    // cout << (tml.tm_year+1900) << ":" << (tml.tm_mon+1) << ":" << tml.tm_mday << " " << tml.tm_hour << ":" << tml.tm_min << ":" << tml.tm_sec << endl;
//    char buffer[20];
//    std::strftime(buffer, 20, "%Y:%m:%d %H:%M:%S", &tml);
//    string dateStr = string(buffer);
//    cout << ".";
//    cout.flush();
//    string command = "exiftool -F -AllDates=\"" + dateStr + "\" " + fileName;
//    //    cout << "Executing command: " << command << endl;
//    string out = CPPUtils::exec(command);
//    string rmFileName = fileName + "_original";
//    remove(rmFileName.c_str());
//    tml.tm_sec += addSeconds;
//  }
//  cout << endl;

  cout << "Done" << endl;
}

//-----------------------------------------------------------------------
// 0: original, 1: create, 2: modify
FileInfo getFileInfo(string fileName) {
  string out = CPPUtils::exec("exiftool -AllDates " + fileName);
  vector<string> rows = CPPUtils::strSplit(out, "\n", true);
  string dateOriginal = "";
  string dateModify = "";
  string dateCreate = "";
  for (auto row: rows) {
    if (CPPUtils::strTrim(row).size() == 0) {
      continue;
    }
    size_t idx = row.find(":", 0);
    assert(idx != string::npos);
    string legend = CPPUtils::strTrim(row.substr(0, idx));
    string timeStamp = CPPUtils::strTrim(row.substr(idx+1, row.size()-idx));
    if (legend == "Date/Time Original") {
      dateOriginal = timeStamp;
    } else if (legend == "Create Date") {
      dateCreate = timeStamp;
    } else if (legend == "Modify Date") {
      dateModify = timeStamp;
    }
    else {
      assert(false);
    }
  }
  return FileInfo(fileName, dateOriginal, dateCreate, dateModify);
}
