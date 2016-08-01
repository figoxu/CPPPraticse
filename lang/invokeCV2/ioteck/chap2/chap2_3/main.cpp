#include <iostream>
using namespace::std;

int main(int argc, char *argv[])
{
	int *pa = NULL;
	int*& pRa = pa;
	//cout << pa << endl;
	cout << "pa = " << static_cast<void *>( pa )<< endl;
	cout << "pRa = " << static_cast<void *>( pRa )<< endl;
	
	int b = 8;
	pRa = &b;
	
	cout << "pa = " << static_cast<void *>( pa )<< endl;
	cout << "pRa = " << static_cast<void *>( pRa )<< endl;
	cout << "&b = " << &b << endl;
	
	return 0;
}
