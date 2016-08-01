#include <stdio.h>

int *pInt; //datebase
int i = 0;

void fun(int *p)
{
	p= &i;	
}

int main()
{
	fun(pInt);
	if(pInt == 0){
		printf("Error! pInt is still NULL\n");
	}
	printf("func--->\n");
	*pInt = 0;
	printf("aaaa\n");
	return 0;
}
