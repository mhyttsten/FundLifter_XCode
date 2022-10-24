//
//  Utils.cpp
//  FundLifter
//
//  Created by Magnus Hyttsten on 10/10/22.
//  Copyright © 2022 Magnus Hyttsten. All rights reserved.
//

#include <iostream>
#include <string>
#include "Utils.h"

using namespace std;

const char charArchive = 0x02;

int getLineNumber(string in, size_t idx_end) {
   int lineNumber = 1;
   size_t idx_current = 0;
   while (true) {
      size_t idx_nextEOL = in.find("\n", idx_current);
      if (idx_nextEOL != string::npos && idx_nextEOL < idx_end) {
         idx_current = idx_nextEOL + string("\n").size();
         lineNumber++;
      } else {
         break;
      }
   }
   return lineNumber;
}

bool isWhitespace(char c) {
   bool r = c == ' ' || c == '\n' || c == '\t' || c == charArchive;  // 0x02 as that is our encoded value
   return r;
}

bool isIdentifierCharacter(char c) {  // ':'=constructor/destructor, "<>"=templates
   return isalnum(c) || c == ':' || c == '_' || c == '<' || c == '>' || c == '~';
}

bool isReturnTypeCharacter(char c) {
   return isIdentifierCharacter(c) || c == '*' || c == '&';
}

size_t backwardsSkippingWhiteSpaces(string in, size_t idx) {
   while (isWhitespace(in[idx])) idx--;
   return idx;
}

size_t backwardsToNonIdentifier(string in, size_t idx) {
   // Allow '::' to include class name, if any
   // size_t orig_idx = idx;
   bool found = false;
   // ':' for scoping, '<', '>' for template specification
   while (isIdentifierCharacter(in[idx]) || in[idx] == '~') {  // '~' to support destructor
      found = true;
      if (idx == 0) return 0;
      idx--;
   }
   // Move up to last found alnum
   if (found) idx++;
   return idx;
}

size_t forwardToNonIdentifier(string in, size_t idx) {
   bool found = false;
   while (isIdentifierCharacter(in[idx]) || in[idx] == '~') {  // '~' to support destructor
      found = true;
      idx++;
   }
   if (!found) return string::npos;
   return idx;
}

size_t forwardToNonWhiteSpace(string in, size_t idx) {
   while (isWhitespace(in[idx])) {
      idx++;
   }
   return idx;
}

void populateIdentifiersAfter(unordered_map<string, void*>& m, string fin, string key) {
   size_t idx_scanner = 0;
   while(true) {
      size_t idx_class  = fin.find(key, idx_scanner);
      if (idx_class == string::npos) return;
      //    cout << "Parsing: [" << fin.substr(idx_class, (fin.size()-idx_class > 20? 20 : fin.size()-idx_class)) << "]\n";
      idx_scanner = idx_class + key.size();
      while (isWhitespace(fin[idx_scanner])) idx_scanner++;
      size_t idx_scanner_end = idx_scanner;
      while (isalnum(fin[idx_scanner_end]) || fin[idx_scanner_end] == '_') idx_scanner_end++;
      
      // Ok, now backtrack, if character before is '<' or ',' then no class, a template thing
      size_t idx_trace_before = idx_scanner;
      idx_trace_before = fin.rfind(key, idx_trace_before);
      assert(idx_trace_before != string::npos);
      // Backtrack if not the beginning of string
      if (idx_trace_before > 0) {
         idx_trace_before--;
         // Backtrack until no whitespace
         while (isWhitespace(fin[idx_trace_before])) {
            idx_trace_before--;
         }
         // Real character before class declraration
         char tchar = fin[idx_trace_before];
         if (tchar != '<' && tchar != ',') {
            string c = fin.substr(idx_scanner, idx_scanner_end-idx_scanner);
            if (c == "T") {
               cout << "Opportunity\n";
               cout << fin.substr(idx_scanner-20, 40) << endl;
            }
            m[c] = nullptr;
         }
      }
      idx_scanner = idx_scanner_end;
      //    cout << "Added class: [" << c << "]\n";
   }
}
