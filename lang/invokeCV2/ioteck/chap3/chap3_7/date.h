#ifndef _DATE_H_
#define _DATE_H_

class TDate{
public:
	TDate();
	TDate(int m);
	TDate(int m, int d);
	TDate(int m, int d, int y);
	~TDate();

	void Set(int m, int d, int y);
	bool IsLeapYear();
	void Print();

private:
	int month;
	int day;
	int year;
	const int i;
};

#endif
