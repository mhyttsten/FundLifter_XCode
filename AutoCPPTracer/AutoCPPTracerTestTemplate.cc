
#include <iostream>

using namespace std;

class Foo {
public:
   void foo1();
   void foo2();
   void foo3();
   void foo4();
};

int main(int argc, char* argv[]) {
   Foo foo;

   cout << "==-----" << endl << "Round 1" << endl;
   foo.foo1();
   cout << "==-----" << endl << "Round 2" << endl;
   foo.foo1();
   cout << "==-----" << endl << "Round 3" << endl;
   foo.foo1();
   cout << "==-----" << endl << "Round 4" << endl;
   foo.foo1();
}

void Foo::foo1() {

   cout << "Foo:foo1() ENTRY" << endl;
   foo2();
   cout << "Foo:foo1() EXIT" << endl;
}

void Foo::foo2() {

   cout << "Foo:foo2() ENTRY" << endl;
   foo3();
   cout << "Foo:foo2() EXIT" << endl;
}

void Foo::foo3() {

   cout << "Foo:foo3() ENTRY" << endl;
   foo4();
   cout << "Foo:foo3() EXIT" << endl;
}

void Foo::foo4() {

   cout << "Foo:foo4() ENTRY" << endl;
   cout << "Foo:foo4() EXIT" << endl;
}


