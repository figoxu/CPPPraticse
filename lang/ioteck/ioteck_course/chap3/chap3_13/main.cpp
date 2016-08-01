#include <iostream>
using namespace::std;

#include "rect.h"

int main()
{
	Rect r1, r2;
	r1.set(10,10,10,10);
	r2.set(200,200,15,25);
	
	cout << r1.area() << endl;
	cout << r2.area() << endl;
}

