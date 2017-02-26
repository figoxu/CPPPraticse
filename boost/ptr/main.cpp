#include <iostream>
#include <boost/scoped_ptr.hpp>

using namespace std;

void scopedPtr(){
    boost::scoped_ptr<int> i(new int);
    *i = 1;
    *i.get()=2;
    i.reset(new int);
}

int main() {
    scopedPtr();
    cout << "Hello, World!" << endl;
    return 0;
}