#include <iostream>

using namespace std;

typedef string music;
enum cities {
    GuangZhou = 4,
    ShenZhen = 3,
    ShangHai = 2,
    BeiJing = 1
};

int main() {
    cout << "Size Of Char : " << sizeof(char) << endl;
    cout << "Size Of Int  : " << sizeof(int) << endl;
    cout << "Size of short int : " << sizeof(short int) << endl;
    cout << "Size of long int : " << sizeof(long int) << endl;
    cout << "Size of float : " << sizeof(float) << endl;
    cout << "Size of double : " << sizeof(double) << endl;
    cout << "Size of wchar_t : " << sizeof(wchar_t) << endl;

    cities c;
    c = ShangHai;
    cout << "the cities value is : " << c << endl;


    music mp3;
    mp3 = "http://www.baidu.com/music.mp3";
    cout << "mp3 is : " << mp3 << endl;

    return 0;
}