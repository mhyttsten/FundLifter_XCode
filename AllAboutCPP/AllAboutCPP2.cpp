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

int main(int argc, char* argv[]) {
    Hello h { "a", "b" };
    
    vector<char> v01(2, 'c');
    cout << "hello: " << v01 << endl;
}
