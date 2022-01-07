//
//  MMFileDateEntity.hpp
//  MediaManager
//
//  Created by Magnus Hyttsten on 12/25/21.
//  Copyright © 2021 Magnus Hyttsten. All rights reserved.
//

#ifndef MMFileDateEntity_hpp
#define MMFileDateEntity_hpp

#include "CPPUtils.h"

#include <stdio.h>
#include <ctime>

using namespace std;

class MMTMWrapper {
public:
   MMTMWrapper(const tm& e) { _isValid = true; _tm = e; }
   MMTMWrapper() { _isValid = false; }
   void setTM(const tm& e)  { _isValid = true; _tm = e;}
   
   bool operator < (const MMTMWrapper& r) const {
      if (!_isValid && !r._isValid) {
         return false;
      }
      if (_isValid && !r._isValid) {
         return true;
      }
      if (!_isValid && r._isValid) {
         return false;
      }
      
      if (_tm.tm_year < r._tm.tm_year) {
         return true;
      }
      else if (_tm.tm_year > r._tm.tm_year) {
         return false;
      }

      if (_tm.tm_mon < r._tm.tm_mon) {
         return true;
      }
      else if (_tm.tm_mon > r._tm.tm_mon) {
         return false;
      }

      if (_tm.tm_mday < r._tm.tm_mday) {
         return true;
      }
      else if (_tm.tm_mday > r._tm.tm_mday) {
         return false;
      }

      if (_tm.tm_hour < r._tm.tm_hour) {
         return true;
      }
      else if (_tm.tm_hour > r._tm.tm_hour) {
         return false;
      }

      if (_tm.tm_min < r._tm.tm_min) {
         return true;
      }
      else if (_tm.tm_min > r._tm.tm_min) {
         return false;
      }

      if (_tm.tm_sec < r._tm.tm_sec) {
         return true;
      }
      else if (_tm.tm_sec > r._tm.tm_sec) {
         return false;
      }

      return false;
   }

   bool operator == (const MMTMWrapper& r) const {
      if (_isValid != r._isValid)
         return false;
      if (!_isValid && !r._isValid)
         return true;
      
      return
      _tm.tm_year == r._tm.tm_year &&
      _tm.tm_mon == r._tm.tm_mon &&
      _tm.tm_mday == r._tm.tm_mday &&
      _tm.tm_hour == r._tm.tm_hour &&
      _tm.tm_min == r._tm.tm_min &&
      _tm.tm_sec == r._tm.tm_sec &&
      _tm.tm_isdst == r._tm.tm_isdst;
   }

   string getString() const;

   tm _tm;
   bool _isValid = false;
};

class MMFileDateEntity {
   
public:
   void setDateOriginal(const MMTMWrapper& a) { _dateOriginal = a; }
   void setDateCreate(const MMTMWrapper& a) { _dateCreate = a; }
   void setDateModify(const MMTMWrapper& a) { _dateModify = a; }
   void setFileName(const string& s)  { _fileName = s; }

   string _fileName;
   MMTMWrapper _dateOriginal;
   MMTMWrapper _dateCreate;
   MMTMWrapper _dateModify;
   
   bool areAllSet() {
      bool c1 = _dateOriginal._isValid && _dateCreate._isValid && _dateModify._isValid;
      return c1;
   }
   
   bool isAnySet() {
      bool c1 = _dateOriginal._isValid || _dateCreate._isValid || _dateModify._isValid;
      return c1;
   }
   
   bool areAllTheSame() {
      bool c2 = _dateOriginal == _dateCreate && _dateOriginal ==  _dateModify;
      return c2;
   }
      
   MMTMWrapper getEarliest() {
      MMTMWrapper earliest;
      if (_dateOriginal._isValid) {
         earliest = _dateOriginal;
      }
      if (_dateCreate._isValid) {
         if (!earliest._isValid || _dateCreate < earliest) {
            earliest = _dateCreate;
         }
      }
      if (_dateModify._isValid) {
         if (!earliest._isValid || _dateModify < earliest) {
            earliest = _dateModify;
         }
      }
      return earliest;
   }
   
   MMTMWrapper getLatest() {
      MMTMWrapper latest;
      if (_dateOriginal._isValid) {
         latest = _dateOriginal;
      }
      if (_dateCreate._isValid) {
         if (!latest._isValid || latest < _dateCreate) {
            latest = _dateCreate;
         }
      }
      if (_dateModify._isValid) {
         if (!latest._isValid || latest < _dateModify) {
            latest = _dateModify;
         }
      }
      return latest;
   }   
};


#endif /* MMFileDateEntity_hpp */
