#include <iostream>
using namespace::std;

#include "martain.h"

void func()
{
	Martain c;
	int count = Martain::getCount();
	cout << "count = " << count << endl;

}

int main(int argc, char *argv[])
{
	int count = Martain::getCount();
	cout << "count = " << count << endl;

	Martain a;
	count = Martain::getCount();
	cout << "count = " << count << endl;

	Martain b;
	count = Martain::getCount();
	cout << "count = " << count << endl;
	func();
	count = Martain::getCount();
	cout << "count = " << count << endl;

	return 0;
}

