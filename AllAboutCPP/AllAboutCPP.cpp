//
//  AllAboutCPP.cpp
//  FundLifter
//
//  Created by Magnus Hyttsten on 8/10/20.
//  Copyright © 2020 Magnus Hyttsten. All rights reserved.
//

#include <cstring>
#include <chrono>
#include <complex>
#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>  // Needed for std::cout etc
#include <istream>
#include <iterator>
#include <list>
#include <map>
#include <numeric>
#include <queue>
#include <random>
#include <ostream>
#include <sstream>
#include <regex>
#include <stdio.h>
#include <thread>
#include <time.h>
#include <unordered_map>
#include <vector>
#include <variant>

#include "CPPIndentWriter.h"

int main(int argc, char* argv[]) {
  using namespace std;
//  std::pair p = std::make_pair("hello", 20);
//  auto a = std::get<0>(p);
//  cout << "a: " << p.first << endl;
//
//  void chapter01(); chapter01();
//  void chapter02(); chapter02();
//  void chapter03(); chapter03();
//  void chapter04(); chapter04();
//  void chapter05(); chapter05();
//  void chapter06(); chapter06();
//  void chapter07(); chapter07();
//  void chapter08(); chapter08();
//  void chapter09(); chapter09();
//  void chapter10(); chapter10();
//  void chapter11(); chapter11();
//  void chapter12(); chapter12();
//  void chapter13(); chapter13();
//  void chapter14(); chapter14();
//  void chapter15(); chapter15();
//
//  void utils(); utils();
  void coding_problems(); coding_problems();
}

// ***************** Chapter 01
void chapter01() {
  std::cout << "*** Starting Chapter 1: The Basics" << std::endl;
  using namespace std;
  
  // Built in types: int, bool (true, false), char, double
  
  int i0 = 3;
  auto i1 = 3;  // Creates int
  int i2 = 3.14;  // Implicitly truncates 3.14
  // int i3 = { 3.14 };  // Cannot be narrows to int in initializer list
  // int i4 { 3.14 };    // Cannot be narrows to int in initializer list
  char c1 = { 3 };
  int i5 { 3 };
  int i6 { '3' };
  std::string s1 {"hello"};
  
  // Requires #include <vector>
  std::vector<int> v1;
  std::vector<int> v2 { 1, 2, 3 };
  
  class Empty { public: Empty() { std::cout << "Empty.Empty called" << std::endl; } };
  Empty e1 = {};  // Calls default constructor
  Empty e2 {};
  
  // Scopes: Local (function + params, lambda), Class, Namespace, otherwise Global
  
  constexpr int i21 = 10;  // Must be calculated at compile time
  const int i22 = 11;  // Can be calculated at run-time
  // i22 = 12;  // But cannot be changed of course
  
  // You can declare constexpr functions and use them to initialize constexpr variables
  // These can be quite complex (i.e. taking arguments), but there are restrictions
  // TODO: You cannot nest function definitions? This gives error
  // constexpr int i23() { return 1; }
  
  char c31[10];  // Array of
  char c310 = 0;  // Ok. We could do std::cin >> c310; if (c310 == 'y') { ... }
  char c320[]  = "Hello";
  char* c32  = c320;  // Pointer to
  char* c33 = &c31[4];
  // Ok, because char[] i C-style 0 terminated strings
  for (; *c32 != 0; ++c32)  { std::cout << *c32; }  std::cout << std::endl;
  
  // i34 is incremented after block execution (even if ++ appears before i34)
  for (auto i34=0; i34 < 10; ++i34) { std::cout << i34; }  std::cout << std::endl;
  
  // Reference to
  int c35[] { 1, 2 };
  for (auto& c36: c35) { c36++; ++c36; std::cout << c36; }  std::cout << std::endl;
  
  // For pointers, use nullptr and not NULL or 0
  int* c37 = nullptr;
  
  // As in 'for', we can declare local variables in 'if' scope
  if (auto c38=c35[0]; c38 != 0) { std::cout << "It was not 0" << std::endl; }
  if (auto c38=c35[0]) { std::cout << "It was not 0 or nullptr" << std::endl; }
  
  int i40x = 0;
  int i40y = 1;
  int& i40rx = i40x;
  int& i40ry = i40y;
  i40rx = i40ry;  // Assign new value to x, i.e. we don't change the reference itself!
  std::cout << "x is: " << i40x << std::endl;
  // Also, references must be initialized (very good!)
  
  // Use initialization instead of assignment whenever possible
  // Avoid ALL_CAPS names
  // Use {} initializer
  // Use unsigned for bit manipulations only
  // Use nullptr instead of 0 or NULL
}

// ***************** Chapter 02
void chapter02() {
  std::cout << "*** Starting Chapter 2: User-Defined types" << std::endl;
  using namespace std;
  
  class A {
  public:
    A(int a): _a{a} {}  // Use member initialization instead of assigning in constructor
    int& operator[](int index) { return _a; }
    int _a;
  };
  A a(5); a[0] = 10; cout << "Value of _a: " << a._a << endl;
  
  // U is a point to memory, refer to that memory through one of these members
  union U {
    int size;
    char string[];
  };
  
  // Don't use unions though, use variant, need #include <variant>
   std::variant<int, float> v;  // v can hold either an int or float
   int i01 = std::get<int>(v);
   int i02 = std::get<0>(v);  // Same as get<int>
   assert(std::holds_alternative<int>(v));
   try { std::get<float>(v); }
   catch (const std::bad_variant_access&) { }
  
  enum class Binary { zero, one };
  Binary b0 = Binary::zero;
  Binary b1 = Binary::one;
  cout << "b0 < b1? " << (b0 < b1) << endl;
  cout << "b0 > b1? " << (b0 > b1) << endl;
  cout << "b0 == 0? " << (b0 == Binary::zero ? "true": "false") << endl;
  // TODO: Book says you can overloat operators for enum classes, but doesn't work
  // Binary and ++ needs to be moved to global scope to compile
  //   enum class Binary { zero, one };
  //   Binary& operator++(Binary& a) { return a = (a == Binary::zero ? Binary::one: Binary::zero ); }
  // b0++;  // Gives compiler error
  
  // Classical enum, should not be used but common in code
  // First element gets int == 0
  enum Color { red, blue, green };
  cout << "Value of red: " << red << endl;
  
  // A struct == class but with default public members
  // Always use 'enum class' (not naked 'enum' enums). Overload operations
  // Use 'enum class' for named constants
}

// ***************** Chapter 03
void chapter03() {
  std::cout << "*** Starting Chapter 3: Modularity" << std::endl;
  using namespace std;
  
  // C++ represents interfaces by declaration
  
  // Modules: C++20 introduces modules to overcome #include "Vector.h" and implementation in Vector.cpp
  // One reason is compilation performance: Vector.h is parsed many times
  // In Vector.cpp
  //    module;  // We will define a module in this file
  //    export module Vector;  // The module will be Vector
  //    export class Vector { ... };  // All vector members will be exported
  //    export int foo();  // Function will be exported
  // In a file using Vector
  //    import Vector;
  //    // ...
  
  // Namespaces: Can only be defined in global or namespace scope
  // namespace MyCode {
  //    // Classes, functions, etc, etc
  // }
  //
  using namespace std;  // Get access to all symbols of std
  // using std::swap;  // Get access to swap of std
  
  // Exceptions
  // Exception handling is not slow (this is a myth), but should only be used in case of errors
  
  class Emtpy { };
  try {
    Emtpy e;
  }
  catch(std::out_of_range& e) { cout << e.what() << endl; }
  catch(std::length_error& e) { throw; }  // Rethrow
  catch(std::bad_alloc& e) { std::terminate(); exit(1); }  // New failed, immediately terminate program
  // Find lots of exceptions in #include <stdexcept>
  // RAII: Resource Aquisition Is Initialization
  // == A constructor shold aquire all resources it needs, a destructor should release them
  //    Constructor should establish class invariants, so members can assume them
  //    'try' blocks are rare in well-designed code, use RAII to reduce them
  
  // If foo throws, then std::terminate will be called
  //    int foo() noexcept { ... }
  
  // C++20: A contract mechanism (invariants, preconditions) etc has been proposed
  //    Throws in development, disabled at production
  
  // TODO: Does assert throw at run-time? How does C++ know the difference?
  assert(true);  // Ok we will not throw
  static_assert(true, "Error");  // No message: Default is source location
  
  // const and default argument value (optional to provide)
  class Empty2 { int sum(const string& a="") const; };  // Example of const, declare things multiple times
  vector<int> v = { 1, 2, 3};
  for (const auto& i: v)  { }
  auto sum();  // Deduce return type (maybe use in definition but not declaration, possible?)
  
  // Pass by value if it has size of 2-3 pointers, otherwise by reference
  // Never return pointers, return objects. If corpus is large, provide move constructor/assignment
  struct Huge {
    Huge() {}
    Huge(const Huge& rv); // Copy by value constructor
    Huge& operator=(const Huge& rv);  // Copy by value assignment, TODO: Scary to return reference?
    Huge(Huge&& rv) {}   // Move constructor (and nullify rv)
    Huge& operator=(Huge&& rv);  // Move assignment (and nullify rv)
  };
  // You can also use std::move(rv) to ensure move semantics
  Huge h1;
  Huge h2 = std::move(h1);  // Definition (above) needed for Huge(Huge&&)
  
  // Structured binding
  struct Pair {
    string name; int age;
    static Pair doit() {
      // Elementwise {} is ok == call constructor with those arguments
      // If no constructor is specified, automatic ones are created (only works for public members)
      // If any constructor is specified { } must match one of them (no automatic ones created)
      if (false) { return { "Clark Kent", 42 }; }
      else if (false) { return { "Clark Kent" }; }
      return Pair{};
    }
    void foo(map<string, int>& m) {
      auto [n,a] = Pair::doit();  // Use structured-binding as returns
      for (auto& [k,v]: m) { v++; }  // Increment value of every key
    }
  };
  
  // Avoid non-inline definitions in headers, but do inline simple methods
  // Prefer modules over headers
  // Don't put 'using' namespace specifier in headers files
  // Use exceptions for error handling only
}

