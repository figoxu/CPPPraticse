#ifndef _HIFI_H_
#define _HIFI_H_

#include <iostream>
using namespace::std;


class Recorder{
public:
	Recorder(int i) 
	{ 
		cout << "constructing recorder\n";		
	}

	~Recorder()
	{
		cout << "desctructing recorder\n";
	}

};

class Hifi{
public:
	Hifi()
	{
		cout << "constructing hifi\n";		
		recorder = new Recorder(1);	
	}

	~Hifi()
	{
		cout << "deconstructing hifi\n";		
		delete recorder;
		recorder = NULL;
	}

private:
	Recorder *recorder;
	int i;

};


#endif
