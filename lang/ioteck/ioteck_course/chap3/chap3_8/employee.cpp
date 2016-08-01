#include <iostream>
using namespace::std;

#include <string.h>

#include "employee.h"

Employee::Employee(char *fname, char *lname, int bm,int bd, int by, int hm, int hd,int hy)	
	:birthDate(bm,bd,by),hireDate(hm,hd,hy)
{
	int len = strlen(fname);
	len = (len < 31) ? len : 31;
	memset(firstName,0,32);
	strncpy(firstName,fname,len);
	
	len = strlen(lname);
	len = (len < 31) ? len : 31;
	memset(lastName,0,32);
	strncpy(lastName,lname,len);	
	
	cout << "Employee object constructor" << firstName << " " << lastName << endl;
	
}

void Employee::print() const
{
	cout << firstName << "  " << lastName << endl;
	cout << "Hired: ";
	hireDate.print();
	cout << "Birth Date: "; 
	birthDate.print();
}

Employee::~Employee()
{
	cout << "destructor of " << firstName << " " << lastName << endl;
}


