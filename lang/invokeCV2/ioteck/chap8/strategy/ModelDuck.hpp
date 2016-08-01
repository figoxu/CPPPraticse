#ifndef	_HFDP_CPP_STRATEGY_MODEL_DUCK_HPP_
#define _HFDP_CPP_STRATEGY_MODEL_DUCK_HPP_

#include "MiniDuckSimulator.hpp"
class ModelDuck : public Duck {

public:	
	ModelDuck() 
		: Duck( new FlyNoWay(), new FakeQuack() ) 
	{
	}

	void display() const {
		cout << "I'm a model duck" << endl;
	}
};


#endif
