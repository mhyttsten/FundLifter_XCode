//
//  MMFileDateEntity.cpp
//  MediaManager
//
//  Created by Magnus Hyttsten on 12/25/21.
//  Copyright © 2021 Magnus Hyttsten. All rights reserved.
//

#include "MMFileDateEntity.h"

string MMTMWrapper::getString() const {
   if (!_isValid)
      return "******NOT SET******";
   string t = CPPUtils::date_tm_To_YYYYcolMMcolDDspcHHcolMMcolSS(_tm);
   return t;
}
