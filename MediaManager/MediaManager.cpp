//
//  main.cpp
//  PhotoDateSetter
//
//  Created by Magnus Hyttsten on 4/24/21.
//  Copyright © 2021 Magnus Hyttsten. All rights reserved.
//

/*
 String Split
 string parsed,input="text to be parsed";
 stringstream input_stringstream(input);
 if (getline(input_stringstream,parsed,' '))
 {
      // do some processing.
 }
 auto io = inStr.find(delim);
 while (io != std::string::npos) {
    string s = inStr.substr(0, io);
 */

// Exiftool commands:
//    $ exiftool -F -AllDates="YYYY:MM:DD HH:MM:SS" <fileName>
//    $ exiftool -AllDaets <fileName>
// Get exif date by filename
// Set exif and Unix datetime on filename
// Command-line
//    Get & print Unix/exif datetime for files, also validating that all O|C|M exif dates are set
//       $ <cmd> -getd <files>
//    Set Unix/exif datetime for files
//       $ <cmd> -setd -date=YYYYMMDDHHMMSS [-interval=<seconds>] <files>
//    Set filenames
//      $ <cmd> -setn -prefix=<alphanum> -date="YYYY:MM:DD HH:MM:SS" | AsExif] -name=<fname>
//         <prefix>_YYYYMMDDHHMMSS_<name>_nnnnn.<ext>
//    Normalize so that all exif dates are the same, and Unix is set to them
//       $ <cmd> -normalize <files>
//
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <regex>
#include "CPPUtils.h"
#include "MMFileDateEntity.h"

using namespace std;

MMTMWrapper getUnixTMFromFilename(const string& fname);
void unix_SetAllDates(const string& file, const tm& unixDateTime);
string getStringForDates(const MMFileDateEntity& fde, const MMTMWrapper& );
int cmd_getd(int argc, const char* argv[]);
int cmd_setd(int argc, const char* argv[]);
int cmd_setn(int argc, const char* argv[]);
int cmd_normalize(int argc, const char* argv[]);
MMFileDateEntity exiftool_getAllDates(const string& fileName);
void exifTool_SetAllDates(const string& file, const string& exifDateTime);

string HELP =
string("Commands") +
string("   Get & print Unix/exif datetime for files, also validating that all O|C|M exif dates are set\n") +
string("      $ <cmd> -getd <files>\n") +
string("   Set Unix/exif datetime for files\n") +
string("      $ <cmd> -setd -date=\"YYYY:MM:DD: HH:MM:SS\" [-interval=<seconds>] <files>\n") +
string("   Set filenames to: <prefix>_YYYYMMDDHHMMSS_<name>_nnnnn.<ext>\n") +
string("      $ <cmd> -setn -prefix=<alphanum> -date=\"YYYY:MM:DD HH:MM:SS\" | AsExif] -name=<fname> <files>\n") +
string("   Normalize so that all exif dates are the same, and Unix is set to them\n") +
string("      $ <cmd> -normalize <files>\n") +
string("\n") +
string("exiftool\n") +
string("   Display all date: exiftool -AllDates <file>\n") +
string("   Set all dates:    exiftool -F -AllDates=\"YYYY:MM:DD HH:MM:SS\" <file>\n") +
string("\n");

string CMD_GETD = "-getd";
string CMD_SETD = "-setd";
string CMD_SETN = "-setn";
string CMD_NORMALIZE = "-normalize";

int main(int argc, const char * argv[]) {
   std::cout << endl << "Welcome to MediaManager v2.0!\n";
   
//   vector<unsigned long long> tmpv;
//   vector<int> tmpv;
//   constexpr auto tmpce = sizeof(tmpv.back());
//   cout << "Size of thing: " << sizeof(tmpv.at(0)) << endl;
   
   if (argc == 1) {
      cout << HELP << endl;
      return 0;
   }
   
   if (argv[1] == CMD_GETD) {
      return cmd_getd(argc, argv);
   }
   else if (argv[1] == CMD_SETD) {
      return cmd_setd(argc, argv);
   }
   else if (argv[1] == CMD_SETN) {
      return cmd_setn(argc, argv);
   }
   else if (argv[1] == CMD_NORMALIZE) {
      return cmd_normalize(argc, argv);
   }
   else {
      cerr << "No command specified" << endl;
      return -1;
   }
   
   return 0;
}

