#include <iostream>
using namespace::std;

class Student{
public:
	Student(){
		cout << "constructing student.\n";
		semesHours = 100;
		gpa = 3.5;
	}

	~Student(){
		cout << "destructing student\n";
	}
private:
	int semesHours;
	float gpa;
};

class Teacher
{
public:
	Teacher(){
		cout << "constructing teacher" << endl;
	}
	~Teacher(){
		cout << "destructing teacher" <<endl;
	}
};

class TutorPair{
public:
	TutorPair(){
		cout << "constructing tuorpair\n";
		noMeetings = 0;
	}	
	
	~TutorPair(){
		cout << "destructing tutorpair" << endl;
	}

private:
	Student student;
	Teacher teacher;
	int noMeetings;
};

int main()
{
	TutorPair pair;
	cout << "back in main" << endl;
	return 0;
}
