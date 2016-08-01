#include <iostream>
using namespace::std;

#include "student.h"

void fun(Student stu)
{
	stu.setId(1003);	
}

void bar(const Student& stu)
{
	//stu.setId(1003);	
	int id = stu.getId();
}

int main(int argc, char *argv[])
{
	{
		int i = 0;
		Student john(1001);
		cout << "john's id " << john.getId() << endl;
		fun(john);
		cout << "john's id " << john.getId() << endl;
	}
	cout << "return from main" << endl;
	return 0;
}
