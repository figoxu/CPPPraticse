#ifndef _CURRENT_CONDITION_H
#define _CURRENT_CONDITION_H

#include <iostream>
using namespace::std;

#include "observer.h"

class CurrentCondition : public Observer{
public:
	CurrentCondition()
		:m_temperature(0.0),m_humidity(0.0)
	{
		
	}

	~CurrentCondition() {}

	void update(float temperature,float humidity, float pressure)
	{
		m_temperature = temperature;
		m_humidity = humidity;
		display();
	}

	void display()
	{
		cout.setf(ios::showpoint);
		cout.precision(3);
		cout << "Current condition " << m_temperature;
		cout << "C,and " << m_humidity << "% humidity" << endl;
	}
private:
	
	float m_temperature;
	float m_humidity;

	CurrentCondition(const CurrentCondition&);
	CurrentCondition& operator=(const CurrentCondition&);
};

#endif
