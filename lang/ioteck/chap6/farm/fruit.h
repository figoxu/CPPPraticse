#ifndef _FRUIT_H_
#define _FRUIT_H_

#include <iostream>
using namespace::std;

class Fruit{
public:
	virtual ~Fruit() = 0;
	virtual void plant() = 0;
	virtual void grow() = 0;
	virtual void harvest() = 0;
};

class Apple : public Fruit{
public:
	Apple();
	~Apple();

	void plant();
	void grow();
	void harvest();
};

class Grape : public Fruit{
public:
	Grape();
	~Grape();
	void plant();
	void grow();
	void harvest();
};

enum{
	APPLE = 0,
	GRAPE = 1,
	ORANGE = 2
};

class Orange;

class Gardener{
public:
	Gardener();
	~Gardener() {}
	//0 apple, 1 grape
	Fruit* getFruit(int );

private:
	Apple *apple;
	Grape *grape;
	Orange *orange;
};

class Orange: public Fruit{
public:
	Orange(){
		cout << "orange constructing" << endl;
	}
	~Orange(){
	}

	void plant(){
		cout << "orange planting" << endl;
	}

	void grow(){
		cout << "orange growing" << endl;
	}

	void harvest() {
		cout << "orange harvesting" << endl;
	}

};

#endif
