#include "martain.h"

int Martain::martainCount  = 0; 

Martain::Martain(){
	martainCount++;
}
Martain::~Martain(){
	martainCount--;
}

int Martain::getCount(){
	return martainCount;
}


void Martain::fight()
{
}

void Martain::hide()
{

}

