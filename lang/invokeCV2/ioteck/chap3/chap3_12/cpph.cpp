#include <string>
#include <iostream>

#include <cstring>

using namespace::std;

extern "C" char* getString()
//char* getString()
{
	char *ret = new char[30];
	memset(ret,30,0);
	string a = "hello";
	strcpy(ret,a.c_str());
	return ret;
}

extern "C" void releaseStr(char *str)
//void releaseStr(char *str)
{
	delete str;
}
