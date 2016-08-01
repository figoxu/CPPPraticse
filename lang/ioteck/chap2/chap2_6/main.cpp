#include <iostream>
using namespace::std;

#include <stdio.h>

inline bool isnumber(char ch)
{
	return ((ch >= '0') && (ch <= '9'));
}

int main(int argc, char *argv[])
{
	bool status = true;
	status = false;

	char c = 0;
	while((c = cin.get()) != 'q'){
		if(isnumber(c)){
			cout << "You enter a digit" << endl;
		}else{
			cout << "You enter a non-digit" << endl;
		}
	}
	
	return 0;
}

