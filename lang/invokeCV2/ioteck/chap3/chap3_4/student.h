#ifndef _STUDENT_H_
#define _STUDENT_H_

class Student
{
public:
	void SetInfo(char *name,int id,char sex,int age);
	void setGrade(float grade1,float grade2,float grade3);
	
	float getGpa();
	float getGpa(float newGpa);

	void Print();
private:
	char name[20];
	//char *name;

	int id;
	char sex;
	int age;
	float grade[3];
	float gpa;
};

#endif

