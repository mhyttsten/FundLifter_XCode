

#include <stdio.h>
#include <iostream>
#include "MHTraceMainTest.h"

void foo1();
void foo2();
void foo3();
int main(int argc, char* argv[]) {
  foo1();
  fooh1();
}

void foo1() {
  foo2();
}
void foo2() {
  foo3();
}
void foo3() {
  std::cout << "Hello from foo3\n";
}


/*
#include <stdio.h>
#include <iostream>

void foo1();
void foo2();
void foo3();
int main(int argc, char* argv[]) {
  foo1();
}

void foo1() {
  foo2();
}
void foo2() {
  foo3();
}
void foo3() {
  std::cout << "Hello from foo3\n";
}
*/
