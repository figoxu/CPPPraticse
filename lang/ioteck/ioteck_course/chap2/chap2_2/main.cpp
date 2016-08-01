#include <iostream>
using namespace::std;

void foo(int val)
{
	val = 10;
}

void bar(int& val)
{
	val = 10;
}

void zoo(int* pval)
{
	*pval = 10;
}

int fun(int *b, int& c,int a)
{
	*b = 1;
	c = 5;
	return 1;
}

int main(int argc, char *argv[])
{
	int a = 1;
	int b = 1;
	int c = 1;
	foo(a);
	bar(b);
	zoo(&c);
	cout << a << " " << b << " " << c << endl;
	return 0;
}
