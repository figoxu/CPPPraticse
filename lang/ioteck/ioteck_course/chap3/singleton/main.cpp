#include "singleton.h"


#include <stdio.h>

int main()
{
	Singleton::getInstance()->doSomething();

/*
	Singleton* val = new Singleton();
	delete val;
	val = NULL;
*/

	return 0;
}
