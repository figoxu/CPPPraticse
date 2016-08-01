#ifndef _DATE_H_
#define _DATE_H_

class Date{
public:
	Date(int=1, int=1, int=1990);//default constructor
	void print() const;//print date in month/day/year format
	~Date();
private:
	//utility function to test proper day for month and year
	int checkDay(int);
	bool isLeapYear();
	int month;
	int day;
	int year;
};
#endif
