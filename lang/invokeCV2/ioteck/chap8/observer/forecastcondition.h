#ifndef FORECAST_CONDITION_H_
#define FORECAST_CONDITION_H_

#include <iostream>
using namespace::std;

#include "observer.h"

class ForecastCondition : public Observer{
public:
	ForecastCondition()
		:m_currentPressure(0.0),m_lastPressure(0.0)
	{
	}

	~ForecastCondition() {}

	void update(float temperature, float humidity, float pressure)
	{
		m_lastPressure = m_currentPressure;
		m_currentPressure = pressure;
		display();
	}

	void display(){
		cout.setf(ios::showpoint);
		cout.precision(3);
		if(m_currentPressure > m_lastPressure){
			cout << "Improving weather on the way!";
		}else if(m_currentPressure < m_lastPressure){
			cout << "Watch out for cooler, rainy weather" ;
		}else{
			cout << "More of the same"; 
		}
		cout << endl;
	}
private:
	float m_currentPressure;
	float m_lastPressure;

	ForecastCondition(const ForecastCondition&);
	ForecastCondition& operator=(const ForecastCondition&);
};


#endif
