#include <iostream>
using namespace::std;

#include "stack.h"

#define STACK_COUNT 10

int main(int argc, char* argv[])
{
	Stack<int> is;
	int i  = 0;

	for(i=0;i<STACK_COUNT;i++){
		is.push(i);
	}

	stackIter<int> it(is);
	for(i=0; i < STACK_COUNT; i++)
	{
		cout << it++ << endl;
	}
	return 0;
}


