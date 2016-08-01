#include <stdio.h>
#include <stdlib.h>

int nGlobal = 100;

int main(int argc, char *argv[])
{
	char *pLocalString1 = "LocalString1";
	const char *pLocalString2 = "LocalString2";

	static int nLocalStatic = 100;

	int nLocal = 1;
	const int nLocalConst = 20;

	int *pNew = new int[5];
	char *pMalloc = (char *)malloc(1);

	printf("global variable:				0x%x\n",&nGlobal);
	printf("static variable:				0x%x\n",&nLocalStatic);
	printf("local expression 1:				0x%x\n",pLocalString1);
	printf("local expression const:			0x%x\n",pLocalString2);

	printf("\n");

	printf("new:		0x%x\n",pNew);
	printf("malloc:		0x%x\n",pMalloc);
	printf("\n");

	printf("local pointer(pNew):				0x%x\n",&pNew);
	printf("local pointer(pLocalString1)		0x%x\n",&pLocalString1);
	printf("local pointer(pLocalString2)		0x%x\n",&pLocalString2);
	printf("local variable(nLocal):				0x%x\n",&nLocal);
	printf("local pointer(pMalloc)				0x%x\n",&pMalloc);
	printf("local const variable:				0x%x\n",&nLocalConst);

	delete []pNew;
	free(pMalloc);
	return 0;

}

