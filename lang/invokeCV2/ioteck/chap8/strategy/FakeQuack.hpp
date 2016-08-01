#ifndef	_HFDP_CPP_STRATEGY_FAKE_QUACK_HPP_
#define _HFDP_CPP_STRATEGY_FAKE_QUACK_HPP_

#include "MiniDuckSimulator.hpp"


class FakeQuack : public QuackBehavior {

public:	
	void quack () const {
		cout << "Qwak" << endl;
	}
};

#endif
