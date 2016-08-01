#include <iostream>
using namespace::std;

int main(int argc,char *argv[])
{
	int intOne = 0;
	//int& r_int;
	int& r_int_one = intOne;
	int j = 10;
	
	intOne = 5;
	
	cout << intOne << endl;
	cout << r_int_one << endl;
	
	r_int_one = 7;
	
	cout << intOne << endl;
	cout << r_int_one << endl;
	
	cout << &(intOne) << endl;
	cout << &(r_int_one) << endl;


	r_int_one = j;
	j = 9;

	cout << intOne << endl;
	cout << r_int_one << endl;
	cout << j << endl;
	
	return 0;
}

