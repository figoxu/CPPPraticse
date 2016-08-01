#ifndef _PERSON_H_
#define _PERSON_H_

class Person
{
public:
	Person(char *pName);
	~Person();

	Person(const Person &s);
	Person& operator=(const Person& other);

	void Print();
private:

	char *name;
};

#endif
