#include <iostream>
using namespace::std;
#include <stdio.h>

#include "student.h"

void foo(Student stu)
{
	cout << "In fun " << __func__  << endl;
	printf("&stu=%x\n",&stu);
}

Student bar()
{
	Student tom("tom",112);
	return tom;
}

void zoo(int val)
{
	printf("&val=%x\n",&val);
	val = 100;
}

int main(int argc, char *argv[])
{
	int i = 0;
	int j = i;
	printf("&j=%x\n",&j);
	zoo(j);
	cout << "j = " << j << endl;

	cout << "Enter main---->\n"; 
	Student joe("Joe",111);
	Student john = joe;
	
	cout << "Calling foo" << endl;
	printf("&joe=%x\n",&joe);
	foo(joe);
	cout << "After call of foo" << endl;

	Student tom = bar();

	return 0;
}