// ***************** Chapter 04
void chapter04() {
  std::cout << "*** Starting Chapter 4: Classes" << std::endl;
  using namespace std;
  
  // Automatic inlining: Function definition at class declaration
  // Use 'inline' keywork at declaration to inline functions defined elsewhere
  
  // You can't redefine operators for built-in types
  // You cannot change syntax i.e. create a unary '/' operator
  // Overload operators at class declaration, or
  class A {};
  A operator+(A v1, A v2);
  
  double* ds1 = new double[10];
  delete[] ds1;  // Use delete[] to delete an array
  double* ds2 = new double{10};
  delete ds2;  // Use delete to delete an object
  
  // Do RAII: Limit usage of new/delete to constructor/destructor
  
  struct A2 {
    int _a2 = 0;
    A2(std::initializer_list<double> a) {}
    static void foo1(istream& is) {  // Read doubles until end-of-file or formatting error
      for (double d; is>>d ;)
        cout << d << endl;
    }
  };
  A2 a2 = { 3.14, 2.72 };  // Enabled by std::initializer_list (known to compiler)
  
  // TODO: Why static_cast?
  unsigned int ui1 = 1000;
  auto ui2 = static_cast<double>(ui1);  // Tries to use sensible conversion
  // double ui3 = reinterpret_cast<double>(ui1);  // Treat as sequence of bytes, TODO: ???
  double ui3 = (double)ui1;  // Don't use
  const int ui4 = 10;
  const A2* a20 = new A2 { 3.14, 2.71 };
  const_cast<A2*>(a20)->_a2 = 20;  // Get rid of const'ness
  
  // Copy an initializer list into pointer memory
  std::initializer_list<double> l1 { 1.1, 2.2 };
  double* l2 = new double[2];
  std::copy(l1.begin(), l1.end(), l2);
  
  class Base {
  public:
    virtual void foo() = 0;
  };
  class Derived: public Base {
  public:
    virtual void foo() override { }  // 'override' is optional but allows compiler to catch spelling errors etc
  };
  Base* base = new Derived{};
  if (Derived* derived = dynamic_cast<Derived*>(base)) {
    cout << "Base is a reference to Derived" << endl;
  }
  
  // Object is automatically destructed when unique_ptr goes out of scope
  unique_ptr<Derived> up = unique_ptr<Derived>(new Derived());
  
  std::list<double> ld { 3.14, 2.72 };
  
  // Avoid naked pointers, use unique_ptr / shared_ptr
  // If a class is a container, give it and std::initializer_list constructor
  // A class with a virtual method should have a virtual destructor
  // Use override to make it explicit
  // Use unique_ptr and shared_ptr to avoid forgetting delete
}

// ***************** Chapter 05
using namespace std;
struct C05String {
  C05String() { cout <<  "C05String: Default constructor\n"; }
  C05String(const char* str) {
    cout << "C05String: const char* constructor\n";
    _size = strlen(str);
    _data = new char[_size];
    memcpy(_data, str, _size);
  }
  C05String(const C05String& str) {
    cout << "C05String: Copy constructor\n";
    _size = std::strlen(str._data);
    _data = new char[_size];
    memcpy(_data, str._data, _size);
  }
  C05String(C05String&& rv) {
    cout << "C05String: RV constructor\n";
    _size = rv._size;
    _data = rv._data;
    rv._data = nullptr;
  }
  ~C05String() { cout << "C05String:Destructor\n";  delete _data; }
  void print() { for (int i=0; i < _size; i++) cout << _data[i]; cout << '\n'; }
  char* _data = nullptr;
  size_t _size;
};
struct C05Entity {
  C05Entity(const C05String& s) : _name(s) { cout << "C05Entity: CC\n"; }
//  C05Entity(C05String&& rv): _name((C05String&&)rv)     { cout << "C05Entity: RV C\n"; }
  C05Entity(C05String&& rv): _name(move(rv))     { cout << "C05Entity: RV C\n"; }
  C05String _name;
  void print() { _name.print(); }
};
void chapter05() {
  std::cout << "\n*** Starting Chapter 5: Essential Operations" << std::endl;
  using namespace std;
  
  cout << "Testing the Cherno thing\n";
  C05String c05str("Cherno");
  cout << "...1\n";
  C05Entity ch00(C05String("Cherno"));
  cout << "...2\n";
  C05Entity ch01(c05str);
  cout << "\n";
  
  C05String c05str2 = move(c05str);
  
  // Rules
  // - An argument by-value, uses an rvalue (temporary) object directly but copy constructor if lvalue (Xcode)
  // std::move at compile time it figures out its argument type
  //    move takes an existing variable and makes it a tmp one
  // delete[] array_type;
  // Assignment operator: Delete your prev resources, if rvalue is this then no-op, return SelfType& (*this)
  
  // For a class with a pointer member, you probably want to defined copy and move operations
  // Such non-trivial class probably needs the below
  // Compiler will generate these as needed (if they are not defined)
  // 'default': Use auto-generated operation
  //            Specifying this for one supresses auto-generation of the others (unless you specify default for them)
  // 'delete':  Operation should not be auto-generated
  // If you are explicit about some defaults, other defaults will not be automatically generated
  class X {
  public:
    X();     // Default
    X(const X&) = delete;  // Copy
    X(X&&);  // Move
    X& operator=(const X&) = default;  // Copy assignment
    X& operator=(const X&&); // Move assignment
    ~X();
  };
  // 'Rule of zero' - either define all of these or accept defaults for all
  
  // When are all these called
  class X2 {
  public:
    int _a;
    X2() { cout << "DConstructor" << endl; }
    X2(const X2& a) { cout << "CConstructor" << endl; }
    X2(X2&& a)  { cout << "MConstructor" << endl; }
    X2& operator=(const X2& a)  { cout << "CAssignment" << endl; return const_cast<X2&>(a); }
    X2& operator=(const X2&& a) { cout << "MAssignment" << endl; return const_cast<X2&>(a); }
    ~X2() { cout << "Destructor" << endl; }
    
    static X2 foo1(X2 x) {
      cout << "foo1 entry" << endl;
      // Return causes MC
      X2 x2 = x;
      x2 = x;
      cout << "foo1 exit" << endl;
      return x;
    }
  };
  cout << "Now doing a series of tests\n";
  X2 x20 = X2();
  cout << endl << "Test 1" << endl;
  X2 x21 = X2::foo1(x20);
  cout << endl << "Test 2" << endl;
  x21 = X2::foo1(x20);
  cout << "\nTest 21\n";
  x21 = X2::foo1(X2());
  
  // Conclusion: Move C/A is called when a stack object is returned by value
  // C or A depends on if caller initializes new object or assigns to old one
  cout << endl << "Test 3" << endl;
  X2 x22 = std::move(x21);  // MConstructor
  x22 = std::move(x21);  // MAssignment
  
  // Single arg constructors create implicit conversion, unless explicit is specified
  class X3 {
  public:
    explicit X3(int a) {}
  };
  X3 x30(7);   // Calling the constructor
  // X3 x31 = 7;  // Cannot implicitly create from int because of explicit keyword above
  
  // p is an iterator pointing to elements of the vector
  // .begin() points to first element, .end() points one-beyond last element
  // You can iterate using ++ on the pointer
  vector<int> v40 { 3, 2, 1 };
  vector<int> v41 = v40;
  sort(v40.begin(), v40.end());
  for (auto p = v40.begin(); p!=v40.end(); ++p) { cout << *p; }
  cout << endl;
  for (auto& x: v41) { x = 0; }  // v41.begin() and .end() implicitly
  for (auto p = v41.begin(); p!=v41.end(); ++p) { cout << *p; }
  cout << endl;
  
  int i0 = 123;  // int literal
  chrono::seconds i1 = 123s; // seconds
  unsigned int ui1 = 0xFF00u;  // unsigned int literal
  double d1 = 3.14;  // double literal
  const char cc[] = "Surprise!";
  std::string s0 = "Surprise!"s;
  std::string_view s1 = "Surprise!"sv;
  // std::complex c1 = 12.7i;  il, if
  // Literal operators: constexpr <T0> operator""<c>(<T1>)  // c is the literal character
  
  // Other functions a type can define
  // - swap(): Used by many algorithm (e.g sort). tmp=a, a=b, b=tmp)
  // - hash<X>: Must be defined if X is used as key in a hash table
  
  // C++ offers a pluggable GC interface, but preferrably smart pointers should be used instead
  // Define binary operators outside class scope (to give identical treatment)
  // Members are not initialized to any default values, you should provide them: int a = 10;
}

// ***************** Chapter 06
// class and typename are the same
template<class T, int N> class C06_X0 {};   // class is old style
template<typename T, int N> class C06_X1 {  // typename is new style (semantics equivalent to class)
  using value_type = T;  // Give user access to template type
  constexpr int size() { return N; }  // N is known at compile-time
  void foo(T* a);
};
template<typename T, int N> void C06_X1<T,N>::foo(T* a)  { }  // Define a function out-of-line
// N allows us to e.g create static memory buffers since it's @ compile-time

