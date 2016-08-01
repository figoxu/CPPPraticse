#include <iostream>
using namespace::std;

#include <stdlib.h>

class Test{
public:
	Test(int val = 0)
		:m_val(val)
	{
		cout << "Test" << endl;
	}
	~Test(){
		cout << "~Test" << endl;
	}
private:
	int m_val;
};

int main()
{
	{
		Test a;
	}
	cout << "end of }" << endl;

	Test* pVal = new Test();
	delete pVal;
	pVal = NULL;

	int *p = (int *)malloc(sizeof(int));
	free(p);
	p=NULL;

	Test *pArray = new Test[2];

	delete[] pArray;
	//delete pArray;

	pVal = new Test(10);
	delete pVal;

	return 0;
}

