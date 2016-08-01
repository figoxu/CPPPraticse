#include "car.h"
using namespace::iotek;

int main()
{
	Engine e1(1);
	Wheel w1(1);
	Stero stero;

	Benchi benchi(&e1,&w1,"benchi",100,&stero);
	benchi.musicOn();
	
	Transformer t(&e1, &w1,"optimusprime",200,true);
	t.transform();
	t.fight();


	return 0;
}
