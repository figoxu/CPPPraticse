#ifndef	_HFDP_CPP_STRATEGY_DECOY_DUCK_HPP_
#define _HFDP_CPP_STRATEGY_DECOY_DUCK_HPP_

#include "MiniDuckSimulator.hpp"


class DecoyDuck : public Duck {

public:	
	DecoyDuck() 
		: Duck( new FlyNoWay(), new MuteQuack() ) 
	{
	}
	
	void display() const {
		cout << "I'm a duck Decoy" << endl;
	}
};


#endif
