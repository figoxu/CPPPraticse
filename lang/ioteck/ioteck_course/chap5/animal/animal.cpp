#include "animal.h"

#include <iostream>
using namespace::std;

namespace iotek{
Animal::Animal(int age,string location)
	:m_age(age),m_location(location)
{
	cout << "Animal constructing" << endl;
}

Animal::~Animal()
{
	cout << "Animal destructing" << endl;
}

int Animal::getAge() const
{
	return m_age;
}

void Animal::setAge(int age)
{
	m_age = age;
}

Cat::Cat(int age, int color,string location)
	:Animal(age,location),m_color(color)
{
	cout << "Cat constructing" << endl;
}

Cat::~Cat()
{
	cout << "Cat destructing" << endl;
}

int Cat::getColor() const
{
	return m_color;
}

void Cat::setColor(int color)
{
	m_color = color;
}


Dog::Dog(int age, int weight,string location)
	:Animal(age,location),m_weight(weight)
{
	cout << "Dog constructing" << endl;
}

Dog::~Dog()
{
	cout << "Dog destructing" << endl; 
}

int Dog::getWeight() const
{
	return m_weight;
}

void Dog::setWeight(int weight)
{
	m_weight = weight;
}

}
