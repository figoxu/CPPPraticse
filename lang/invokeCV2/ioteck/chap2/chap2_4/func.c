#include <stdio.h>

int* func()
{
	int i = 0;


	return &i;
}

int main()
{
	int *p = func();


	*p = 19999;


	return 0;
}
