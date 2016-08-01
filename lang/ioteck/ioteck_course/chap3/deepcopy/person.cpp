#include <iostream>
using namespace::std;

#include <string.h>

#include "person.h"

Person::Person(char *pN)
{
	if(pN != NULL){
		cout << "Constructing" << pN << endl;
		int len = strlen(pN) + 1;
		name = new char[len]; //name = malloc();
		cout << "name = " << static_cast<void *>(name) << endl;
		memset(name,0,len);	
		strcpy(name,pN);
	}else{
		name = NULL;
	}	
}

Person::~Person()
{
	cout << "Destructing Person--->\n" << endl;
	if(name != NULL){
		Print();
		delete []name;
		name = NULL;
	}	
}
Person::Person(const Person &p)
{
	cout << "Copy constructor of Person" << endl;
	if(p.name != NULL){
		int len = strlen(p.name) + 1;
		name = new char[len];
		cout << "name = " << static_cast<void *>(name) << endl;
		memset(name,0,len);
		strcpy(name,p.name);
	}else{
		name = NULL;
	}	
}

Person& Person::operator=(const Person &other)
{

	cout << "operator = " << endl;
	if(&other == this)
	{
		return *this;
	}

	if(name != NULL){
		delete []name;
		name = NULL;
	}
	
	if(other.name != NULL){
		int len = strlen(other.name) + 1;
		name = new char[len];
		memset(name,0,len);
		strcpy(name,other.name);
	}else{
		name = NULL;
	}

	return *this;
}

void Person::Print()
{
	cout << "pName = " << static_cast<void *>(name) << endl;
}