template<typename T> struct C06_X3 {
  using value_type = T;
  C06_X3(initializer_list<T>) { };
  template<typename Iter> C06_X3(Iter, Iter) { }
  static void foo() {
    C06_X3 v1 = { 1, 2, 3};
    C06_X3 v2(v1.begin(), v1.begin()+2);  // TODO: Why does v1 support begin()? Also book says this shouldn't work without a deduction guide
  }
};
// 'Deduction guide' book says is needed for foo to compile, but strangely it works without it
template<typename Iter> C06_X3(Iter,Iter) -> C06_X3<typename Iter::value_type>;

// Function Templates
// Can be member functions, but not virtual since compiler cannot create vtbl for it
template<typename Sequence, typename Value> Value sum(const Sequence& s, Value v) {
  for (auto e: s) { v += s; }
  return v;
}

// Function Objects (functors): Define objects that can be called like functions
template<typename T>
class LessThan {
  const T _v;
public:
  LessThan(const T& v): _v(v)  { }
  bool operator()(const T& a) { return a < _v; }
};

// Function template we use to describe lambdas below
template<typename C, typename Operation>
void c06_forAll(C& c, Operation o) {
  for (auto& e: c) {
    o(e);
  }
}

template<typename Value>
using String_map = std::map<std::string, Value>;

// Variable template
template<typename T>
const std::vector<T> acceleration = { T{}, T{-9.8}, T{} };

void chapter06() {
  std::cout << "\n*** Starting Chapter 6" << std::endl;
  using namespace std;
  
  pair<int,double> p0 { 1.5,2 };
  pair p1 = { 1.5, 2 };
  auto p2 = make_pair(1, 2.5);  // p2 == pair<int,double>
  vector v0 = { 0, 1 };
  vector v1 = { "a", "b" };   // vector<const char*>
  vector v2 = { "a"s, "b"s }; // vector<string>
  
  // A functor, Function Object. Very often used as input to algorithms
  LessThan lt(42);
  std::cout << "30 lt? " << lt(30) << std::endl;
  // LessThan::foo();
  
  // Lambda
  int i20 = 10;
  int i21 = 20;
  vector vi = {0, 10, 20};
  c06_forAll(vi, [&](int& a){ a += i20; i20++; });  // Bind all as reference
  cout << "Test 1: i20: " << i20 << endl;
  for (auto e: vi) { cout << "   " << e << ", "; }  cout << endl;
  
  c06_forAll(vi, [=](int& a){ a += i20; });  // Bind all as value -> we cannot modify i20
  cout << "Test 2: i20: " << i20 << endl;
  for (auto e: vi) { cout << "   " << e << ", "; }  cout << endl;
  
  c06_forAll(vi, [&i20=i21](auto& a){ a += i20; i20++; });  // We can change i20 but not i21, auto works
  cout << "Test 3: i20: " << i20 << endl;
  for (auto e: vi) { cout << "   " << e << ", "; }  cout << endl;
  
  c06_forAll(vi, [](int& a){
    int lambdalocal = 10;
    cout << "lambdalocal: " << lambdalocal << endl;
  });
  
  // Variable templates
  auto a1 = acceleration<float>;
  // auto a2 = acceleration<int>;  // Gives error because int cannot be initialized to -9.8
  
  // Type alias
  using size_t2 = unsigned int;
  
  // Type traits
  cout << "is_assignable1: " << is_assignable<double&, int>::value << endl;
  cout << "is_assignable2: " << is_assignable<int&, double>::value << endl;
  cout << "is_assignable3: " << is_assignable<double, string>::value << endl;
  cout << "is_poddable1: " << is_pod<int>::value << endl;  // Can type be trivially copied
  cout << "is_poddable2: " << is_pod<string>::value << endl;
  
  String_map<int> sm = String_map<int>();  // map<string,int>
  
  // Every standard library has a 'value_type' member holding the value type
  // A template member function cannot be virtual (because vtbl is now known)
}


// ***************** Chapter 07

// Concepts (C++20)
// Now:
template <typename Seq, typename Val>
Val sum(Seq s, Val v) {
  for (const auto& e: s) {  // Any Seq supporting begin(), and end()
    v += e;  // Any Val supporting +=
  }
  return v;
}
// Concepts
//   template<Sequence Seq, Number Num> requires Arithmetic<Value_type<Seq>,Num>
//   ... Now same def as sum above
//   Sequence, Number, Arithmetic can be defined using 'concept' keyword (won't go into details here)
//      You should probably never define your own concepts

// Variadic template: Use when you need a function that takes
//   a variable number of arguments of a variety of types
//   Don't use it if you have same type - then use initializer lists
template<typename... T>
void c07_print(T&&... args) {
  using namespace std;
  (cout << ... << args) << endl;
}

void chapter07() {
  std::cout << "*** Starting Chapter 07" << std::endl;
  using namespace std;
  c07_print(10, 20, 30, "hello");
}

// ***************** Chapter 08
void chapter08() {
  std::cout << "*** Starting Chapter 08" << std::endl;
  using namespace std;
  
  // Header files and what they contain
  //   All these symbols are in std namespace
  //
  //   <algorithm>     copy(), find(), sort()
  //   <array>         array
  //   <chrono>        duration, time_point
  //   <cmath>         sqrt(), pow()
  //   <complex>       complex, sqrt(), pow()
  //   <filesystem>    path
  //   <forward_list>  forward_list
  //   <fstream>       fstream, ifstream, ofstream
  //   <future>        future, promise
  //   <ios>           hex, dec, scientific, fixed, defaultfloat
  //   <iostream>      istream, ostream, cin, cout
  //   <map>           map, multimap
  //   <memory>        unique_ptr, shared_ptr, allocator
  //   <random>        default_random_engine, normal_distribution
  //   <regex>         regex, smatch
  //   <string>        string, basic_string
  //   <set>           set, multiset
  //   <sstream>       istringstream, ostringstream
  //   <stdexcept>     length_error, out_of_range, runtime_error
  //   <thread>        thread
  //   <unordered_map> unordered_map, unordered_multimap
  //   <utility>       move(), swap(), pair
  //   <variant>       variant
  //   <vector>        vector
  //
  //   Also, all C-headers have C++ versions witha name convention: <stdlib.h> -> <cstdlib>
  //   This places all its declarations in the std namespace
}

// ***************** Chapter 09
using namespace std;

template<typename Target =std::string, typename Source =std::string>
Target to(Source s) {
  stringstream ss;
  Target t;
  if (!(ss << s)
      || !(ss >> t)
      || !(ss >> std::ws).eof())
    throw new runtime_error("It all failed");
  return t;
}
void testTo() {
  auto a1 = to<string,double>(1.2);
  auto a2 = to<string>(1.2);
  auto a3 = to<>(1.2);
  auto a4 = to(1.2);  // Maximum typ inference
}

