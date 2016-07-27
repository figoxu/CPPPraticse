#include <iostream>

extern "C" {

#include <Hello.h>
#include <World.h>

}
int main() {
    std::cout << "Hello, World!" << std::endl;
    printHello();
    std::cout << std::endl;
    printHello2(100);

    std::cout << "Total : " << sum(11,12) << std::endl;
    printSum(15,16);
    return 0;
}