//
//  CPPUtils.h
//  FundLifter
//
//  Created by Magnus Hyttsten on 12/19/20.
//  Copyright © 2020 Magnus Hyttsten. All rights reserved.
//

#ifndef CPPUtils_h
#define CPPUtils_h

#include <string>
#include <vector>
#include <set>

using namespace std;

struct CPPUtils {
  static vector<string> getLines(istream& is);
  static vector<string> strSplit(const string& s, const string& delim, bool trim=true);
  static bool strEndsWith(const string& s, const string& match);
  static bool strStartsWith(const string& s, const string& match);
  static string strTrim(const string& s, set<char> trim = {});
  static string strReplace(const string& inputStr, const string& strOld, const string& strNew);
  static int strCountOccurrences(const string& s, const string& occ);
  
  static string dateNowAs_YYMMDDSHHMMSS();
  static tm dateTMFrom_YYYYCMMCDDSHHMMSS(string str);

  static string exec(const string& cmd);
  static string getMyProgramName();
  
  static vector<string> fsGetDirectoryFiles(const string& directory);
  static vector<string> fsFileReadToLines(const string& file);
  static string fsFileReadToString(const string& file);
  static void fsFileWrite(const string& file, const string& content);
};

#endif /* CPPUtils_h */
