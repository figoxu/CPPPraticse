#include <iostream>
using namespace::std;

int month;
int day;
int year;

void Set(int m, int d, int y)
{
	::month = m;
	::day = d;
	::year = y;
}

class TDate{
public:
	void Set(int m, int d, int y)
	{
		::Set(m,d,y);
	}
	void Print(){
		cout << month << "/" << day << "/" << year << "/" << static_cast<void *>(name) << endl;
	}
private:
	char *name;
	int month;
	int day;
	int year;
};

int main(int argc, char *argv[])
{
	TDate d;
	d.Set(10,2,1999);	
	cout << month << "/" << day << "/" << year << endl; //10/2/1999
	d.Print();//xxx/xxxx/xxxx
	return 0;	
}
