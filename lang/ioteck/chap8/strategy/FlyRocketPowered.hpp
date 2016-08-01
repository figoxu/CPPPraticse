#ifndef	_HFDP_CPP_STRATEGY_FLY_ROCKET_POWERED_HPP_
#define _HFDP_CPP_STRATEGY_FLY_ROCKET_POWERED_HPP_

#include "MiniDuckSimulator.hpp"


class FlyRocketPowered : public FlyBehavior {

public:	
	void fly () const {
		cout << "I'm flying with a rocket" << endl;
	}
};


#endif
