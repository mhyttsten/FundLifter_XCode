#include <array>
#include <iostream>
#include <vector>

using namespace std;


template <class T>
ostream& operator << (ostream& o, const vector<T>& v) {
    for (auto i=v.cbegin(); i != v.cend(); i++) {
        o << *i;
        if (i+1 != v.end()) {
            o << ", ";
        }
    }
    return o;
}

struct Hello {
    string a, b;
};

static auto ccid = 0;
class CCTest {
public:
   int _ccid;
   CCTest() { _ccid = ccid; cout << "CCTest::CCTest()[" << to_string(_ccid) << "]" << endl; ccid++; }
   CCTest(const CCTest& rvalue) { cout << "CCTest::CCTest(const CCTest&[" << rvalue._ccid << "])" << endl; }
};


class Move {
public:
   Move()  { cout << "DC" << endl; }
   Move(const Move& rv)   { cout << "CC" << endl; }
   Move(Move&& rv)        { cout << "MC" << endl; }
   Move& operator = (Move&& rv) { cout << "MA" << endl; return *this; }
   Move& operator = (Move& rv)  { cout << "CA" << endl; return *this; }
   int _m;
};

Move foo(int&& a) {
   int* b = &a;
   Move m;
   m._m = 30;
   return m;
}

//int main(int argc, char* argv[]) {
//    Hello h { "a", "b" };
//    vector<char> v01(2, 'c');
//    cout << "hello: " << v01 << endl;

//   int ur00 = 10;
//   auto&& ur01 = ur00;
//   int&& u4 = 10;
//   u4 = 20;
//   ur01 = 30;
   
//   int t01;
//   foo(t01);
//   Move m1 = foo();
//   Move m2 = move(m1);
//   int a=10;
//   int& b = a;
//   auto&&c = b;
//}

