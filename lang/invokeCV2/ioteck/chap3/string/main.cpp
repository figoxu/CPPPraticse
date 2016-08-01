#include <string>

#include <iostream>

using namespace::std;

int main()
{
	std::cout << "Hello string" << std::endl;	
	string a("c++");
	string b(a);
	string c(4,'a');
	cout << a << endl;
	cout << b << endl;
	cout << c << endl;

	string d;
	d=a;
	if(d.empty())
	{
		cout << "string d is empty" << endl; 
	}else{
		cout << "string d's size is " << d.size() << endl;
	}
	
	string e = a + c;
	cout << e << endl;
	if(e == a){
		cout << "e equals a" << endl;
	}else{
		cout << "e doesn't equal a " << endl;
	}


	return 0;
}


