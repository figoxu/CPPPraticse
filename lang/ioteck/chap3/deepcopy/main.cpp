#include <iostream>
using namespace::std;

#include "person.h"

int main(int argc, char *argv[])
{
	Person	p("Joe");
	Person p3("tom");
	Person p2 = p;
	p.Print();
	p2.Print();
	
	return 0;
}
