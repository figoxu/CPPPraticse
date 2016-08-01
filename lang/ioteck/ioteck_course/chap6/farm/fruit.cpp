#include <iostream>
using namespace::std;

#include "fruit.h"

Fruit::~Fruit()
{
}

Apple::Apple()
{
	cout << "Apple construnting" << endl;
}

Apple::~Apple()
{
	cout << "Apple destructing" << endl;
}

void Apple::plant()
{
	cout << "Apple plant" << endl;
}

void Apple::grow()
{
	cout << "Apple grow" << endl;
}

void Apple::harvest()
{
	cout << "Apple harvest" << endl;
}

Grape::Grape()
{
	cout << "Grape construnting" << endl;
}

Grape::~Grape()
{
	cout << "Grape destructing" << endl;
}

void Grape::plant()
{
	cout << "Grape plant" << endl;
}

void Grape::grow()
{
	cout << "Grape grow" << endl;
}

void Grape::harvest()
{
	cout << "Grape harvest" << endl;
}

Gardener::Gardener()
{
	apple = NULL;
	grape = NULL;
	orange = NULL;
}

Fruit* Gardener::getFruit(int type)
{
	Fruit* fruit = NULL;
	if(type == APPLE){
		if(apple == NULL){
			apple = new Apple();
		}
		fruit = apple;
	}else if(type == GRAPE){
		if(grape == NULL){
			grape = new Grape();
		}
		fruit = grape;
	}else if(type == ORANGE){
		if(orange == NULL){
			orange = new Orange();
		}
		fruit = orange;
	}

	return fruit;
}
