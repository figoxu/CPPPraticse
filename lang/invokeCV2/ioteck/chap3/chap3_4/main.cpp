#include <iostream>
using namespace::std;

#include "student.h"

int main(int argc, char *argv[])
{
	Student stu1;
/*
	stu1.SetInfo("Zhang Sanssssssssssssssss",2010011019,'m',18);
	stu1.SetInfo(NULL,2010011019,'m',18);
*/
	stu1.SetInfo("Zhang San",2010011019,'m',18);
	stu1.setGrade(79,98,87);
	
	float gpa = stu1.getGpa();
	cout << "GPA is" << gpa << endl;	

	gpa=stu1.getGpa(99);

	cout << "GPA is " << gpa << endl;	

	return 0;
}
