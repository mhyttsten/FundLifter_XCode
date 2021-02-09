//
//  MHTracer.hpp
//  AllAboutCPP
//
//  Created by Magnus Hyttsten on 1/30/21.
//  Copyright © 2021 Magnus Hyttsten. All rights reserved.
//

#ifndef MHTracer_hpp
#define MHTracer_hpp

#include <stdio.h>
#include "CPPIndentWriter.h"

using namespace std;

class MHTracer {
  static CPPIndentWriter _iw;
  string _functionName;
public:
  MHTracer(string fileName, int lineNumber, string functionName);
  ~MHTracer();
};

#endif /* MHTracer_hpp */
