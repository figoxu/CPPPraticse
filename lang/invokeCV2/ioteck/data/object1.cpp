#include <stdio.h>
#include <stdlib.h>

class A{
public:
	A() { printf("A created\n"); }
	~A() { printf("A destroyed\n"); }
};

class B{
public:
	B() { printf("B created\n"); }
	~B() { printf("B destroyed\n"); }
};

A globalA;

B globalB;

int foo(void)
{
	printf("\nfoo()------------------------->\n");
	A localA;
	static B localB;
	printf("foo()<----------------------------\n");
	return 0;
}

int main(int argc, char *argv[])
{
	printf("main()------------------------>\n");
	foo();
	foo();
	printf("main()<---------------------------\n");
	return 0;
}
	

