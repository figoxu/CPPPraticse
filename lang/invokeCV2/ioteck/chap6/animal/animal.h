#ifndef _ANIMAL_H_
#define _ANIMAL_H_

#include <iostream>
using namespace::std;

class Animal{
public:
	Animal() {
		cout << "Animal constructin" << endl;	
	}
	virtual ~Animal() {
		cout << "Animal destructing" << endl;	
	}

	virtual void makeSound() const = 0; //pure virtual function
	/*
	virtual void makeSound() const {
		cout << "Animal make sound" << endl;
	}
	*/
private:
	int m_age;
};

class Dog : public Animal{
public:
	Dog() {
		cout << "Dog constructing" << endl;	
	}

	~Dog() {
		cout << "Dog destructing" << endl; 	
	}

	virtual void makeSound() const{
		cout << "Dog make sound" << endl; 
	}
	
};

class Cat : public Animal {
public:
	Cat(){
		cout << "Cat constructing" << endl;
	}

	~Cat() {
		cout << "Cat destructing" << endl;
	}

	virtual void makeSound() const{
		cout << "Cat make sound" << endl;
	}
};

#endif

