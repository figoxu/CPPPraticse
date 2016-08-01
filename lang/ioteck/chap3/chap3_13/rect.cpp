#include "rect.h"

#include <iostream>
using namespace::std;

Rect::Rect()
{
	top = 0;
	left = 0;
	width = 0;
	height = 0;
}

void Rect::set(double l, double t,double w,double h)
{
	left = l;
	top = t;
	width = w;
	height = h;
}

double Rect::area()
{
	return width * height;
}

