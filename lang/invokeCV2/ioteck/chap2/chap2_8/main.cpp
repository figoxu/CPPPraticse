#include <iostream>
using namespace::std;

void foo(int i, int j=5, int k=10);
//void foo(int , int = 5, int = 10);

int main(int argc, char *argv[])
{
	foo(20);
	foo(20,30);
	foo(20,30,40);
	
	return 0;
}

void foo(int i, int j, int k)
{
	cout << i << "  " << j << "  " << k << endl;
}
