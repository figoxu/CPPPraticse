#include "fruit.h"

int main()
{
	Gardener tom;

	Fruit* fruit = tom.getFruit(ORANGE);
	fruit->plant();
	fruit->grow();
	fruit->harvest();

	return 0;
}
