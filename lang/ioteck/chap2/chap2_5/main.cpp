//chap2_5.cpp
//Simple C++ reference

int* f(int *x)
{
	(*x)++;
	return x;//Safe, x is outside this scope
}

int& g(int &x)
{
	x++;//Same effect as in f()
	return x;//safe,outside this scope
}

int& h(){
	int q = 0;
	//return q; //Error
	static int x;
	return x;//Safe, x lives outside this scope
}

int main(int argc, char *argv[])
{
	int a = 0;
	f(&a);//Ugly,explicit
	g(a);//Clean, but hidden
}