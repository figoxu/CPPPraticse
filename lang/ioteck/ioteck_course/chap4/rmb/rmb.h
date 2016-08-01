#ifndef _RMB_H_
#define _RMB_H_

typedef unsigned int uint;

#include <iostream>
using namespace::std;

class RMB{
	friend RMB operator+(const RMB&, const RMB&);
	friend bool operator>(const RMB&, const RMB&);
public:
	RMB(uint y, uint jf);
	~RMB();
	
	void display() const{
		cout << "RMB:" << yuan << ".";	
		if(jf < 10){
			cout << "0";
		}
		cout << jf << endl;
	}

		
private:
	uint yuan;
	uint jf;
};

#endif
