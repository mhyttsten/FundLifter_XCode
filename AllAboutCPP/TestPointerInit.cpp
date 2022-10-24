//
//  TestPointerInit.cpp
//  FundLifter
//
//  Created by Magnus Hyttsten on 10/11/22.
//  Copyright © 2022 Magnus Hyttsten. All rights reserved.
//

#include <stdio.h>
#include <iostream>

class Base {
public:
   virtual void run() { std::cout << "Base" << std::endl; }
};
class Derived : public Base {
public:
   void run() { std::cout << "Derived" << std::endl; }
};

int main(int argc, char* argv[]) {
   int a{10};
   Base* client0{new Derived()};
   Base* client1 = new Derived();
   client0->run();
   client1->run();
}
