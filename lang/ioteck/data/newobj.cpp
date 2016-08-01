#include <stdio.h>
#include <stdlib.h>

class A{
	public:
		A() {printf("A created \n");}
		~A() {printf("A destroyed 0x%x\n",this);}
};

A* CreateA(void)
{
	A *p = new A();
	return p;
}

int main(int argc, char *argv[])
{
	A *pA = new A();
	printf("pA = 0x%x\n",pA);
	delete pA;

	pA = CreateA();
	printf("pA = 0x%x\n",pA);
	delete pA;

	return 0;
}
