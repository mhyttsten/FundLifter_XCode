//
//  ReduceRepetitiveBlocks_WIP.cpp
//  FundLifter
//
//  Created by Magnus Hyttsten on 10/10/22.
//  Copyright © 2022 Magnus Hyttsten. All rights reserved.
//

#include <stdio.h>

// ****************************************************************
struct MHReduction {
   int countOccurence = 1;
   string functionName;
   string prefix;
   vector<string> params;  // E.g. [str: "hello", fileSystem: "gcs"]
   string location;
   
   bool isComment = false;
   string commentLine;
   
   MHReduction(const string& in) {
      
      if (CPPUtils::strStartsWith(CPPUtils::strTrim(in), "//")) {
         isComment = true;
         commentLine = in;
         return;
      }
      
      size_t idx_lb = in.find("[");
      functionName = in.substr(0, idx_lb);
      size_t idx_rb = in.find("]", idx_lb);
      string countRepeatStr = in.substr(idx_lb+1, idx_rb-1-idx_lb);
      countOccurence = stoi(countRepeatStr);
      
      idx_lb = in.find("[", idx_rb);
      idx_rb = in.find("]", idx_lb);
      prefix = CPPUtils::strTrim(in.substr(idx_lb+1, idx_rb-1-idx_lb));
      
      idx_lb = in.find("[", idx_rb);
      idx_rb = in.find("]", idx_lb);
      string param = in.substr(idx_lb+1, idx_rb-1-idx_lb);
      if (param.size() > 0) {
         params.push_back(param);
      }
      
      idx_lb = in.find("[", idx_rb);
      idx_rb = in.find("]", idx_lb);
      location = in.substr(idx_lb+1, idx_rb-1-idx_lb);
   }
   
   bool operator != (const MHReduction& rv) {
      if (isComment && rv.isComment) {
         return true;
      }
      return (functionName != rv.functionName
              || location != rv.location
              || prefix != rv.prefix);
   }
   
   void cleanParams() {
      unordered_map<string, void*> um;
      for (auto e: params) {
         if (CPPUtils::strEndsWith(e, ", ")) {
            e = e.substr(0, e.size()-2);
         }
         um[e] = nullptr;
      }
      params.clear();
      for (auto p: um) {
         params.push_back(p.first);
      }
   }
   
   string getString() {
      stringstream ss;
      
      if (isComment) {
         ss << commentLine << "\n";
         return ss.str();
      }
      
      ss << functionName + " [" + to_string(countOccurence) + "] [" + prefix + "]";
      
      cleanParams();
      if (params.size() == 0 || params.size() > 1) {
         ss << " []";
      }
      else if (params.size() == 1) {
         ss << " [" << params[0] << "]";
      }
      ss << " [" + location + "]";
      
      int sc = 0;
      while (sc < functionName.size() && functionName[sc] == ' ') {
         sc++;
      }
      string spaces(sc, ' ');
      bool didEmitNL = false;
      if (params.size() > 1) {
         ss << "\n";
         for (auto i=0; i < params.size(); i++) {
            ss << spaces << to_string(i) << ": [" << params[i] << "]\n";
         }
         didEmitNL = true;
      }
      if (!didEmitNL) {
         ss << "\n";
      }
      return ss.str();
   }
};

int reduceRepetitiveBlocks() {
   cout << "Reading rows from file\n";
   istringstream iss(fin);
   vector<string> v = CPPUtils::getLines(iss);
   iss.str("");
   iss.clear();
   cout << "...Done\n";
   cout << "Creating data structure\n";
   vector<MHReduction> vr;
   for (auto i=0; i < v.size(); i++) {
      auto e = v[i];
      string s = CPPUtils::strTrim(e);
      if (s.size() > 0) {
         vr.push_back(MHReduction(e));
      }
   }
   unsigned long countOriginalLines = vr.size();
   cout << "...Done\n";
   v.clear();
   
   cout << "Total number of lines in file: " << countOriginalLines << "\n";
   cout << "Processing reductions\n";
   int countReduction = 0;
   int countReducedLines = 0;
   for (int i=1; i < 100; i++) {
      vr = modeReduce(i, countReduction, countReducedLines, vr);
      cout << "...Did: " << i << ", countReduction: " << countReduction << ", countReducedLines: " << countReducedLines << "\n";
   }
   //    vr = modeReduce(1, countReduction, countReducedLines, vr);
   cout << "...Done\n";
   cout << "Reductions: " << countReduction << endl;
   cout << "Calls eliminated: " << countReducedLines << ", from original: "  << countOriginalLines << "\n";
   
   stringstream ss;
   for (auto v: vr) {
      ss << v.getString();
   }
   string ss_str = ss.str();
   ss.str("");
   //    cout << ss_str;
   CPPUtils::fsFileWrite("/tmp/python_start_reduced.txt", ss_str);
   return 0;
}

//-----------------------------------------------------------------------------
vector<MHReduction> modeReduce(int windowSize,
                               int& countReductions,
                               int& countReducedLines,
                               const vector<MHReduction>& vIn2) {
   vector<MHReduction> w;
   vector<MHReduction> vIn = vIn2;
   vector<MHReduction> vOut;
   
   int idx_v = 0;
   int matchHappenedCount = 0;
   while (true) {
      // Cleanup if we don't have enough data left for a window(*2) pass
      unsigned long needed = windowSize-w.size() + windowSize + idx_v;
      unsigned long available = vIn.size();
      if (needed > available) {
         // Commit window that potentially has reductions
         for (auto e: w) {
            vOut.push_back(e);
         }
         // Commit leftovers in v
         while (idx_v < vIn.size()) {
            auto e = vIn[idx_v];
            vOut.push_back(e);
            idx_v++;
         }
         break;
      }
      
      // We have data to do a window pass
      // Fill window
      // TODO: Fails if run out of data due to comments
      while (w.size() < windowSize) {
         auto mr = vIn[idx_v];
         w.push_back(mr);
         idx_v++;
      }
      
      // Detect if window matches
      bool matchNow = true;
      vector<MHReduction> testersAgainstW;
      // TODO: Fails if run out of data due to comments
      int idx_0 = 0;
      while (testersAgainstW.size() < windowSize) {
         auto mr = vIn[idx_0+idx_v];
         testersAgainstW.push_back(mr);
         idx_0++;
      }
      for (auto i=0; i < windowSize; i++) {
         MHReduction mrw = w[i];
         MHReduction mrv = testersAgainstW[i];
         if (mrw != mrv) {
            matchNow = false;
            break;
         }
      }
      
      // No match
      if (!matchNow) {
         // If this window never matched, then consume and move forward 1
         if (matchHappenedCount == 0) {
            vOut.push_back(w[0]);
            w.erase(w.begin());
         }
         // Window has matched at least once, then cancel this window and move to next mismatch
         else {
            for (auto e: w) {
               vOut.push_back(e);
            }
            w.clear();
            matchHappenedCount = 0;
         }
      }
      // Match, consume window and retain it
      // We only commit window to out (above) when we have encounter first mismatch or no data left
      else {
         matchHappenedCount++;
         countReductions++;
         countReducedLines += w.size();
         idx_v += windowSize;
         if (w.size() != testersAgainstW.size()) {
            cout << "*** Error size mismatch: " << fileName << endl;
         }
         assert(w.size() == testersAgainstW.size());
         // Merge into window object
         for (auto i=0; i < w.size(); i++) {
            w[i].countOccurence += testersAgainstW[i].countOccurence;
            for (auto p: testersAgainstW[i].params) {
               w[i].params.push_back(p);
            }
         }
      }
   }
   return vOut;
}
