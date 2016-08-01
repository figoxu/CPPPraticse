#include <iostream>
using namespace::std;

#include "car.h"

int main()
{
	int i = 0;
	Car a(5000,888888);
	cout << "end of define Car a" << endl;
	a.print();
	a.setProperty(10000,123456);
	a.print();
	a.run();

	return 0;
}
