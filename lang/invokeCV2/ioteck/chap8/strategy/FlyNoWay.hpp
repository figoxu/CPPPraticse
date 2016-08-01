#ifndef	_HFDP_CPP_STRATEGY_FLY_NO_WAY_WINGS_HPP_
#define _HFDP_CPP_STRATEGY_FLY_NO_WAY_WINGS_HPP_

#include "MiniDuckSimulator.hpp"


class FlyNoWay : public FlyBehavior {

public: 
	void fly () const {
		cout << "I can't fly" << endl;
	}
};

#endif
