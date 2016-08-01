//overload
#include <iostream>
using namespace::std;


int square(int x)
{
	cout << __FILE__ << __LINE__ << __func__ << endl;
	return x*x;
}

float square(float x)
{
	cout << __FILE__ << __LINE__ << __func__ << endl;
	return x*x;
}

double square(double x)
{
	cout << __FILE__ << __LINE__ << __func__ << endl;
	return x*x;
}

int main(int argc, char *argv[])
{
	cout << "square(10)		" << square(10) << endl;
	cout << "square(2.5f)	" << square(2.5f) << endl;
	cout << "square(1.1)	" << square(1.1) << endl;

	return 0;
}
