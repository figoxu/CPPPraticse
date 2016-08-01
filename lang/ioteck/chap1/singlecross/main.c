#include <stdio.h>

#define STRING(x) #x

int main()
{
	char *str = STRING(abcdefghijklmn);
	printf("%s\n",str);
	return 0;
}

