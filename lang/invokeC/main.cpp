#include <iostream>


extern "C" {

#include <Hello.h>

}

int main() {
    std::cout << "Hello, World!" << std::endl;
    printHello();
    std::cout << std::endl;
    printHello2(100);
    return 0;
}