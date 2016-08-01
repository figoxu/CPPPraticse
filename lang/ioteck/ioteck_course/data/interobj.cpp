#include <stdio.h>

class A
{
	public:
		A() { printf("A created\n"); }
		~A() {printf("A destroyed\n");}

		A(const A& a) {printf("A created with a copy\n");}
};

A *foo(A a)
{
	printf("foo----------->\n");
	A *p = new A();
	printf("foo<----------------\n");
	return p;
}

A *boo(const A& a){
	A *p = new A();
	printf("boo<----------------\n");
	return p;
}



int main(int argc, char *argv[])
{
	printf("main()---------------->\n");
	A a;

	A *p = foo(a);

	delete p;
	p=NULL;
	
	p = boo(a);
	delete p;
	p = NULL;

	printf("main()<----------------\n");
	return 0;
}
