#include "MiniDuckSimulator.hpp"

int main(int argc, char* argv[]) {

	auto_ptr< MallardDuck > mallard( new MallardDuck() );
	auto_ptr< RubberDuck > rubberDuckie( new RubberDuck );
	auto_ptr< DecoyDuck > decoy( new DecoyDuck );
	auto_ptr< ModelDuck > model( new ModelDuck );

	mallard->performQuack();
	mallard->performFly();

	rubberDuckie->performQuack();
	rubberDuckie->performFly();

	decoy->performQuack();
	decoy->performFly();

	model->performQuack();
	model->performFly();	
	model->setFlyBehavior( new FlyRocketPowered() );
	model->performFly();

	return 0;
}
