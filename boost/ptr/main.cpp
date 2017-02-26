#include <iostream>
#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>

using namespace std;

void scopedPtr(){
    boost::scoped_ptr<int> i(new int);
    *i = 1;
    *i.get()=2;
    i.reset(new int);
}

void scopedPtr_array(){
    boost::scoped_array<int> i(new int[2]);
    *i.get() = 1;
    i[1] = 2;
    i.reset(new int[3]);
}

int main() {
    scopedPtr();
    scopedPtr_array();
    cout << "Hello, World!" << endl;
    return 0;
}