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

using namespace std;

struct CPPUtils {
  static vector<string> strSplit(string s, string delim, bool trim=true);
  static string strTrim(string s);
  static string strReplace(string inputStr, string strOld, string strNew);
  static int strCountOccurrences(string s, string occ);
  
  static string dateNowAs_YYMMDDSHHMMSS();

  static string exec(const string& cmd);
  static string getMyProgramName();
  
  static vector<string> fsGetDirectoryFiles(string directory);
  static vector<string> fsFileReadToLines(string file);
  static string fsFileReadToString(string file);
  static void fsFileWrite(string file, string content);
};

#endif /* CPPUtils_h */
