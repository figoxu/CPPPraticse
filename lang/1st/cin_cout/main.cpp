#include <iostream>

using namespace std;

int main() {
    char name[50];
    cout << "Please Input Your Name:";
    cin >> name;
    cout << "Your Name is :" << name << endl;
    clog << "Log Msg was Print" << endl;
    cerr << " error was occour" << endl;
    return 0;
}