void chapter09() {
  std::cout << "*** Starting Chapter 09: Strings" << std::endl;
  using namespace std;
  
  string s00 = "hello";
  string s01 = s00 + ' ';
  s01 += s00;
  cout << s01[0] << endl;
  // string have a move constructor so you can safely return big ones by value
  // strings are mutable
  s01.replace(0,3,"good");
  s01[0] = 'b';
  s01.at(0) = 'c';
  cout << "s01: " << s01 << endl;
  cout << "s01.substr: " << s01.substr(0,2) << endl;
  
  // string also support: ==, !=, <, <=, >, >=
  // And iterators
  for (auto s=s01.begin(); s != s01.end(); s++)
    cout << *s;
  cout << endl;
  cout << "c-str: " << s01.c_str() << endl;
  
  auto s02 = "string literal"s;  // string literal provided by std::literals::string_literals
  auto s03 = "const char* literal";
  
  // string automatically adjusts from array to heat automatically (~<14 character are on array)
  
  // string is really: using string = basic_char<char>
  // using Jstring = basic_string<Jchar>;  // string of japanese characters
  
  // string_view contains pointer to a character, and length. Can be constructed from
  // string_view is read-only of its characters
  string s04 = "Kingdom here";
  string_view sv01 = "hello";  // const char*
  string_view sv02 = s02;  // string
  string_view sv03 = "string view literal"sv;  // defined in std::literals::string_view_literals
  string_view sv04 {&s04[2], 2};
  cout << sv04 << endl;
  
  // Regex
  regex re01{R"(\w{2}\s*\d{5}(-\d{4})?)"};   // US post code: State(2), any number of spaces 5 digits, optionally -4digits
  // regex_match(): Find in string of known size
  // regex_search(): Find in stream
  // regex_replace(): Replace in stream
  // regex_iterator: Iterate over matches & submatches
  // regex_token_iterator: Iterate over non-matches
  smatch sm01;  // vector<string>
  string sr01 = "This is another CA 94041-0001 day at the post office";
  if (regex_search(sr01, sm01, re01)) {
    // 1: Full match, 2: Optional 4-digit subpattern
    cout << "Found matches, 1st: " << sm01[0] << ", 2nd: " << sm01[1] << endl;
  } else { cout << "No matches found" << endl; }
  // Regex is designed so it's compiled into state machines for efficient execution (done at run-time)
  
  // Notation
  //   .      - Any single character
  //   []     - Begin/End character class
  //   {m,n}  - Begin/End count. m=min (req), n=max (opt)
  //   ()     - Begin/end grouping
  //   \      - Next character has special meaning
  //   |      - OR
  //   ^ $    - Start of line/negation End of line
  //   .      - Any single character
  //   * + ?  - Zero or more {0,}, One or more, {1,} Zero or one {0,1}
  regex re02("^A*B+C?$");
  regex re03("^A*B+C{0,1}$");
  //    As seen above, a subpattern (available in a smatch) can be defined by ()
  //    By default, the longest matching path is used (Max Munch rule)
  // Character classes, e.g. [:d:], \ gives shorthand notation (no need for [::]
  //   alnum - Alphanumeric
  //   alpha - Alphabetic
  //   blank - Whitespace (not linefeed)
  //   cntrl - Control character
  //   d digit \d \D - Digit, \D is not \d [^[:digit:]
  //   graph - Graphical character
  //   lower - Lowercase
  //   print - Printable
  //   punct - Punctuation
  //   s space \s \S - Whitespace (space, tab, etc), \S is not space [^[:space:]
  //   upper - Uppercase
  //   w \w \W - Word=Alphanuemeric and underscore [_[:alnum:]], \W is not \w [^_[:alnum:]
  //   xdigit - Hex digit character
  //   Do not use: \l (lowercase character), \u (uppercase character)
  regex cpp_identifier1("[_[:alpha:]]\\w*");  // Underscore or Alpha followed by underscores, letters, or digits
  regex cpp_identifier2(R"([_[:alpha:]]\w*)");  // Raw, no need for \\
  regex r04("(bs|BS)");  // bs or BS
  regex r05("(\\s|:|,)*(\\d*)");   // Optional spaces, colons, and/or commas followed by an optional number
  regex r06("(?:\\s|:|,)*(\\d*)"); // Matches same as r05, but don't emit to me the first () result
  cout << "Matches1? " << regex_match("any general string", r06) << endl;
  regex r07_xml("<(.*?)>(.*?)</\\1>");  // Last is same as group '1'
  cout << "Matches2? " << regex_match("<b>XML tag</b>", r07_xml) << endl;
  smatch sm07;
  string s07 = "<b>XML tag</b>";
  if (regex_search(s07, sm07, r07_xml)) {
    cout << "XML smatch decode" << endl;
    for (string s: sm07) {
      cout << "   " << s << endl;
    }
  } else { cout << "No XML smatch decode found" << endl; }
  // Iterators
  //    sregex_iterator = regex_iterator<string>
  for (sregex_iterator p(s07.begin(), s07.end(), r07_xml); p != sregex_iterator{}; ++p) {
  }
  // regex_iterator{} is only possible end-of-sequence
  // regex_iterator is a bi-directional iterator so we annot iterate over an istream
  
  // Use string and C++ functions instead of C versions
  // Return string by value is ok, string has move semantics
  // Use substr() replace to read/write substrings
  // string automatically adjusts to local/heap storage
  // Use at() instead of [] if you want range checking
  // Use iterators and [] if you optimize for speed
  // Use c_str to get C type string
  // Use stringstream or generic value extraction function to<X>() for numeric conversion of strings
  // basic_string is used to create string of any character types
  // string_view is great
  // Literals ""s and ""sv
  // regex, regex_match, regex_search, regex_iterator
}

// ***************** Chapter 10
void chapter10() {
  std::cout << "*** Starting Chapter 10: Input and Output" << std::endl;
  using namespace std;
  
  // { char, int, tuple } -> ostream -> stream buffer -> byte sequence
  // byte sequence -> stream buffer -> istream -> { char, int, tuple }
  
  // #include <ostream>
  cout << "Printing on stdout" << endl;  // Standard output stream
  cerr << "Printing on stderr" << endl;  // Standard error stream
  
  //    #include <istream>
  // Read of integer is terminated that is not a digit, skips initial whitespace
  // If you read into string, it gets terminated by first whitespace
  //    int i01;
  //    double d01;
  //    cin >> i01 << d01;  // E.g 123 followed by 12.34e5
  // Read a whole line, newline is discarded
  //    string str;
  //    getline(cin, str);
  
  // Do formatting to/from memory using stringstreams
  
  // States in istreams
  struct Dummy {
    void foo(istream& is) {
      for (int i; is >> i;) { }  // Read int, returns false if fails (or returns istream& implicit cast to bool)
      if (is.eof()) { return; }  // Return if nothing else was entered
      if (is.fail()) {
        is.clear();  // Clear error
        is.unget();  // Put non-digit back to stream
        string s;
        if (cin >> s && s == "stop")  { return; }  // Expected 'stop' after the ints
        cin.setstate(ios_base::failbit);  // Did not encounter stop, put cin in failed state
      }
    }
  };
  
  // is >> c skips whitespace
  // is.get(c)  does not skip whitespace
  
  // User defined types
  //   ostream& operator<<(ostream& os, const YourObject& yo) { return os << ... ; }
  //   istream& operator>>(istream& is, YourObject& yo)  {
  //      ... Do the read
  //      if (failure)  is.setstate(ios_base::failbit);
  //      return is;
  //   }
  //
  // Formatting flags, e.g: cout << hex << 255 endl;
  // Defined in <ios>, <istream>, <ostream>, <iomanip>
  //   scientific
  //   hexfloat
  //   fixed
  //   defaultfloat
  //   precision(n): n=number of digits (rounded), does not affect ints
  
  // File I/O: <fstream> has classes: ifstream, ofstream
  ofstream ofs2 {"/tmp/cpptemporaryfile", ofstream::out};
  ifstream ifs2 {"/tmp/file2"};
  if (!ofs2 || !ifs2) { cout << "Error opening file" << endl; }
  
  // StringBuffer: <sstream>, has classes: istringstream>, ostringstream, stringstreams
  ostringstream os;
  os << 10 << "hello" << endl;
  cout << os.str();
  // ifstream in("file.txt")
  // if (!in) { cout << "File not found" << endl; }
  // for (string s; getline(in, line); ) { ... }
  
  // If you don't use C-style stuff (printf, scanf, ...) and care about performance then call
  ios_base::sync_with_stdio(false);
  
  // File system: <filesystem>, namespace filesystem
  using namespace filesystem;
  path p = "tmp/cpptemporaryfile.txt";  // Path can be implicitly converted to a path
  cout << "Exists: " << exists(p) << endl;
  ofstream pofs {p};
  // Traversing directory structure
  try {
    if (is_directory(p)) {
      for (const directory_entry& x: directory_iterator{p})
        cout << x.path() << endl;
    }
  } catch (const filesystem_error& ex) { }
  cout << "Value type: " << path::value_type() << endl;
  cout << "Filename: " << p.filename() << endl;
  cout << "Stem: " << p.stem() << endl;
  cout << "Extension: " << p.extension() << endl;
  for (auto c=p.begin(); c!=p.end(); ++c) {
    cout << "Another iterator part: " << *c << endl;
  }
  cout << endl;
  
  // Filesystem ops (there are more). Use throw versions if unavailability is unexpected
  cout << "exists: " << exists(path{"/tmp"}) << endl;
  // copy(p1,p2) throws
  // copy(p1, p2, error_code e);
  // bool copy_file(p1, p2
  cout << "Create directory /tmp/mh: " << create_directory(path{"/tmp/mh"}) << endl;
  cout << "Create directory /tmp/mh/1/2: " << create_directories(path{"/tmp/mh/1/2"}) << endl;
  cout << "Current path: " << current_path().string() << endl;
  // Set current path: current_path("/tmp")
  cout << "File size: " << file_size(path{"/etc/man.conf"}) << endl;
  // Remove file or empty directory: bool remove(path)
  // Types of files: f is a path or file_status
  //    is_block_file(f)
  //    is_character_file(f)
  //    is_directory(f)
  //    is_empty(f)
  //    is_fifo(f)
  //    is_other(f)
  //    is_regular_file(f)
  //    is_socket(f)
  //    is_symlink(f)
  //    status_known(f)
  
  // Avoid endl
  // By default >> skips initial whitespace
  // Use stream state fail to handle potentially recoverable errors
  // Don't use C-style I/O printf/scanf, not typesafe
  //    Call ios_base::sync_with_stdio(false);  If you do not use C-style stuff
}

// ***************** Chapter 11

void c11_foo() {  // TODO: What are the 'throw' declaration you can use here & C++20
  throw new std::out_of_range("Error");
}
void chapter11() {
  std::cout << "*** Starting Chapter 11" << std::endl;
  using namespace std;
  
  // Some more exception stuff
  try { c11_foo(); }
  catch(std::out_of_range ex) { }
  catch(...) {}  // Catch anything else
  
  // All STD containers has begin(), end()
  
  // vector elements are stored an array
  vector<double> v01(32, 9.9);  // 32 elements with value 9.9
  vector<double> v02 = { 3.14, 2.72 };
  // Initial value when given size is 0 for ints, and nullptr for pointers
  v02.push_back(33.33);
  // Copying vectors by value can be expense, use references or pointers in that case
  // .at() does range checking: throws out_of_range, [] does not do range check
  //    Penalty: 10%? You could subclass vector<T> and switch between range and non-range checking
  vector<pair<string,int>> v000 = {{"a",0}, {"b",1}};
  
  list<double> l00;  // Double-linked list allowing easy insert and delete without entire moves
  for (auto i=l00.begin(); i != l00.end(); ++i) {
    l00.insert(i, 3.14);
    l00.erase(i);
  }
  // Use vector when uncertain: It is faster for traversal, sourting
  
  // forward_list<double> fl00;  // Single-linked list. Probably not use.
  
  map<string,int> m00 = {{"k0", 0}, {"k1", 1}};
  int m00v = m00["k3"];  // Will actually insert this key with value 0
  cout << "m00v: " << m00v << '\n';
  // Use .find() and .insert() to avoid implicit insertion made by []
  // map is implemented as a r-b tree, so O(log n) search so it's linked list of key/values (associative array)
  //   As such it requires an ordering function '<'
  
  // Hashmap (because keys are unordered)
  // Key requires hash function and equality ==
  //   O(1)
  unordered_map<string,int> um00;
}

