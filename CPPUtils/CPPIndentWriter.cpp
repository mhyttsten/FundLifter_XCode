//
//  CPPIndentWriter.cpp
//  AllAboutCPP
//
//  Created by Magnus Hyttsten on 10/11/20.
//  Copyright © 2020 Magnus Hyttsten. All rights reserved.
//

#include "CPPIndentWriter.h"
#include <iostream>

using namespace std;

void CPPIndentWriter::push() {
  _ic += 3;
}

void CPPIndentWriter::pop() {
  assert(_ic >= 3);
  _ic -= 3;
}

void CPPIndentWriter::println(std::string str) {
  string pad = "";
  pad.assign(_ic, ' ');
  _str += pad + str + '\n';
}

void CPPIndentWriter::flush() {
  std::cout << _str;
  _str.clear();
}
