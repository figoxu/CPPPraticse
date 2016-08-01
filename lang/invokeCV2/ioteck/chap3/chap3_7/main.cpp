#include <iostream>
using namespace::std;

#include "date.h"

TDate d(11,11,2010);

static void foo()
{
	cout << __LINE__ << endl;
	static TDate ds(12,12,2010);
	ds.Print();
	cout << __LINE__ << endl;
}

static void boo()
{
	cout << __LINE__ << endl;
	TDate mm;
	cout << __LINE__ << endl;
}

int main(int argc, char *argv[])
{
	cout << __LINE__ << endl;
/*
	foo();
	cout << __LINE__ << endl;
	foo();
*/
	boo();
/*
	TDate date;
	cout << __LINE__ << endl;
	date.Print();

	cout << __LINE__ << endl;
	TDate *pDate = new TDate;
	cout << __LINE__ << endl;
	pDate->Print();
	cout << __LINE__ << endl;
	delete pDate;
	cout << __LINE__ << endl;
	pDate = NULL;

	cout << __LINE__ << endl;
	TDate &rDate = date;
	date.Set(10,1,2009);
	date.Print();
	rDate.Print();

	{
		int i = 0;
		cout << __LINE__ << endl;
		TDate date1(1);
		date1.Print();
	}
	
	cout << __LINE__ << endl;
	TDate date2(1,1);
	date2.Print();
	
	cout << __LINE__ << endl;
	TDate date3(1,1,2011);
	date3.Print();
*/
	cout << __LINE__ << endl;


	return 0;
}

