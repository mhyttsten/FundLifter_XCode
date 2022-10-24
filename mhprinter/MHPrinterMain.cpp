//
//  MHPrinterMain.cpp
//  AllAboutCPP
//
//  Created by Magnus Hyttsten on 5/24/22.
//  Copyright © 2022 Magnus Hyttsten. All rights reserved.
//

#include <stdio.h>
#include <iostream>


void foo();
void bar();
int main(int argc, char* argv[]) {
   cout << "Hello rom MHPrinterMain.cc" << endl;
   return 0;
}

void foo() {
   cout << "foo" << endl;
   bar();
}

void bar() {
   cout << "bar" << endl;
}
