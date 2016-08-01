#include <iostream>
using namespace::std;

#include "date.h"
using namespace::TESTDate;

void func(TDate *date){
	cout << __func__ << endl;
	date->Set(2,28,1999);
	date->Print();	
}
void bar(TDate &date){
	cout << __func__ << endl;
	date.Set(8,1,2012);
	date.Print();
}
void foo(TDate date){
	date.Set(7,31,2012);
	date.Print();
}
int main(int argc, char *argv[]){
	TDate date;

	date.Print();

	date.Set(2,1,1999);
	date.Print();
	func(&date);
	date.Print();
	cout << "return from func" << endl;
	bar(date);
	date.Print();
	cout << "return from bar" << endl;
	foo(date);
	date.Print();


	TDate *pDate = new TDate;
	pDate->Set(10,15,2010);
	pDate->Print();
	delete pDate;
	pDate = NULL;
	if(pDate != NULL){
		pDate->Print();
		cout << "pDate is still not 0\n";
		cout << pDate << endl;
	}

	TDate &rDate = date;
	date.Set(10,1,2009);
	date.Print();
	rDate.Print();

	return 0;
}

