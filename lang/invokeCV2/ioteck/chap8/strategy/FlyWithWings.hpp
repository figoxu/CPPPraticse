#ifndef	_HFDP_CPP_STRATEGY_FLY_WITH_WINGS_HPP_
#define _HFDP_CPP_STRATEGY_FLY_WITH_WINGS_HPP_

#include "MiniDuckSimulator.hpp"


class FlyWithWings : public FlyBehavior {

public:	
	void fly () const {
		cout << "I'm flying!!" << endl;
	}
};


#endif
