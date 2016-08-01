#ifndef _EMPLOYEE_H_
#define _EMPLOYEE_H_
#include "date.h"
class Employee{
public:
	Employee(char *firstname,char *lastname,int,int,int,int,int,int);
	void print() const;
	~Employee();

private:
	char firstName[32];
	char lastName[32];
	const Date hireDate;
	const Date birthDate;
};
#endif
