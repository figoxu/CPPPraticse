#ifndef	_HFDP_CPP_STRATEGY_MUTE_QUACK_HPP_
#define _HFDP_CPP_STRATEGY_MUTE_QUACK_HPP_

#include "MiniDuckSimulator.hpp"
class MuteQuack : public QuackBehavior {

	void quack () const {
		cout << "<< Silence >>" << endl;
	}
};


#endif
