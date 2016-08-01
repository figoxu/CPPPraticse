#ifndef	_HFDP_CPP_STRATEGY_REDHEAD_DUCK_HPP_
#define _HFDP_CPP_STRATEGY_REDHEAD_DUCK_HPP_

#include "MiniDuckSimulator.hpp"


class RedHead : public Duck {

public:	
	RedHead() 
		: Duck( new FlyWithWings(), new Quack() ) 
	{
	}
	
	void display() const {
		cout << "I'm a real Red Headed duck" << endl;
	}
};


#endif
