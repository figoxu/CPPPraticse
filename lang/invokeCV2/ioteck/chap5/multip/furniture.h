#ifndef _FURNITURE_H_
#define _FURNITURE_H_

#include <iostream>
using namespace::std;

class Furniture{
public:
	Furniture(int weight)
	{
		cout << "Furniture constructing" << endl;
		m_weight = weight;
	}

	void setWeight(int weight){
		m_weight = weight;
	}

	int getWeight() const
	{
		return m_weight;
	}
private:
	int m_weight;
};


class Sofa :virtual public Furniture{
public:
	Sofa(int weight=0);
	void watchTV() {
		cout << "watch TV" << endl; 
	}
};


class Bed:virtual public Furniture{
public:
	Bed(int weight=0);
	void sleep() {cout << "sleep" << endl;}
};


class SofaBed:public Sofa, public Bed{
public:
	SofaBed(int weight);
	void foldout() {
		cout << "fold out" << endl;
	}
};

#endif

