#include "singleton.h"

#include <iostream>
using namespace::std;

Singleton* Singleton::instance = NULL;

Singleton::Singleton()
{
	cout << "Singleton instance" << endl;
}

Singleton::~Singleton()
{
}

void Singleton::destroy()
{
	delete this;
	instance = NULL;
}

//not thread safe,using pthread_mutex_lock/unlock
Singleton* Singleton::getInstance()
{
	Singleton* ret = instance;
	if(instance == NULL){
		instance = new Singleton();
		ret = instance;
	}

	return ret;
}

void Singleton::doSomething()
{
	cout << __func__ << "," << __LINE__ << endl;
}
