#include <iostream>
using namespace::std;

int main(int argc , char *argv[])
{
	int integer1 = 0;

	cout << "Enter the first integer:\n";
	cin >> integer1;

	int integer2=0,sum=0;

	cout << "Enter the second integer:\n";
	cin >> integer2;

	sum = integer1 + integer2;
	cout << "Sum is " << sum << endl;

	return 0;
}