int cmd_normalize(int argc, const char* argv[]) {
   if (argc <= 2) {
      cerr << "Expecting filename as argument" << endl;
      return -1;
   }
   
   int unixChanges = 0;
   int exifChanges = 0;
   vector<MMFileDateEntity> files;
   for (int i=2; i < argc; i++) {
      string fname = argv[i];
      MMFileDateEntity de = exiftool_getAllDates(fname);
      de.setFileName(fname);

      if (de.areAllSet() && de.areAllTheSame()) {
         unixChanges++;
         unix_SetAllDates(fname, de._dateOriginal._tm);
         continue;
      }
      
      MMTMWrapper unixTM = getUnixTMFromFilename(fname);

      bool modify = false;
      if (de._dateOriginal._isValid) {
         if (de._dateCreate._isValid && !(de._dateOriginal == de._dateCreate)) {
            cerr << "Cannot normalize 1: " << getStringForDates(de, unixTM._tm) << endl;
         }
         else if (de._dateModify._isValid && !(de._dateOriginal == de._dateModify)) {
            cerr << "Cannot normalize 2: " << getStringForDates(de, unixTM._tm) << endl;
         }
         else {
            modify = true;
            de._dateCreate = de._dateOriginal;
            de._dateModify = de._dateOriginal;
         }
      }
      else if (de._dateCreate._isValid) {
         if (de._dateOriginal._isValid && !(de._dateCreate == de._dateOriginal)) {
            cerr << "Cannot normalize 3: " << getStringForDates(de, unixTM._tm) << endl;
         }
         else if (de._dateModify._isValid && !(de._dateCreate == de._dateModify)) {
            cerr << "Cannot normalize 4: " << getStringForDates(de, unixTM._tm);
         }
         else {
            modify = true;
            de._dateOriginal = de._dateCreate;
            de._dateModify = de._dateCreate;
         }
      }
      else if (de._dateModify._isValid) {
         if (de._dateOriginal._isValid && !(de._dateModify == de._dateOriginal)) {
            cerr << "Cannot normalize 5: " << getStringForDates(de, unixTM._tm);
         }
         else if (de._dateCreate._isValid && !(de._dateModify == de._dateCreate)) {
            cerr << "Cannot normalize 6: " << getStringForDates(de, unixTM._tm);
         }
         else {
            modify = true;
            de._dateOriginal = de._dateModify;
            de._dateCreate = de._dateModify;
         }
      }
      
      if (modify) {
         exifChanges++;
         unix_SetAllDates(fname, de._dateOriginal._tm);
         string newdatetime = CPPUtils::date_tm_To_YYYYcolMMcolDDspcHHcolMMcolSS(de._dateOriginal._tm) + "-00:00";
         exifTool_SetAllDates(fname, newdatetime);
      }
   }
   
   cout << "Result from normalization, for total number of files: " << to_string(argc-2) << endl;
   cout << "   Exif changes: " << exifChanges << endl;
   cout << "   Unix changes: " << unixChanges << endl;
   return 0;
}


