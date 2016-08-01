#ifndef _ANIMAL_H_
#define _ANIMAL_H_

#include <string>
#include <iostream>
using namespace::std;

namespace iotek{

class Animal{
public:
	Animal(int age,string location);
	~Animal();

	void setAge(int age);
	int getAge() const;
	
	string getLocation() const
	{
		return m_location;
	}

protected:
	void setLocation(string location)
	{
		cout << "Animal setLocation" << endl;
	}

	string m_location;	

private:
	void doSomething() {}
	int m_age;
};


class Cat : public Animal{
public:
	Cat(int age, int color,string location);
	~Cat();
	
	int getColor() const;
	void setColor(int color);

	void setCatLocation(string location)
	{
		setAge(2);
	//	Animal::setLocation(location);		
	//	Animal::doSomething();	
		m_location = location;
	//	m_age = 0;
	}

private:
	int m_color;
};

class Dog : public Animal{
public:
	Dog(int age, int weight,string location);
	~Dog();

	int getWeight() const;
	void setWeight(int weight);

private:
	int m_weight;
};

}

#endif
