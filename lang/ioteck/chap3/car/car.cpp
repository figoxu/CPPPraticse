#include <iostream>
using namespace::std;

#include "car.h"

Car::Car(int price, int carNum)
{
	cout << "Car constructor" << endl;
	setProperty(price, carNum);
}

void Car::run()
{
	cout << "Car run" << endl;
}

void Car::stop()
{
	cout << "Car stop" << endl; 
}

void Car::setProperty(int price, int carNum)
{
	this->m_price = price;
	this->m_carNum = carNum;
}

void Car::print()
{
	cout << "This car price is " << m_price << endl;
	cout << "This car number is " << m_carNum << endl;

}

