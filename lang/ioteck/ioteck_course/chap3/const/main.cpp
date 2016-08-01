#include <iostream>
using namespace::std;

int main()
{
	const int a = 1;
	//a = 3;

	int b = 0;
	/*这里const 修饰的是int，而int定义的是一个整值
	因此*p 所指向的对象值 不能通过 *p 来修改，但是可以重新给p来赋值，使其指向不同的对象*/
	const int *p = &a;
	p = &b;
	b = 3;

	//error
	//*p = 1;

	int *const p2 = &b;
	p2 = &a;
	*p2 = 3;


	return 0;
}
