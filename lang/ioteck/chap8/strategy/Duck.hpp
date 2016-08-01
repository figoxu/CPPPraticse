#ifndef	_HFDP_CPP_STRATEGY_DUCK_HPP_
#define _HFDP_CPP_STRATEGY_DUCK_HPP_

#include "MiniDuckSimulator.hpp"


class Duck {
private:
	auto_ptr< FlyBehavior > _flyBehavior;
	auto_ptr< QuackBehavior > _quackBehavior;

	Duck( const Duck& ); // Disable copy constructor
	void operator=( const Duck& ); // Disable assignment operator

protected: 
	Duck( FlyBehavior* flyBehavior, QuackBehavior* quackBehavior ) 
		:_flyBehavior( flyBehavior ), _quackBehavior( quackBehavior ) 
	{ 
		assert( flyBehavior ); 
		assert( quackBehavior );
	}

	virtual ~Duck()  {
	}

public:	
	void setFlyBehavior( FlyBehavior* fb ) 
	{ 
		assert( fb );
		_flyBehavior = auto_ptr< FlyBehavior >( fb );
	}
	
	void setQuackBehavior( QuackBehavior* qb ) 
	{ 
		assert( qb );
		_quackBehavior = auto_ptr< QuackBehavior >( qb );
	}
	
	void performFly() const {
		_flyBehavior->fly();
	}
	
	void performQuack() const {
		_quackBehavior->quack();
	}
	
	void swim() const {
		cout << "All ducks float, even decoys!" << endl;
	}
	
	virtual void display() const = 0;
};


#endif
