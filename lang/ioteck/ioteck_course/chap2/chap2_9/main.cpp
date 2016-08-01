#include <iostream>
using namespace::std;

long labs(long a)
{
	cout << __func__ << __LINE__<< endl;
}

double labs(double a)
{
	cout << __func__ << __LINE__<< endl;
}

void fun(int a)
{
	labs(a);
}

int main()
{
	labs(0.05);
	return 0;
}
