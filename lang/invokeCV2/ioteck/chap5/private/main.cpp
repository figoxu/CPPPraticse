#include <iostream>
using namespace::std;

class Animal{
public:
	Animal() {}
	void eat() const {cout << "eat" << endl;}
};

class Cat : public Animal
{
public:
	Cat() {}
	void maew() { cout << "maew" << endl; }
};

class Duck : private Animal
{
public:
	Duck() {}
	void stretchNeck() {cout << "stretch" << endl;} 
};

void func(const Animal& animal)
{
	animal.eat();
}

int main()
{
	Cat kitty;
	func(kitty);

	Duck duck;
	func(duck);

	return 0;
}
