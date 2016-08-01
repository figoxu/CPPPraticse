#ifndef	_HFDP_CPP_STRATEGY_MALLARD_DUCK_HPP_
#define _HFDP_CPP_STRATEGY_MALLARD_DUCK_HPP_

#include "MiniDuckSimulator.hpp"

class MallardDuck : public Duck {

public:	
	MallardDuck() 
		: Duck( new FlyWithWings(), new Quack() ) 
	{
	}

	void display() const {
		cout << "I'm a real Mallard duck" << endl;
	}
};


#endif
