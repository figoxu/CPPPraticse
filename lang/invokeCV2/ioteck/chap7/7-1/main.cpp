#include <iostream>
#include <string>
using namespace::std;

#include "example.h"
using namespace::iotek;

int main()
{
	Example<int> a(19);
	int val = a.getVal();
	cout << val << endl;

	Example<string> b("hello");

	cout << b.getVal() << endl;

	Test test;
	Example<Test> c(test);

	return 0;
}
