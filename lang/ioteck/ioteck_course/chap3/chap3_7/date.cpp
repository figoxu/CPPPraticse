#include <iostream>
using namespace::std;

#include "date.h"

TDate::TDate()
	:month(1),day(1),year(2010),i(100)
{
/*
	month = 1;
	day = 1;
	year = 2010;
*/
	cout << "constructor TDate()"<<endl;
}

TDate::TDate(int m)
	:month(m),day(1),year(2009),i(100)
{
/*
	month = m;
	day = 1;
	year = 2009;
*/
	cout << "constructor TDate(int)"<<endl;
}

TDate::TDate(int m, int d)
	:i(100)
{
	month = m;
	day = d;
	year = 2010;
	cout << "constructor TDate(int,int)"<<endl;
}

TDate::TDate(int m, int d, int y)
	:i(100)
{
	month = m;
	day = d;
	year = y;
	cout << "constructor TDate(int,int,int)"<<endl;
}

TDate::~TDate()
{
	cout << "destructor of TDate" << endl;
}

void TDate::Set(int m, int d, int y)
{
	month = m;
	day = d;
	year = y;
}

bool TDate::IsLeapYear()
{
	return (((year%4 == 0) && (year%100!=0))||(year%400 == 0));
}

void TDate::Print()
{
	cout << month << "/" << day << "/" << year << endl;
}
