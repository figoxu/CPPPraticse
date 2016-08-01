#include <iostream>
using namespace::std;

long cow_birth(int year)
{
	if(year >= 4){
		return cow_birth(year-1) + cow_birth(year -3);
	}else{
		return 1;
	}
}

int main(int argc, char *argv[])
{
	int i = 0;
	for(i=1; i< 12; i++){
		cout << i << "\t" << cow_birth(i) << endl;
	}
	
	return 0;	
}