// ***************** Chapter 12
void chapter12() {
  std::cout << "*** Starting Chapter 12: Algorithms" << std::endl;
  using namespace std;
  
  vector<int> v00;
  list<int> l00;
  
  //Sort
  sort(v00.begin(), v00.end());  // Sort using <
  
  // Copy
  unique_copy(v00.begin(), v00.end(), l00.begin());  // Requires ==
  unique_copy(v00.begin(), v00.end(), back_inserter(l00));  // Append unique elements to l00
  
  // Strings also have iterators
  string s00 = "Oaktree"s;
  for (auto si=s00.begin(); si != s00.end(); si++)  cout << *si << '\n';
  
  // find: For specific match
  cout << (find(s00.begin(), s00.end(), 'k') != s00.end()) << '\n';
  // find_if #2: Using lambda
  // Alt, use a Function Object, overloading: bool operator()(const pair<string,int>& a) const { ... }
  // OBS: A lambda, function object may not modify its arguments
  map<string,int> m00 { {"a", 0}, {"b", 1}, {"c", 2} };
  auto m00found = find_if(m00.begin(), m00.end(), [](const auto& a1) { return a1.second > 0; });
  cout << "First matching element: " << m00found->first << endl;
  
  // Collections always have begin() and end(). Iterators always have: ++ and *
  // SDL collections define their iterators in scope Collection::Iterator (since they implement their own logic)
  
  // IOStream magic. Could use set instead of vector to avoid duplicates and only use copy
  istringstream iss00 { "A stone's throw throw from Jerusalem\nI walked\nwalked a lonely mile in the moonlight"s };
  cout << "iss00 after construction: " << iss00.eof() << !iss00 << '\n';  // We should be at eof, and it is marked with error (eof)
  istream_iterator<string> isi0s { iss00 };  // Why does it separate by and discard white spaces?
  istream_iterator<string> isi0e {};  // Sentinel, beyond last element
  ostream_iterator<string> osi { cout, "\n" };
  vector<string> vs00 { isi0s, isi0e };  // Initialize vector with istream iterator content
  unique_copy(vs00.begin(), vs00.end(), osi);  // Copy vector with no adjacent duplicates
  cout << "iss00 at end: " << iss00.eof() << !iss00 << '\n';  // We should be at eof, and it is marked with error (eof)
  cout << "cout should not be an error: " << !cout << '\n';
  
  // Other algorithms: p=iterator, out=iterator, i.e return result range is [out: return)
  // iterator for_each(b, e, f)
  // iterator find(b, e, x) / find_if(b, e, f)
  // int count(b, e, x) / count_if(b, e, f)
  // int replace(b, e, v, v2) / replace_if(b, e, r, v2)  // Obs modifies container
  // iterator copy(b, e, out / copy_if(b, e, out, f)
  // iterator move(b, e, out)
  // iterator unique_copy(b, e, out)
  // sort(b, e)
  // sort(b, e, f)
  // (p1, p2) = equal_range(b, e, v)  // Return [p1, p2) for values matching v
  // iterator merge(b, e, b2, e2, out) merge b/e pairs into [out: return)
  // iterator merge(b, e, b2, e2, out, f) merge b/e pairs into [out: return)
  
  // Concurrency and vectorization
  // Many algorithms can optionally start with an argument specifying execution paradigm
  // Xcode does not recognize these keywords though
  //   sort(seq, ...)  // sequential single threaded execution
  //   sort(par, ...)  // multi-threaded execution
  //   sort((par_unseq, ...)  // multi-threaded or vectorized execution
}

// ***************** Chapter 13
int c13_foo() { return 10; }
void chapter13() {
  std::cout << "*** Starting Chapter 13: Utilities" << std::endl;
  using namespace std;
  
  class Class {
  public:
    Class(string a, int b, char c) {};
  };
  
  // unique_ptr: Allocated heap is reclaimed when up00 destructs, no ref counting, no overhead
  // Obs: unique_ptr supports move constructor so ok to return a local instance b
  unique_ptr<Class> up00(new Class("a", 42, 'a'));
  // TODO: How can make_unique be implemented like this?
  auto up01 = make_unique<Class>("a", 42, 'a');  // Preferred
  
  // shared_ptr: Reference counted, help reclaimed when count reaches 0
  auto sh00 = make_shared<Class>("a", 42, 'a');
  
  // Explicit use of move constructor
  // Dangerous, up00 is now nullptr, avoid using move
  auto up02 = move(up00);
  
  // TODO: I don't understand std::forward<Args&&...>(args)
  // But it has to do with variadic template (variable list of template arguments)
  
  // Guidelines Support Library: gsl::span (not yet in standard, can be downloaded)
  // gsl::span<int> s00;  // Holds unowned int* and its size: begin(), size()
  // int[] ia[100];
  // span<int> s01 { ia, 100 }
  // for (int& i: s00) { ... }
  // Idea of span: Do not send (int* pl, int size) to a function, send (span<int> s)
  
  // array<type, #elements>: Same performance & space as built-in array []
  char c00[c13_foo()];
  char c01[] = { 'a' };
  char* c02 = c01;
  cout << "c02 is: " << c02[0] << '\n';
  array<int, 3> a00 = {1,2,3};  // With array object you must specify compile-time size
  int* a01 = a00.data();
  int* a02 = &a00[0];
  // array<int> a01 = {1,2,3};  // You must provide element count
  // array<int, c13_foo()> a02 = {1,2,3};  // Size must be constant
  
  // bitset<#bits>: If you want to name the bits, use set or enum
  bitset<9> bs00 {"110001111"};
  bitset<9> bs01 {0b1'1000'1111};
  cout << "Is bs00 == bs01: " << (bs00==bs01) << '\n';  // 1==true
  bitset<8> bs02 {128};
  bitset<9> bs03 = ~bs00;  // Complement
  bitset<9> bs04 = bs03 & bs00;  // All zeros
  // Shift operators << >> of course work
  cout << "to_string: " << bs00.to_string() << ", to_ullong: " << bs00.to_ulong() << '\n';
  
  // pair: From <utility>
  auto [first, last] = pair<string, int> {"Hello", 22};
  auto p00 = pair<string, int> {"Hello", 22};
  auto p01f = p00.first;
  auto p01s = p00.second;
  auto p02 = make_pair("Hello", 23);
  auto t00 = make_tuple("Hello", 24, 1, 'a');
  cout << "Get element from tuple: " << get<2>(t00) << ", weirder: " << get<char>(t00) << '\n';
  get<char>(t00) = 'b';
  get<3>(t00) = 'c';
  cout << "Tuple is now: " << get<3>(t00) << '\n';
  
  // variant: Use instead of union. Return string or error code
  variant<string,int> foo(void);
  variant<string,int> v00 {"A"};
  cout << "Variant is string: " << holds_alternative<string>(v00) << '\n';
  cout << "Variant is int: " << holds_alternative<int>(v00) << '\n';
  cout << "Variant value: " << get<string>(v00) << '\n';
  // Is a substitute for void*: variant<Expression,Statement,Declaration,Type> v01 = something();
  // bad_variant_access is thrown if we access wrong type
  // Book says below should be possible, but overloaded is not recognized
  //  visit(overloaded {
  //    [](string& s) { cout << "string: " << s << '\n'; }
  //    [](int& i) { cout << "int: " << i << '\n'; }
  //  });
  
  // optional
  optional<string> s00 = "hello";
  if (auto a=s00) { cout << "s00 was defined: " << *a << '\n'; }
  if (s00) { cout << "s00 was defined" << '\n'; }
  // An optional returns nullptr if value is not set
  
  // any
  // Does not compile
  // any a00 { "stone" };
  // cout << any_cast<string>(a00) << '\n';  // Throws bad_any_access if error
  
  // Allocators: STL containers use new/delete by default but can optionally take an Allocator
  // This could solve problem related to e.g memory fragmentation
  // Below code does not compile in Xcode though
  //  class MyEvent {};
  //  pmr::synchronized_pool_resource pool;
  //  list<shared_ptr<MyEvent>> pmr00 { &pool };
  //  pmr00.push_back(allocate_shared<MyEvent, pmr::polymorphic_allocator<MyEvent>{&pool}();
  
  // Time: <chrono>
  using namespace std::chrono;
  time_point ct00 = high_resolution_clock::now();
  std::this_thread::sleep_for(std::chrono::milliseconds{100});  // Needs <thread>
  std::this_thread::sleep_for(1000ms + 33us);
  time_point ct01 = high_resolution_clock::now();
  cout << "Duration: " << duration_cast<milliseconds>(ct01-ct00).count() << "ms\n";
  // c++20 has more stuff:
  //    auto spring_day = apr/7/2018;
  //    cout << weekday(spring_day) << '\n';
  
  vector vn00 { 1, 2 };
  for_each(vn00.begin(), vn00.end(), [](int& i) { cout << "i: " << i << '\n'; });
  struct MemFn { void foo() { cout << "foo" << '\n'; } };
  vector vn01 { MemFn{}, MemFn{} };
  for_each(vn01.begin(), vn01.end(), mem_fn(&MemFn::foo));
  // Does not compile in Xcode: function fo00<int(double)> = [](int){ return 3.14; };
  
  char space00[c13_foo()];  // For some reason this is ok, how can it be, what is memory layout?
  class MySpace {
    char a[10];
    // char b[c13_foo()];  // But this is not ok
  };
  cout << "min_float: " << numeric_limits<float>::min() << '\n';  // From <limits>
  cout << "sizeof(MySpace): " << sizeof(MySpace) << '\n';
  
  // iterator_traits: Find traits of iterators. c++20 concepts will simplify this
  //    random access iterators: required by sort
  //    forward_iterator: single_linked list
  // iterator_traits<vector<int>> it00;
  
  
  cout << "is_arithmetic<int>: " << is_arithmetic<int>() << '\n';
  cout << "is_arithmetic<MySpace>: " << is_arithmetic<MySpace>() << '\n';
  cout << "is_class<MySpace>: " << is_class<MySpace>() << '\n';
  
  // Not sure how this works...
  //  template<typename T>
  //  class c13_enable_if {
  //    std::enable_if<is_class<T>(), T&>operator->();
  //  }
  
  // basic_string<C>: Provides string operations on C character class
  // valarray<T>: Array of numeric values, providing math on content
}

