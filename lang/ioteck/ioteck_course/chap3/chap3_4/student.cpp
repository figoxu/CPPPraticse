#include <iostream>
using namespace::std;

#include <cstring>

#include "student.h"



void Student::SetInfo(char *na,int i,char s,int a)
{
	strcpy(name,na);
	id = i;
	sex = s;
	age = a;	
}

void Student::setGrade(float a, float b,float c)
{
	grade[0] = a;
	grade[1] = b;
	grade[2] = c;
	gpa = 0;
}

float Student::getGpa()
{
	gpa = (grade[0] + grade[1] + grade[2])/3;
	return gpa;	
}

float Student::getGpa(float newGpa)
{
	gpa = newGpa;
	return gpa;
}

void Student::Print()
{
	cout << "Student info" << endl;
}
