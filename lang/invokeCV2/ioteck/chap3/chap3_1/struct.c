
struct Savings{
	unsigned accountNumber;
	float balance;
};

int main(int argc, char *argv[])
{
	struct Savings a;
	struct Savings b;
	
	a.accountNumber = 1;
	b.accountNumber = 2;
	a.balance = 100.00;
	b.balance = -1000.00;
}