// ***************** Chapter 14
void chapter14() {
  std::cout << "*** Starting Chapter 14: Numerics" << std::endl;
  using namespace std;
  
  // <cmath>
  cout << "abs(-3.14): " << abs(-3.14) << '\n';
  cout << "ceil(3.14): " << ceil(3.14) << '\n';
  cout << "ceil(-3.14): " << ceil(-3.14) << '\n';
  cout << "floor(3.14): " << floor(3.14) << '\n';
  cout << "floor(-3.14): " << floor(-3.14) << '\n';
  // sqrt, cos, sin, tan, acos, asin, atan, sinh, cosh, tanh
  // beta, rieman_zeta, sph_bessel
  cout << "exp(2): " << exp(2.0) << '\n';
  cout << "log(1): " << log(1.0) << '\n';
  cout << "log10(10): " << log10(10) << '\n';
  cout << "pow(2, 2): " << pow(2, 2)  << '\n';
  
  // <errno>
  errno = 0;
  cout << "Errno: " << errno << ", EDOM: " << EDOM << ", ERANGE: " << ERANGE << '\n';
  cout << "sqrt(-1): " << sqrt(-1) << '\n';
  cout << "After sqrt. Errno: " << errno << ", EDOM: " << EDOM << ", ERANGE: " << ERANGE << '\n';
  // Weird sqrt(-1) should give errno==EDOM
  
  // <cstdlib> contains a few more mathematical function
  
  // <numeric>: Numerical algorithms
  //   accumulate
  //   inner_product
  //   partial_sum
  //   adjacent_difference
  //   iota: Assign values to vector
  //   gcd(n,m): Greatest common denominator of integers n and m
  //   lcd(n,m): Least common multiple of integers n and m
  // <numeric> also has parallel versions
  //   reduce
  //   exclusive_scan
  //   inclusive_scan
  //   transform_reduce
  //   transform_exclusive_scan
  //   transform_inclusive_scan
  
  // <complex> for complex numbers, also include sqrt and pow functions
  
  // <random>
  default_random_engine myre {};
  myre.seed(time(0));
  uniform_int_distribution<> one2six { 1, 6 };
  cout << "one2six: " << one2six(myre) << '\n';
  cout << "one2six: " << one2six(myre) << '\n';
  cout << "one2six: " << one2six(myre) << '\n';
  // Also uniform_int_distribution, normal_distribution, exponential_distribution
  
  // valarray: like vector but that can do math ops too
  // Before writing a loop, consider using accumulate, inner_product, partial_sum, adjacent_difference
  // Don't use C standard library rand
  // Use numeric_limits to find properties of numeric types
}

// ***************** Chapter 15
void chapter15() {
  std::cout << "*** Starting Chapter 15: Concurrency" << std::endl;
  using namespace std;
  
  // C++ provides suitable memory model and atomic operations to allow lock-free programminmg
  // STL: thread, mutex, lock, packaged_task, future. As fast as OS constructs

  // Task: Things that can execute concurrently
  //   Thread: System-level representations of a task
  
  // A task is a function or function object (overloading void operator()())
  vector<double> v1 { 1, 2, 3 };
  vector<double> v2 { 1, 2, 3 };
  double result = 0.0;
  // thread t00 { f, ref(v1) };  // void f(vector<double>& v). Variadic template, ref is need to pass reference (not value)
  // thread t01 { F{v2} };
  // thread t02 { [&]() { result = g(); }
  // t00.join();  // Wait until t00 is finished
  // t01.join();  // Wait until t01 is finished
  // t02.join();  // Wait until t01 is finished
  // Get results by binding them to locals t02, or references

  
  // Thread will not continue until l00 constructor has acquired mutex m
  mutex m;  // Needs <mutex>
  // Locks mutext through m.lock() or blocks if already acquired
  // Destructor unlocks using m.unlock(): Any thread waiting will wake up
  scoped_lock<mutex> l00(m);  // scoped_lock lck { m1, m2, m3 };
  // So, you need to know which mutex is related to which data
  
  // Multi-reader
  //   shared_mutex m01;
  //   shared_lock<mutex> sl01{m01};  // Multiple readers can aqcuire this lock
  // Single-write (no readers or writers allowed)
  //   unique_lock ul01{m01};
  
  // Event: sleep <chrono>
  auto t0 = chrono::high_resolution_clock::now();
  this_thread::sleep_for(chrono::milliseconds{20});
  auto t1 = chrono::high_resolution_clock::now();
  // duration_cast adjust clocks unit to another unit
  cout << "duration: " << duration_cast<chrono::nanoseconds>(t1-t0).count() << "ns\n";
  
  // Event: Exchanging events between 2 thread using <condition_variable>
  // condition_variable allows one thread to wait for a condition/event caused by another thread
  class Message {};
  std::queue<Message> mqueue;
  condition_variable mcond;
  mutex mmutex;
  // Reader thread: while(true) {
  //   Need unique_lock, scoped_lock cannot be copied + does not have low-level ops like unlock
  //     unique_lock lck {mmutex};
  //   Wait on lck until queue is non-empty. While waiting mcond releases lck, then re-acquires it (if !empty)
  //     mcond.wait(lck, []{ return !mqueue.empty;});  // lck is copied, does not support scoped_lock
  //   Message m = mqueue.pop()
  //   lck.unlock();
  // }
  // Writer thread: while(true) {
  //   Message m;
  //   scoped_lock lck {mmutex};
  //   mqueue.push(m);
  //   mcond.notify_one();
  //   Lock is released here, end of scope
  // }
  
  // <future>
  // future and promise: Transfer a value efficiently between 2 tasks without using locks
  // Task #1: Put a value into a promise. Value appear in Task #2 as a future.
  // Future
  //   future<Message> f00;
  //   try {
  //      Message m = f00.get();  // Thread blocks until value is available (or exception)
  //   } catch (...)  {  }
  // Promise
  //   promise<Message> p00;
  //   p00.set_value(Message {});
  //   promise.set_exception(...);
  // packaged_task is what glues futures and promises together
  /*  For some reason below causes SIGABRT on exit from function
  struct PTTask { static double accum(double* beg, double* end) { return accumulate(beg, end, 0); }  };
  packaged_task<double(double*, double*)> pt00 {PTTask::accum};
  packaged_task<double(double*, double*)> pt01 {PTTask::accum};
  future<double> ft00 { pt00.get_future() };
  future<double> ft01 { pt01.get_future() };
  vector<double> v { 1.0, 2.0, 3.0, 4.0, 5.0 };
  auto vs = v.size();
  auto v0 = &v[0];
  // Move needed: package_task can't be copied
  thread tpf01 { move(pt00), v0,      v0+vs/2 };
  thread tpf02 { move(pt01), v0+vs/2, v0+vs   };
  cout << "Results from both threads: " << (ft00.get() + ft01.get()) << '\n';
  
  // async: Convenience to run parallel tasks
  //   Never use if you need resources sharing/locks, you don't know how many threads it fires up
  auto asf0 = async(PTTask::accum, v0, v0+vs/2);
  auto asf1 = async(PTTask::accum, v0+vs/2, v0+vs);
  cout << "Result from async: " << asf0.get() + asf1.get() << '\n';
   */
    
  // Treat a thread as a function (but running in parallel)
  // Don't underestimate cost of starting thread, e.g. if <x elements in vector, maybe do it sequentially
  // Consider using processes instead of threads
  // The thread is the interface to a system thread
  // Use .join to wait for a thread to complete
  // Use scoped_thread: i) To manage mutex's, ii) Aquire multiple locks
  // Use shared_lock: To implement reader/writer
  // Use condition_variables: To manage communication amoung threads
  // Use unique_lock (instead of scoped_lock) if: i) Need to copy a lock, ii) Need .lock() .unlock() explicit control
  // Prefer packaged_task/future/promise over threads/mutexes (return value: promise, get value: future)
  //   (can also manage exception propagation)
  //   Use packaged_task and future to express a request to external service and wait for response
  // Use async to launch a simple task in parallel
}

