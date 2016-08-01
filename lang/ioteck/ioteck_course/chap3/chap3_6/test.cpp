#include <iostream>
using namespace::std;

int val = 10;

int main()
{
	int val = 100;
	::val++;
	val++;
	cout << ::val << endl;
	cout << val << endl;
	
	return 0;
}

