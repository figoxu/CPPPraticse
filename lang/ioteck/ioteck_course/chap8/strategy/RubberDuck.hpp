#ifndef	_HFDP_CPP_STRATEGY_RUBBER_DUCK_HPP_
#define _HFDP_CPP_STRATEGY_RUBBER_DUCK_HPP_

#include "MiniDuckSimulator.hpp"

class RubberDuck : public Duck {

public:	
	RubberDuck() 
		: Duck( new FlyNoWay(), new Squeak() ) 
	{
	}
	
	void display() const {
		cout << "I'm a rubber duck" << endl;
	}
};


#endif
