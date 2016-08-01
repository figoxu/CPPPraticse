#include "rmb.h"

int main()
{
	RMB val1(2, 50);
	RMB val2(2, 48);

	if(val1 > val2){
		cout << "val1 is more than val2\n"; 
	}

	RMB val3 = val1 + val2;
	val3.display();

	return 0;
}