// Terminology:
//   Invariants: Conditions that hold true for a class (e.g. like a function pre-/post-condition
//      It is constructors responsibility to check for this
//   RAII: Resource Acquisition Is Initialization: Allocate in constructor, deallocate in destructor
//   Constrained Template Arguments: == C++20 Concepts
//   Value Template Argument: A template argument can be e.g. Vector<Elem, 4> v;
//      E.g, size can now be constexpr size();  Since 4 is known at compile time
//   Template Argument Deduction (C++17): pair p = {1, 5.2};
//   Other types of templates
//     Function Templates e.g. arg/return value
//     Function Objects operator ()
//     Lambda expressions
//     Variable Templates: constexpr bool Assignable = is_assignable<T1& T2>::value;
//   Compile-time if:  if constexpr(is_pod<T>::value) { ... }  // Select at compile time (is_pod = true if type can be trivially copied)
//   Modules (C++20)
//   Concepts (C++20)
//   Variadic templates: Template receiving variables #args with variable types
//     Fold expressions: int a = (v + ... + 0);  // Add all elements of v to 0
//   Predicate: Looking for an element that fulfills a specific requirement, e.g find_if (using Function Object or lambda)
//   Type functions: Evaluated at compile time: constexpr float min = numeric_limits<float>::min();
//   When task t1 puts a value into a promise, task 1 reads the value in corresponding future
//   TODO: Iterator traits
//   TODO: && and rvalues, why would you foo(int&& a)
//   TODO: std::tuple_element

template<typename T>
struct U00_C0 {
  int _a, _b;
  vector<T> _v;
  U00_C0<T>(initializer_list<T> a) : _v{a} {
    _v.assign(a);
    _v.assign(a.begin(), a.end());
  }  // All works
  U00_C0<T>(int a, int b) { _a = a; _b = b; }
  U00_C0<T>(double a, double b) { _a = a; _b = b;}
};

template<typename T>
struct U00_C1 {
  int _a, _b;
  U00_C1<T>(double a, double b) { _a = a; _b = b;}
};

void utils()  {
  // TL;DR: {} construction enforces only initializer_list if defined, otherwise uses other constructors
  // Constructor with initializer_list overrides a fixed arg constructor when using {}
  cout << "Starting utils\n";
  U00_C0<int> tc00 { 10 };
  U00_C0<int> tc01 { 10, 20 };  // Will still use initializer_list<int> constructor
  U00_C0<int> tc02(10, 20);     // Will call (int, int) constructor
  U00_C0<int> tc03 { 10, 20, 30 };
  // U00_C0<int> tc04 { 3.14, 2.72 };  // Error, no initializer_list<double>
  U00_C0<int> tc05(3.14, 2.72);  // Error
  U00_C1<int> tc11 {3.14, 2.72}; // This works U00_C1 has a double initializer_list

  // Size is 4
  std::size_t intsize = sizeof(int);  // Can be used for classes
  cout << "size of int: " << intsize << '\n';
  
  // *** vector
  cout << "*** vector" << '\n';
  vector<int> v00(4, 8);  // 4 elemenent with value 8, alt = { 8, 8, 8, 8 };
  vector<int> v01 { 1, 2, 3, 4 };
  // Element access
  v00[0] = 2;     // No range checking
  v00.at(0) = 3;  // Range checking
  int efirst = v00.front();
  int elast  = v00.back();
  int* data = v00.data();
  // Iterators
  auto ibegin = v00.begin();
  *ibegin = 10;
  auto iend   = v00.end();
  auto icbegin = v00.cbegin();
  // *cbegin = 20;  // Error cbegin is a constant iterator
  auto icend = v00.cend();
  cout << "Forward iterator: " << *v01.cbegin() << '\n';
  cout << "Reverse iterator: " << *v01.crbegin() << '\n';
  cout << "Reverse iterator++1: " << *(++v01.crbegin()) << '\n';  //
  // Capacity
  v00.empty();
  v00.size();
  v00.reserve(10);
  v00.capacity();
  v00.shrink_to_fit();
  // Modifiers
  v00.clear();
  v00.assign(10, 8);  // Create 10 elements of value  8
  v00.assign({ 1, 2, 3, 4});  // Assign with initializer list
  v00.insert(v00.begin(), 10);  // Insert 10 at front()
  v00.erase(v00.begin(), v00.begin()+1);
  v00.push_back(57);  // Append element
  v00.pop_back();  // Remove last element (not returning it)
  
  // *** Pair and Tuple
  cout << "*** pair/tuple" << '\n';
  pair<int, int> p00 = { 1, 2 };
  int p00i = p00.first;
  p00i = p00.second;
  auto p01 = std::make_pair(1, "hello");
  auto [p02_1, p02_2] = std::make_pair(1, "hello");
  // Before C++17: int p03_1; string p03_2; std::tie(p03_1, p03_2) = std::make_pair(1, "hello");
  tuple<int, int, int> t00 = { 1, 2, 3 };
  auto t01 = std::make_tuple(1, "hello", 2, 3.14);
  auto [t02_1, t02_2, t02_3, t02_4] = t01;
  cout << "Second in a tuple: " << std::get<1>(t01) << '\n';

  // *** unordered_map
  cout << "*** unordered_map\n";
  unordered_map<string, int> um00 = { {"a", 1}, {"b", 2} };
  um00["c"] = 3;  // Insert or replace
  um00.insert(pair {"e", 99});
  um00.empty();
  um00.size();
  for (auto l=um00.begin(); l!=um00.end(); l++)   { cout << " {" << l->first << "," << l->second << '}'; }  cout << '\n';
  vector<pair<string, int>> vum00 { um00.begin(), um00.end() };
  for (auto l=vum00.begin(); l!=vum00.end(); l++) { cout << " {" << l->first << "," << l->second << '}'; }  cout << '\n';
  cout << "um00[\"b\']: " << um00["b"] << ", " << um00.at("b") << '\n';
  cout << "um00.bucket_count: " << um00.bucket_count() << ", " << um00.bucket_size(0) << '\n';
  unordered_map<string, int> um01 { vum00.begin(), vum00.end() };
  vum00.insert(vum00.begin(), {{"a", 1}, {"b", 2}});
  um01["a"] = 99;
  um01.insert({{"a", 1}, {"b", 2}});
  cout << "Insert\n";
  for (auto l=um01.begin(); l!=um01.end(); l++)   { cout << " {" << l->first << "," << l->second << '}'; }  cout << '\n';

  
  cout << "*** string" << '\n';
  string s00 = "hello";
  s00.replace(0,2,"goodby");
  cout << "  s00: " << s00 << '\n';
  s00 += " throw";
  cout << "  s00: " << s00 << '\n';
  s00.assign(8, 'a');
  cout << "  s00: " << s00 << '\n';
  s00 = "0123456789";
  cout << "  s00: " << s00.substr(4, 2) << '\n';

  // *** Prefix vs postfix ++
  cout << "*** prefix vs postfix ++" << '\n';
  int pia = 0;
  int pib = (pia++) + 1;
  int pic = pia++ + 1;
  cout << "pia: " << pia << ", pib: " << pib << ", pic: " << pic << '\n';  //  pia=2, pib=1, pic=2
  for (int i=0; i < 4; ++i) { cout << i << ", "; } cout << '\n';  // 0, 1, 2, 3
  int pif = 0;
  if (pif++) {  } // Will not be entered, but aif==1 after closing }
  
  pia = pib = pic = 0;
  pia = pib++ + pib;
  cout << "Increment statement pia: " << pia << '\n';

  int pii[] = { 0, 1, 2 };
  int* piip1 = pii;
  int* piip2 = &pii[0];  // Creates a pointer to first element of pii
  cout << "pii[0]: " << pii[0] << ", *piip2: " << *piip2 << '\n';
  cout << "piip1 == piip2?: " << ((piip1==piip2) ? "true": "false") << '\n';

  string pis[] = { "a", "stones", "throw" };
  string* pisp1 = pis;
  string* pisp2 = &pis[0];
  cout << "pisp1 == pisp2?: " << ((pisp1==pisp2) ? "true": "false") << '\n';
  cout << "*++pisp1: " << *++pisp1 << '\n';
  cout << "*pisp2++: " << *pisp2++ << '\n';
  cout << "*pisp2: "   << *pisp2 << '\n';

  // *** Function pointers
  cout << "\n*** Function pointers\n";
  struct FP00 { static string foo(const string& s) { return "FP00::foo: " + s; }; };
  auto fp00Lambda = [](const string& s)->string { return "FP00Lambda: " + s; };
  string (*fp00Foo)(const string&);
  fp00Foo = FP00::foo;
  cout << fp00Foo("Calling fptr through static struct function\n");
  fp00Foo = fp00Lambda;
  cout << fp00Foo("Calling fptr through lambda\n");
  
  cout << "\n*** General\n";
  // Create an array as a literal
  for (auto& x: { "burgers", "fries", "soda" }) { }
  uint32_t unsigned_int_32_bits = 100;
  const void* g01 = "Hello";
  cout << "Analyzing g01 content: ";
  for (int i=0; i < 6; i++)  cout << (int)((char*)g01)[i] << ", "; cout << '\n';
  void* g02 = malloc(6);
  memcpy(g02, g01, 6);
  cout << "g02: " << (char*)g02 << '\n';
  
  cout << "\n*** const, lvalue/rvalue, and move semantics\n";
  // Reference to an int object which happens to be constant
  // c00 must be initialized, and cannot be reassigned
  // Compiler probably generates: int tmp = 10; and then uses it as 10 rvalue
  const int& c00 = 10;
  // A reference to a constant int
  // Same as c00
  int const& c01 = 20;

  // Constant reference to an int
  // Error: References are always constant (refer to the same memory)
  // int& const c02 = 30;

  // Integer to a reference of a const
  // Error: Makes no sense
  // const& int c03 = 40;

  
  // int& c000 = 10;  // ERROR: Non-const ref cannot bind to a temporary variable
  auto crl00 = [](string& a)->string { return a; };
  auto crl01 = [](const string& a)->auto { return a; };
  string a = "10", b = "20";
  crl00(a);  // Ok
  // crl00(a+b);  // ERROR: Non-const ref cannot bind to a temporary variable (argument is string&)
  crl01(a+b);  // Ok, "const string& can accept an rvalue"
  // Can we write a lambda that only accepts rvalue references? Yes.
  auto crl020 = [](string&& a) { return (string&&)a; };
  auto crl02 = [](string&& a)->string {
    return a;
  };
  crl02("hello");
  crl02(a+b);
  string&& crlv00 = a+b;  // OK: we can assign to a temporary variable
  const string& crlv01 = a+b;
  crlv00 = b+a;
  // crl02(crlv00);  // ERROR: crlv00 is an lvalue
  // string&& crlv01 = a;  // ERROR: Cannot assign to an lvalue
  // crl02(a);  // ERROR: Cannot pass lvalue to argument only accepting rvalue references
  // You can overload a function, one accepting string& and one string&&
  //   string&& is chosen over one that has const string&
  // Why rvalue reference?
  //   Optimization: If we know we are taking in a temporary object. Then,
  //      a. We don't have to worry about keeping it alive
  //      b. We can steel its resources (e.g. char*) and use it somewhere else
  //         We know it wont exist beyond the function scope

  // Move constructor
  // 1. Avoid copy construction of tmp argument object
  struct CRLEntity {
    CRLEntity(int a)            { _a = a; cout << "CRLEntity constructor\n"; };
    CRLEntity(const CRLEntity&) { cout << "CRLEntity, copy constructor\n"; };
    CRLEntity(CRLEntity&&)      { cout << "CRLEntity, move constructor\n"; };
    int _a;
  };
  auto crll00 = [](CRLEntity v)  { cout << "Lambda 1\n"; };
  crll00(CRLEntity{1});  // Creates temporary object, the copy constructs it into v
  auto crll01 = [](const CRLEntity&& v)  { };
  crll01(CRLEntity{2});  // Calls CRLEntity(const CRLEntity&&), rip things from tmp object
  
  struct IOBase {};
  struct IODerived : IOBase {};
  struct IODisjoint {};
  cout << "\n1. Is base class: " << std::is_base_of<IOBase,IOBase>::value << '\n';
  cout << "2. Is base class: "   << std::is_base_of<IOBase,IODerived>::value << '\n';
  cout << "3. Is base class: "   << std::is_base_of<IODerived,IODerived>::value << '\n';
  cout << "4. Is base class: "   << std::is_base_of<IOBase,IODisjoint>::value << '\n';
  

}

