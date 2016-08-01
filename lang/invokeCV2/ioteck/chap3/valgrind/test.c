#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	char *p = NULL;
	p = (char *)malloc(10);
	strcpy(p,"0123456789");
	return 0;
}
