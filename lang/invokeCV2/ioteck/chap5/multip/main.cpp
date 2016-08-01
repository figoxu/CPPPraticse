#include "furniture.h"

int main()
{
	cout << sizeof(Furniture) << endl;
	cout << sizeof(Sofa) << endl;
	cout << sizeof(Bed) << endl;
	cout << sizeof(SofaBed) << endl;

	SofaBed sofabed(1);

	sofabed.watchTV();
	sofabed.sleep();
	sofabed.foldout();

	return 0;
}
