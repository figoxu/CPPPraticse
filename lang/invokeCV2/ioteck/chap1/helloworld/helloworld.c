#include <stdio.h>

int x;

//#define LINUX

//file helloworld.c
int main(int argc, char *argv[])
{

	int y;
#ifdef LINUX
	printf("hello world!");

	printf("x=%d\n",x);
	
	printf("y=%d\n",y);
#else
	printf("Linux is not defined");
#endif
	

	return 0;
}