int cmd_setn(int argc, const char* argv[]) {
   // -prefix=<alphanum> -date="YYYY:MM:DD HH:MM:SS" | AsExif] -name=<fname>
   cout << "Assigning filenames" << endl;
   int argcf = 2;

   // Get the prefix
   string prefix = "";
   if (argc < 3 || string(argv[2]).find("-prefix=") != 0) {
      cerr << "Command " << CMD_SETD << " requires -prefix= parameter" << endl;
      return -1;
   }
   prefix = argv[2];
   prefix = prefix.substr(8, prefix.size()-1);
   argcf++;

   // Get the date
   string date = "";
   if (argc < 4 || string(argv[3]).find("-date=") != 0) {
      cerr << "Command " << CMD_SETD << " requires -date= parameter" << endl;
      return -1;
   }
   date = argv[3];
   date = date.substr(6, date.size()-1);
   argcf++;

   // Get the base name
   string name = "";
   if (argc < 5 || string(argv[4]).find("-name=") != 0) {
      cerr << "Command " << CMD_SETD << " requires -name= parameter" << endl;
      return -1;
   }
   name = argv[4];
   name = name.substr(6, name.size()-1);
   argcf++;

   // Get the files
   vector<MMFileDateEntity> files;
   for (int i=argcf; i < argc; i++) {
      string fname = argv[i];
      MMFileDateEntity de = exiftool_getAllDates(fname);
      de.setFileName(fname);
      files.push_back(de);
   }

   // Sort according to exif dates or filename
   auto sortExif     = [](const MMFileDateEntity& v1, const MMFileDateEntity& v2) { return v1._dateOriginal < v2._dateOriginal; };
   auto sortFilename = [](const MMFileDateEntity& v1, const MMFileDateEntity& v2) { return v1._fileName < v2._fileName; };
   if (date == "AsExif") {
      for (auto e: files) {
         if (!e.areAllSet() || !e.areAllTheSame()) {
            cerr << "Not all exif dates were the same for: " << e._fileName << endl;
            return -1;
         }
      }
      sort(files.begin(), files.end(), sortExif);
   } else {
      regex reDateTime{R"(\d{4}:\d{2}:\d{2} \d{2}:\d{2}:\d{2})"};
      smatch reResult;
      if (!regex_search(date, reResult, reDateTime)) {
         cerr << "Incorrect date format: " << date << endl;
         return -1;
      }
      sort(files.begin(), files.end(), sortFilename);
   }
   
   // Generate filenames
   int index = 0;
   for (auto e: files) {
      // <prefix>_YYYYMMDDHHMMSS_<name>_nnnnnn.<ext>
      
      string basePart = "";
      size_t basePos = e._fileName.rfind(__fs::filesystem::path::preferred_separator);
      if (basePos != string::npos) {
         basePart = e._fileName.substr(0, basePos+1);
      }

      index += 10;
      string indexStr = to_string(index);
      while (indexStr.size() < 6) indexStr = "0" + indexStr;

      string newDate = date;
      if (date == "AsExif") {
         newDate = CPPUtils::date_tm_To_YYYYcolMMcolDDspcHHcolMMcolSS(e._dateOriginal._tm);
         newDate = newDate.substr(0,4) + newDate.substr(5, 2) + newDate.substr(8, 2) + newDate.substr(11, 2) + newDate.substr(14, 2) + newDate.substr(17, 2);
      } else {
         newDate = date.substr(0,4) + date.substr(5, 2) + date.substr(8, 2) + date.substr(11, 2) + date.substr(14, 2) + date.substr(17, 2);
      }

      size_t extPos = e._fileName.rfind(".");
      string ext = e._fileName.substr(extPos, e._fileName.size()-extPos);

      string fname = basePart + prefix + "_" + newDate + "_" + name + "_" + indexStr + ext;

      // cout << "Old: " << e._fileName << ", New: " << fname << endl;
      int rc = rename(e._fileName.c_str(), fname.c_str());
      if (rc != 0) {
         cerr << "Could not rename file. Old: " << e._fileName << ", New: " << fname << endl;
         return -1;
      }
   }
   cout << "Done, set: " << files.size() << " files" << endl;
   return 0;
}

