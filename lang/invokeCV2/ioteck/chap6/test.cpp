#include <iostream>

using namespace std;

class Base

{

	public:

		char Value() { return 'A';}

		virtual char VirtualValue() { return 'X';}

};

class Derived:public Base

{

	public:

		char Value(){return'U';}

};

class VirtualDerived:virtual public Base

{

	public:

		char Value() { return 'Z';}

		char VirtualValue() { return 'V';}

};

int main()

{

	Base *p1=new Derived();

	Base *p2=new VirtualDerived();

	cout<<p1->Value()<<" "<<

		p1->VirtualValue()<<" "<<

		p2->Value()<<" "<<

		p2->VirtualValue()<<endl;
	return 0;
}

