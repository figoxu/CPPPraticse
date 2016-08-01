#include <stdio.h>
#include <stdlib.h>

int i = 0;


class A
{
public:
	int val;
	static int nCount;
	A() {++nCount;};
	~A() {--nCount;};
};


int A::nCount = 0;

int main(int argc, char *argv[])
{
	A a;
	A b;
	
	printf("Number of A is %d====>\n\n",A::nCount);
	printf("no static variable: 0x%x\n",&a.val);
	printf("no static variable: 0x%x\n",&b.val);
	printf("static class member:0x%x\n",&a.nCount);
	printf("static class member:0x%x\n",&b.nCount);
	printf("global member:0x%x\n",&i);
}
