#include <iostream>
using namespace::std;

class A{
public:
	A();
	~A() {}

private:
	const int val;
};

A::A()
	:val(0)
{
}

int main()
{
	A a;
}
