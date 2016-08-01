#include <iostream>
using namespace::std;

#include <cstring>

class StudentID
{
public:
	StudentID(int = 0){
		cout << "before asign id " << value << endl;
		value = 0;
		cout << "Assigning student id " << value << endl;
	}
	
	void SetId(int id) 
	{
		value = id;
	}
	
	~StudentID(){
		cout << "Destructing id " << value << endl;
	}
private:
	int value;
};

class Student{
public:
	Student(char *pName,int ssId=0);
	~Student();
private:
	char name[20];
	StudentID id[2];
};

Student::Student(char *pName, int ssId)
{
	cout << "Constructing student " << pName << endl; 
	memset(name,0,20);
	strcpy(name,pName);
	id[1].SetId(1);
	id[0].SetId(2);

}

Student::~Student()
{
	cout << "destructing student" << endl;
}

int main()
{
	Student s("Randy",9891);
	Student t("Jenny");

	return 0;
}
