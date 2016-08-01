#include "animal.h"



void func(Animal *panimal)
{
	panimal->makeSound();
}

int main()
{
	Animal *pcat = new Cat;
	pcat->makeSound();

	func(pcat);

	delete pcat;

	return 0;
}
