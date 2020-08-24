//
//  AllAboutCPP.cpp
//  FundLifter
//
//  Created by Magnus Hyttsten on 8/10/20.
//  Copyright © 2020 Magnus Hyttsten. All rights reserved.
//

#include <complex>
#include <filesystem>
#include <fstream>
#include <iostream>  // Needed for std::cout etc
#include <istream>
#include <list>
#include <map>
#include <ostream>
#include <sstream>
#include <regex>
#include <stdio.h>
#include <vector>
#include <variant>

int main(int argc, char* argv[]) {
    
  std::cout << "Hello World" << std::endl;
  
  // Chapter 1: The Basics
  {
    std::cout << "*** Starting Chapter 1" << std::endl;
    // Built in types: int, bool (true, false), char, double
    
    int i0 = 3;
    auto i1 = 3;  // Creates int
    int i2 = 3.14;  // Implicitly truncates 3.14
    // int i3 = { 3.14 };  // Gives error
    // int i4 { 3.14 };
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

  // Chapter 2: User-defined types
  {
    std::cout << "*** Starting Chapter 2" << std::endl;
    
    using namespace std;
    
    class A {
    public:
      A(int a): _a{a} {}  // Use member initialization instead of assigning in constructor
      int& operator[](int index) { return _a; }
      int _a;
    };
    A a(5); a[0] = 10; cout << "Value of _a: " << a._a << endl;
    
    // U is a point to memory, refer to that memory thorugh one of these members
    union U {
      int size;
      char string[];
    };

    // Don't use unions though, use variant, need #include <variant>
    // TODO: For some reason below does not complie
    // std::variant<int, float> v;  // v can hold either an int or float
    // int i01 = std::get<int>(v);
    // int i01 = std::get<0>(v);  // Same as get<int>
    // assert(std::holds_alternative<int>(v));
    // try { std::get<float>(v); }
    // catch (const std::bad_variant_access&) { }
        
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
  
  // Chapter 3: Modularity
  {
    std::cout << "*** Starting Chapter 3" << std::endl;
    
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
  
  // Chapter 4: Classes
  {
    std::cout << "*** Starting Chapter 4" << std::endl;
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
  
  // Chapter 5: Essential Operations
  {
    std::cout << "*** Starting Chapter 5" << std::endl;
    using namespace std;
    
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
    X2 x20 = X2();
    cout << endl << "Test 1" << endl;
    X2 x21 = X2::foo1(x20);
    cout << endl << "Test 2" << endl;
    x21 = X2::foo1(x20);
    
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
  
  void chapter06(); chapter06();
  void chapter07(); chapter07();
  void chapter08(); chapter08();
  void chapter09(); chapter09();
  void chapter10(); chapter10();
  void chapter11(); chapter11();
  void chapter12(); chapter12();
  void chapter13(); chapter13();
  void chapter14(); chapter14();
  void chapter15(); chapter15();
  void chapter16(); chapter16();
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
  std::cout << "*** Starting Chapter 6" << std::endl;
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
  
  /*
   Header files and what they contain
   All these symbols are in std namespace
   
   <algorithm>     copy(), find(), sort()
   <array>         array
   <chrono>        duration, time_point
   <cmath>         sqrt(), pow()
   <complex>       complex, sqrt(), pow()
   <filesystem>    path
   <forward_list>  forward_list
   <fstream>       fstream, ifstream, ofstream
   <future>        future, promise
   <ios>           hex, dec, scientific, fixed, defaultfloat
   <iostream>      istream, ostream, cin, cout
   <map>           map, multimap
   <memory>        unique_ptr, shared_ptr, allocator
   <random>        default_random_engine, normal_distribution
   <regex>         regex, smatch
   <string>        string, basic_string
   <set>           set, multiset
   <sstream>       istringstream, ostringstream
   <stdexcept>     length_error, out_of_range, runtime_error
   <thread>        thread
   <unordered_map> unordered_map, unordered_multimap
   <utility>       move(), swap(), pair
   <variant>       variant
   <vector>        vector
   
   Also, all C-headers have C++ versions witha name convention: <stdlib.h> -> <cstdlib>
   This places all its declarations in the std namespace
   */
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
  std::cout << "*** Starting Chapter 09" << std::endl;
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
  cout << s01 << endl;
  cout << s01.substr(0,2) << endl;

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
  regex re01{"\\w{2}\\s*\\d{5}(-\\d{4})?)"};   // US post code: State(2), any number of spaces 5 digits, optionally -4digits
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
  cout << "hello" << endl;  // Standard output stream
  cerr << "error" << endl;  // Standard error stream
  
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
  path p = "tmp/cpptemporaryfile";  // Path can be implicitly converted to a path
  cout << "Exists: " << exists(p) << endl;
  ofstream pofs {p};
  // Traversing directory structure
  try {
    if (is_directory(p)) {
      for (const directory_entry& x: directory_iterator{p})
        cout << x.path() << endl;
    }
  } catch (const filesystem_error& ex) { }
  
  
}

// ***************** Chapter 11
void chapter11() {
  std::cout << "*** Starting Chapter 11" << std::endl;
  using namespace std;
}

// ***************** Chapter 12
void chapter12() {
  std::cout << "*** Starting Chapter 12" << std::endl;
  using namespace std;
}

// ***************** Chapter 13
void chapter13() {
  std::cout << "*** Starting Chapter 13" << std::endl;
  using namespace std;
}

// ***************** Chapter 14
void chapter14() {
  std::cout << "*** Starting Chapter 14" << std::endl;
  using namespace std;
}

// ***************** Chapter 15
void chapter15() {
  std::cout << "*** Starting Chapter 15" << std::endl;
  using namespace std;
}

// ***************** Chapter 16
void chapter16() {
  std::cout << "*** Starting Chapter 16" << std::endl;
  using namespace std;
}



