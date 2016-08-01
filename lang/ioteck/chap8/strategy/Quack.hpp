#ifndef	_HFDP_CPP_STRATEGY_QUACK_HPP_
#define _HFDP_CPP_STRATEGY_QUACK_HPP_

#include "MiniDuckSimulator.hpp"

class Quack : public QuackBehavior {

	public: void quack () const {
		cout << "Quack" << endl;
	}
};


#endif
