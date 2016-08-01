#include "iotekstring.h"
#include <iostream>
using namespace::std;
using namespace::iotek;

int main(int argc, const char*argv[])
{
	String s1("hello");

	String s2 = s1;

	String s3 = "world";

	s3 = s1;
	s3 = "world";

	s1 += s3;
	cout << "s1 = " << s1.data() << endl;

	s3 += "!";

	String s4 = s1 + s2;

	s4 = s1 + "hello";
	
	cout << "s3 = " << s3.data() << endl;

	return 0;
}
