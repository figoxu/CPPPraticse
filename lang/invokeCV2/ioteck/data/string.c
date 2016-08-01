#include <stdio.h>

int main()
{
	char *p = "hello";
	char *p2 = "hello";
	if(p==p2){
		printf("Smart complier, set the string in one place!\n");
	}
	
	char *p3 = (char *)malloc(12);
	memset(p3,0,12);
	strcpy(p3,p2);
	
	printf("p3 = %s\n",p3);
	p3[1] = 'x';
	printf("p3 = %s\n",p3);
	free(p3);
	p3 = NULL;

	//p[1] = 'x';

	return 0;
}