// **********************************************************************
string doit(const string& in);
void coding_problems() {
   doit("This is a string");
   
//  cout << "\ncoding_problems\n";
//  string sortByOccurrence_um(const string& str);
//  cout << "sortByOccurrence_um: [" << sortByOccurrence_um("This is an example string") << "]" << '\n';
//
//  bool isPeriodic(int period, const string& str);
//  cout << "isPeriodic: " << isPeriodic(3, "abcabcacb") << '\n';
//
//  bool isBalancedBTMain();
//  cout << "isBalancedBT: " << isBalancedBTMain() << '\n';


}

//---
bool isPeriodic(int period, const string& str) {
  if (str.length() % period != 0)
    return false;
  for (int i=period; i < str.length(); i+=period) {
    if (str.substr(0, period) != str.substr(i,period)) {
      return false;
    }
  }
  return true;
}

//---
struct BTNode {
  string n;
  BTNode* l;
  BTNode* r;
  int v;
};
bool isBalancedBTMain() {
  
//  struct Node *root = newNode(3);
//  root->left        = newNode(2);
//  root->right       = newNode(5);
//  root->left->left  = newNode(1);
//  root->left->right = newNode(4);
  
  //       40
  //   20       50
  // 10  30
  BTNode n1 = BTNode{"n10", nullptr, nullptr, 10};
  BTNode n2 = BTNode{"n20", nullptr, nullptr, 20};
  BTNode n3 = BTNode{"n30", nullptr, nullptr, 30};
  BTNode n4 = BTNode{"n40", nullptr, nullptr, 40};
  BTNode n5 = BTNode{"n50", nullptr, nullptr, 50};
  n4.l = &n2;
  n4.r = &n5;
  n2.l = &n1;
  n2.r = &n3;
  CPPIndentWriter iw;
  cout << '\n';
  int minValue = 0;
  bool isBalancedBT(CPPIndentWriter&, const BTNode*);
  bool r = isBalancedBT(iw, &n4);
  cout << iw.getString();
  return r;
}

string btGetStr(const BTNode* n) {
  return n != nullptr ? n->n : "null";
}

bool isBalancedBT(CPPIndentWriter& iw, const BTNode* n)  {
  if (n == nullptr) {
    return true;
  }
  
  bool l = isBalancedBT(iw, n->l);
  cout << "Visited: " << n->v << '\n';
  bool r = isBalancedBT(iw, n->r);
  
  return false;
}


//---
// function pointer
// lambda binds : lambda is the way to do nested C++ functions
// cout << boolalpha
// std::less, std::greater: How do they work
bool  sortByOccurrence_m_f(const pair<char,int>& v1, const pair<char,int>& v2) {
  return v1.second > v2.second ? true : v1.first < v1.second;
}
struct sortByOccurrence_m_fo { bool operator()(const pair<char,int>& v1, const pair<char,int>& v2) { return v1.second > v2.second ? true : v1.first < v1.second; } };
string sortByOccurrence_m(const string& str) {

  auto lambda = [](const pair<char,int>& v1, const pair<char,int>& v2)->bool { return v1.second > v2.second ? true : v1.first < v1.second; };
  bool (*fptr)(const pair<char,int>& v1, const pair<char,int>& v2);
  fptr = sortByOccurrence_m_f;
  fptr = &sortByOccurrence_m_f;
  fptr = lambda;

  map<char, int, sortByOccurrence_m_fo> m00;
  map<char, int, decltype(lambda)> m01(lambda);
  map<char, int, bool (*)(const pair<char,int>&, const pair<char,int>&)> m02(lambda);
  map<char, int, std::function<bool(const pair<char,int>&,const pair<char,int>&)>> m03(lambda);
  map<char, int, std::function<bool(const pair<char,int>&,const pair<char,int>&)>> m04(fptr);
  map<char, int, bool (*)(const pair<char,int>&, const pair<char,int>&)> m05(fptr);

  return "";
}

//---
string sortByOccurrence_um(const string& str) {
  unordered_map<char,int> um;
  for (auto c: str) {
    if (um.count(c) == 0) {
      um[c] = 0;
    }
    um[c]++;
  }

  vector<pair<char,int>> v(um.begin(), um.end());
  sort(v.begin(), v.end(), [](const auto& v1, const auto& v2) {
    return (v1.second == v2.second) ? v1.first < v2.first : v1.second > v2.second;
  });

  string r, tmp;
  for (const auto& e: v) {
    r += tmp.assign(e.second, e.first);
  }
  
  return r;
}

string doit(const string& in) {
   unordered_map<char,int> um;
   for (auto e: in) {
      if (um.count(e) == 0) {
         um[e] += 1;
      } else {
         um[e] = 1;
      }
   }
   
   vector<pair<char,int>> v(um.begin(), um.end());
   for (auto e: v) {
      cout << "v: " << e.first << ", " << e.second << endl;
   }
   
   sort(v.begin(), v.end(), [](const pair<char,int>& a1, const pair<char,int>& a2) {
      return (a1.second == a2.second) ? a1.first < a2.first: a1.second > a2.second;
   });
   
   string r(in.size(), 'a'),tmp;
   for (auto e: v) {
      tmp.assign(e.second, e.first);
      cout << "tmp is: " << tmp << endl;
   }
   cout << "r is: " << r << endl;
   return r;
}


/*
 A Candy Tree contains White and Red nodes arranged thus:
 Every node has exactly two children.
 The left child of a White node is White and the right child is Red.
 The left child of a Red node is Red and the right child is White.
 For example, expanded to 5 levels:
                   W
         W                   R
    W         R         R         W
  W    R    R    W    R    W    W    R
 W R  R W  R W  W R  R W  W R  W R  R W


 isWhite(true, 3, 6)

 3

 2**3-1

 isWhite(2**3-1, 3, 6);

 bool isWhite(bool isWhite, int totalNodes, int row, int col) {
    indexOfSoughtNode = 2**row + col;
    if (totalNodes/2 < indexOfSoughtNode) {
       return isWhite(!isWhite, totalNodes/2, row-1, (2**(row-1)+(col/2));
    }

 }

 bool isWhite(bool isWhite, int row, int col) {
    if (row == 0) return isWhite;

    int nodes = 2**(row) + (col+1)
    int tnodes = (2**row)
    if (nodes
    
    for




 Index of level
 Index of node

 [2, 3]

 W WR W RRW WR RW





 Question 1

 struct Node {
    bool isWhite:
    Node* left;
    Node* right;
 };

 Node* createTree(int level, bool isWhite) {
    if (level == 0) return nullptr;
    Node* n = new Node();
    isWhite = isWhite;
       n.left = createTree(level-1, isWhite);
       n.right = createTree(level-1, !isWhite);
 return n;
 }


 */
