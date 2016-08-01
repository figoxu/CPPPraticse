#include <iostream>
using namespace::std;

#include "student.h"


#include <cstring>
#include <cstdio>

Student::Student(const char *pName,int sId)
	:id(sId)
{
	memset(name,0,40);
	if(pName != NULL){
		int len = strlen(pName);
		len = (len > 39)?39:len;
		strncpy(name,pName,len);
	}
	cout << "constructor of student:" << name << endl;	
}

Student::~Student()
{
	cout << "destructor of student:"<< name << endl;
}

void Student::print()
{
	cout << "Student: " << name << endl;
}

Student::Student(const Student &s)
{
	cout << "Constructing copy ! " << endl;
	printf("&s=%x\n",&s);
	strcpy(name,"copy");//joy
	strcat(name,s.name);//copyjoy
	id = s.id;
}
