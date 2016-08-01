#include <iostream>
using namespace::std;

#include "date.h"

Date::Date(int m, int d, int y)
{
	if(m > 0 && m <= 12){
		month = m;
	}else{
		month = 1;
		cout << "Month " << m << "Invalid. Set month to 1." << endl; 
	}	

	year = y;
	
	day = checkDay(d);
	cout << "Date object constructor for date" ;
	print();
	cout << endl;
}

Date::~Date()
{
	cout << "destructor of date" << endl;
	print();
}

void Date::print() const
{
	cout << month << "/" << day << "/" << year << endl;
}

int Date::checkDay(int day)
{
	static const int daysPerMonth[13] =\
				{0,31,28,31,30,31,30,31,31,30,31,30,31};
	if(day > 0 && day <= daysPerMonth[month])
		return day;
	
	if(month == 2 && day == 29 && isLeapYear()) {
		return day;
	}
	
	return 1; 
}	

bool Date::isLeapYear()
{
	return (((year%4 == 0) && (year%100!=0))||(year%400 == 0));
}
