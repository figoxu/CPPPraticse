#ifndef _WEATHER_DATA_H_
#define _WEATHER_DATA_H_

#include <assert.h>

#include <list>
using namespace::std;

#include "subject.h"
#include "observer.h"

class WeatherData : public Subject{
public:
	WeatherData()
		:m_temperature(0.0),m_humidity(0.0),m_pressulre(0.0)
	{
	}

	~WeatherData(){
	}

	void registerObserver(Observer* o)
	{
		assert(o);
		m_observers.push_back(o);
	}

	void removeObserver(Observer *o)
	{
		m_observers.remove(o);
	}

	void notifyObserver() const {
		for(list<Observer*>::const_iterator it = m_observers.begin();
		it != m_observers.end();
		++it)
		{
			Observer * obj = *it;
			obj->update(m_temperature,m_humidity,m_pressulre);
		}
	}

	void measurementsChanged()
	{
		notifyObserver();
	}
	
	void setMeasurements(float temperature, float humitity, float pressure){
		m_temperature = temperature;
		m_humidity = humitity;
		m_pressulre = pressure;
		measurementsChanged();
	}


private:
	float m_temperature;
	float m_humidity;
	float m_pressulre;
	
	list<Observer*> m_observers;

private:
	WeatherData(const WeatherData&);
	WeatherData& operator=(const WeatherData&);
};

#endif
