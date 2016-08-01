#include <vector>
#include <algorithm>
#include <iostream>

using namespace::std;

int main()
{
	vector<int> num;

	num.push_back(1);
	num.push_back(10);
	num.push_back(9);
	num.push_back(21);
	num.push_back(100);
	for(vector<int>::iterator it = num.begin();
			it != num.end(); ++it){
		cout << *it << endl;
	}

	sort(num.begin(), num.end());
	for(vector<int>::iterator it = num.begin();
			it != num.end(); ++it){
		cout << *it << endl;
	}

	return 0;
}



