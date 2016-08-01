#include <iostream>
using namespace::std;

void swap(int a, int b)
{
	cout << "enter" << __func__ << endl;
	cout << "&a=" << &a << endl;
	cout << "&b=" << &b << endl;
	int temp;
	temp = a;
	a = b;
	b = temp;
	cout << "leave" << __func__ << endl;
}

void swap_pointer(int *a, int *b)
{
	cout << "a=" << a << endl;
	cout << "b=" << b << endl;
	int temp = *a;
	*a = *b;
	*b = temp;
}

void swap_reference(int& a, int& b)
{
	cout << "enter" << __func__ << endl;
	cout << "&a=" << &a << endl;
	cout << "&b=" << &b << endl;
	int temp;
	temp = a;
	a = b;
	b = temp;
	cout << "leave" << __func__ << endl;
}

int func(const int* a, int *b)
{
	//*a = 100;
	*b = 10000;
	
	int c = 0;
	//a = &c;
	const int d = 100;
	
	d =10;

	return 1;
}

int funcb(int &a, int b)
{
	a = 100;
	b = 10000;

	return 2;
}

int main(int argc, char *argv[])
{
	int x = 3;
	int y = 4;

	int z = func(&x, &y);

	z = funcb(x,y);

	cout << x << endl;
	cout << y << endl;
	cout << z << endl;
	
	cout << "&x=" << &x << endl;
	cout << "&y=" << &y << endl;
	
	swap(x,y);

	cout << "x = " << x << endl;
	cout << "y = " << y << endl;

	swap_pointer(&x,&y);

	cout << "x = " << x << endl;
	cout << "y = " << y << endl;
	
	swap_reference(x,y);

	cout << "x = " << x << endl;
	cout << "y = " << y << endl;

	return 0;
}
