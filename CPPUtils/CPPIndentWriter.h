//
//  CPPIndentWriter.hpp
//  AllAboutCPP
//
//  Created by Magnus Hyttsten on 10/11/20.
//  Copyright © 2020 Magnus Hyttsten. All rights reserved.
//

#ifndef CPPIndentWriter_hpp
#define CPPIndentWriter_hpp

#include <stdio.h>
#include <string>

class CPPIndentWriter {
public:
  void push();
  void pop();
  void println(std::string);
  void flush();
  std::string getString() { return _str; }
private:
  int _ic = 0;
  std::string _str = "";
};

#endif /* CPPIndentWriter_hpp */
