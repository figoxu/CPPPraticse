#ifndef _CAR_H_
#define _CAR_H_

#include <iostream>
#include <string>
using namespace::std;

namespace iotek{

class Engine{
public:
	Engine(int id)
		:m_id(id)
	{
	}

	~Engine(){}

	void start()
	{
		cout << "engine start\n";
	}

	void stop(){
		cout << "engine stop\n";
	}

private:
	int m_id;
};

class Wheel{
public:
	Wheel(int id)
	{
		m_id = id;
	}
	~Wheel(){}

	void roll(){
		cout << "wheel rolling" << endl;
	}

private:
	int m_id;
};

//class Car : public Wheel, public Engine
class Car{
public:
	Car(Engine *, Wheel*, string name, int price);
	~Car() {
	}

	void run();
	void stop();
	
private:
	Car(const Car&);
	Car& operator=(const Car&);

	Engine *engine;
	Wheel *wheel;
	string name;
	int price;
};

class Stero{
public:
	Stero() {}
	~Stero() {}

	void play()
	{
		cout << "stero play" << endl;
	}

};

class Benchi : public Car
{
public:
	Benchi(Engine *,Wheel *, string, int,Stero *);
	~Benchi(){
	}
	
	void musicOn();

private:
	Stero *stero;
};

class Transformer : public Car
{
public:
	Transformer(Engine*,Wheel*,string,int,bool);
	~Transformer()
	{
	}

	void fight();
	void transform();

private:
	bool val;
};

}

#endif