int cmd_setd(int argc, const char* argv[]) {
   int argcf = 2;

   // Get the base date
   if (argc < 3 || string(argv[2]).find("-date=") != 0 || string(argv[2]).size() != 25) {
      cerr << "Command " << CMD_SETD << " requires -date=\"YYYY:MM:DD HH:MM:SS\" parameter" << endl;
      return -1;
   }
   string sdate = string(argv[2]);
   sdate = sdate.substr(6, sdate.size()-6);
   cout << "   Date is: [" << sdate << "]" << endl;
   regex reDateTime{R"(\d{4}:\d{2}:\d{2} \d{2}:\d{2}:\d{2})"};
   smatch reResult;  // vector<string>
   if (regex_search(sdate, reResult, reDateTime)) {
      // Great, format is: "YYYY:MM:SS HH:MM:SS" confirmed
   } else {
      cerr << "Error, encountered weird date format: [" << sdate << "]" << endl;
      assert(false);
   }
   argcf++;
   
   // Get the interval (if any)
   int interval = 5;  // Default interval is 5s
   if (argc >= 4 && string(argv[3]).find("-interval=") == 0) {
      string sinterval = string(argv[3]);
      if (sinterval.size() <= 10) {
         cerr << "Command " << CMD_SETD << " -interval= specified, expecting number of seconds" << endl;
         return -1;
      }
      sinterval = sinterval.substr(10, sinterval.size()-10);
      for (auto e: sinterval) {
         if (!isdigit(e)) {
            cerr << "Command " << CMD_SETD << " -interval= specified, expecting number of seconds" << endl;
            return -1;
         }
      }
      argcf++;
      interval = stoi(sinterval);
   }
   cout << "   Interval is: [" << interval << "] seconds" << endl;
   
   // Get files
   vector<string> files;
   for (int i=argcf; i < argc; i++) {
      string fname = argv[i];
      files.push_back(fname);
      // cout << "Added file: " << fname << endl;
   }
   sort(files.begin(), files.end());
   
   // Set dates
   tm valueTM = CPPUtils::date_YYYYcolMMcolDDspcHHcolMMcolSS_To_tm(sdate);
   for (auto e: files) {
      string newdatetime = CPPUtils::date_tm_To_YYYYcolMMcolDDspcHHcolMMcolSS(valueTM) + "-00:00";
      exifTool_SetAllDates(e, newdatetime);
      unix_SetAllDates(e, valueTM);
      valueTM.tm_sec += interval;
      mktime(&valueTM);
   }
   
   return 0;
}

int cmd_getd(int argc, const char* argv[]) {
   if (argc <= 2) {
      cerr << "Error: Expecting at least one file argument" << endl;
      return -1;
   }
   
   vector<string> files;
   for (int i=2; i < argc; i++) {
      files.push_back(argv[i]);
   }
   sort(files.begin(), files.end());
   
   // Extracting exif date/time for files
   MMTMWrapper first;
   MMTMWrapper last;
   bool hasPrevious = false;
   bool isSequential = true;
   bool isUnixInSync = true;
   MMTMWrapper previous;
   
   vector<string> out;
   for (auto e: files) {
      MMFileDateEntity de = exiftool_getAllDates(e);
      de.setFileName(e);
      string
         prefix = "[OK]          ";
      if (!de.areAllSet()) {
         prefix = "[*E_NOTALLSET]";
      }
      if (!de.areAllTheSame()) {
         prefix = "[*E_NOTALLEQ] ";
      }

      // Validate completeness and sequentialness of exifdate times
      MMTMWrapper unixTM = getUnixTMFromFilename(de._fileName);
      if (!de.areAllSet() || !de.areAllTheSame()) {
         isSequential = false;
         isUnixInSync = false;
      }
      if (isSequential && !hasPrevious) {
         hasPrevious = true;
         previous = de._dateOriginal;
         
         if (!(previous == unixTM)) {
            isUnixInSync = false;
         }
      }
      else if (isSequential) {
         if (!(previous < de._dateOriginal)) {
            isSequential = false;
            isUnixInSync = false;
            // cout << "*** " << de._fileName << " not sync" << ", date: " << de._dateOriginal.getString() << endl;
         } else {
            previous = de._dateOriginal;
            if (!(previous == unixTM)) {
               isUnixInSync = false;
            }
         }
      }
      
      MMTMWrapper deEarliest = de.getEarliest();
      if (deEarliest._isValid) {
         if (!first._isValid || deEarliest < first)
            first = deEarliest;
      }
      MMTMWrapper deLatest = de.getLatest();
      bool isLess = last < deLatest;
      if (deLatest._isValid) {
         if (!last._isValid || isLess) {
            last = deLatest;
         }
      }

      out.push_back(prefix + " " + getStringForDates(de, unixTM) + "    " + e);
   }
   
   cout << "Files processed" << endl;
   for (auto e: out) {
      cout << "   " << e << endl;
   }
   cout << endl << "Report for files" << endl;
   cout << "   Earliest date:         " << first.getString() << endl;
   cout << "   Latest date:           " << last.getString() << endl;
   cout << "   Exif dates sequential: " << (isSequential ? "true": "false") << endl;
   cout << "   Unix dates in sync:    " << (isUnixInSync ? "true": "false") << endl;
   cout << endl;

   return 0;
}

