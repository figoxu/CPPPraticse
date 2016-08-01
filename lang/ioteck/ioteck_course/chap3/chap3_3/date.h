#ifndef _DATE_H_
#define _DATE_H_

namespace TESTDate{

class TDate
{
public:
	TDate()
	{
		month = 8;
		day = 1;
		year = 2012;
	}
	void Set(int m, int d, int y);

	bool IsLeapYear();
	
	void Print() const;

private:
	int month;
	int day;
	int year;
};

}

#endif
