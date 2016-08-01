#include "furniture.h"

Sofa::Sofa(int weight)
	:Furniture(weight)
{
	cout << "sofa constructing" << endl;
}

Bed::Bed(int weight)
	:Furniture(weight)
{
	cout << "Bed constructing" << endl;
}

SofaBed::SofaBed(int weight)
	:Furniture(weight)
{
	cout << "Sofabed constructing" << endl;
}