void unix_SetAllDates(const string& file, const tm& unixDateTime) {
   char unixbuffer[16];  // YYYYMMDDHHMM.SS
   std::strftime(unixbuffer, 16, "%Y%m%d%H%M.%S", &unixDateTime);
   string unixstr = string(unixbuffer);
   string cmd = "touch -a -m -t " + unixstr + " \"" + file + "\"";
   string result = CPPUtils::exec(cmd);
}

void exifTool_SetAllDates(const string& file, const string& exifDateTime) {
   string cmd = "/usr/local/bin/exiftool -F -AllDates=\"" + exifDateTime + "\" \"" + file + "\"";
   // cout << "Executing cmd: " << cmd << endl;
   string result = CPPUtils::exec(cmd);
   if (result.find("    1 image files updated") != 0) {
      if (result.find("\n") == result.size()-1) {
         result = result.substr(0, result.size()-1);
      }
      cerr << "*** ERROR: Could not set exiftool date for file: " << file << ", error is: " << result << endl;
   }
   CPPUtils::fsFileRemove(file + "_original");
}

// ***************************************************************************

MMFileDateEntity exiftool_getAllDates(const string& fileName) {
   string result = CPPUtils::exec("/usr/local/bin/exiftool -AllDates \"" + fileName + "\"");
   
   vector<string> lines = CPPUtils::strSplit(result, "\n");
   MMFileDateEntity r = MMFileDateEntity();
   
   for (auto e: lines) {
      if (e.size() == 0) {
         continue;
      }
      size_t colonPos = e.find(":");
      
      if (colonPos == string::npos) {
         cerr << "Did not find colon in exiftool date element: " << e << endl;
         assert(false);
      }
      
      if (e.size() > colonPos+2) {
         string key = e.substr(0, colonPos);
         key = CPPUtils::strTrim(key);
         
         string value = e.substr(colonPos+1, e.size()-colonPos+1);
         value = CPPUtils::strTrim(value);
         
         if (value.size() > 0) {
            regex reDateTime{R"(\d{4}:\d{2}:\d{2} \d{2}:\d{2}:\d{2})"};
            smatch reResult;  // vector<string>
            if (regex_search(value, reResult, reDateTime)) {
               // Great, format is: "YYYY:MM:SS HH:MM:SS" confirmed
            } else {
               cerr << "Error, encountered weird date format: [" << value << "]" << endl;
               assert(false);
            }
            
            tm valueTM = CPPUtils::date_YYYYcolMMcolDDspcHHcolMMcolSS_To_tm(value);
            if (key == "Date/Time Original") {
               r.setDateOriginal(valueTM);
            } else if (key == "Create Date") {
               r.setDateCreate(valueTM);
            } else if (key == "Modify Date") {
               r.setDateModify(valueTM);
            } else {
               cerr << "Unknown key: " << key << endl;
               assert(false);
            }
         }  // End-of has a value
      }  // End-of has a value
   }  // End of for over list
   return r;
}

string getStringForDates(const MMFileDateEntity& exifTime, const MMTMWrapper& unixTime) {
   string s = "OCMU:[";
   s += exifTime._dateOriginal.getString();
   if (exifTime._dateOriginal == exifTime._dateCreate)
      s += "|_______ORIG________";
   else
      s += exifTime._dateCreate.getString();
   s += "|";
   if (exifTime._dateOriginal == exifTime._dateModify)
      s += "_______ORIG________";
   else
      s += exifTime._dateModify.getString();
   s += "|";
   if (exifTime._dateOriginal == unixTime)
      s += "_______ORIG________";
   else
      s += unixTime.getString();
   
   return s + "]";
}

MMTMWrapper getUnixTMFromFilename(const string& fname) {
   string cmd = "ls -lT \"" + fname + "\"";
   string result = CPPUtils::exec(cmd);
   MMTMWrapper unixTM = CPPUtils::date_UnixlslT_to_tm(result);
   return unixTM;
}

