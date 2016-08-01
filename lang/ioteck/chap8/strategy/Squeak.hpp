#ifndef	_HFDP_CPP_STRATEGY_SQUEAK_HPP_
#define _HFDP_CPP_STRATEGY_SQUEAK_HPP_

#include "MiniDuckSimulator.hpp"


class Squeak : public QuackBehavior {

	public:	void quack () const {
		cout << "Squeak" << endl;
	}
};


#endif
