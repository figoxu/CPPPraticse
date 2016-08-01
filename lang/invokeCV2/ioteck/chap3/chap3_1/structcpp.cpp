class Savings
{
public:
	void deposit(double amount){
		balance += amount;
	}
	
	void set(unsigned num, double amount)
	{
		accountNumber = num;
		balance = amount;
	}
protected:
	int i;
private:
	unsigned accountNumber;
	double balance;
};

int main(int argc, char *argv[])
{
	Savings a;
	a.set(1,100.0);
	a.deposit(5.00);

	a.balance = 100;
	a.i = 0;
	
	return 0;
}
