#include "currentcondition.h"
#include "forecastcondition.h"
#include "weatherdata.h"

int main()
{
	CurrentCondition current;
	ForecastCondition forecast;
	WeatherData weatherdata;

	weatherdata.registerObserver(&current);
	weatherdata.registerObserver(&forecast);

	weatherdata.setMeasurements(14,65,30.4f);
	weatherdata.setMeasurements(19,70,29.2f);
	weatherdata.setMeasurements(12,90,29.2f);

	return 0;
}
