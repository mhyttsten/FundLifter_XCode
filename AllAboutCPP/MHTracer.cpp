//
//  MHTracer.cpp
//  AllAboutCPP
//
//  Created by Magnus Hyttsten on 1/30/21.
//  Copyright © 2021 Magnus Hyttsten. All rights reserved.
//

#include "MHTracer.h"

CPPIndentWriter MHTracer::_iw;

MHTracer::MHTracer(string fileName, int lineNumber, string functionName) {
  _functionName = functionName;
  _iw.println("Enter: " + functionName + " [" + to_string(lineNumber) + "@" + fileName);
  _iw.push();
}

MHTracer::~MHTracer() {
  _iw.pop();
  _iw.println("Exit: " + _functionName);
}
