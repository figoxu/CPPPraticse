#include <iostream>
using namespace::std;

double& foo(const double &pa)
{
	static double ad = 3.0;
	cout << "ad = " << ad << endl;
	ad += pa;
	cout << "fpp called, the value is " << ad << endl;
	//pa = 333;	
	return ad;
}

int main(int argc, char *argv[])
{
	double a = 3.6;
	double& pa = foo(a);
	cout << pa << endl;
	
	a = 5.4;
	pa = foo(a);
	cout << pa << endl;
	
	pa = 5.4;
	pa = foo(pa);
	cout << pa << endl;
}

