#include <stdio.h>

#define CONNECT(x,y) x##y

static void connect1()
{
	printf("%s\n",__func__);
}

static void connect2()
{
	printf("%s\n",__func__);
}

static void connect3()
{
	printf("%s\n",__func__);
}

int main()
{
	CONNECT(connect,1)(); //connect1();
	return 0;
}
