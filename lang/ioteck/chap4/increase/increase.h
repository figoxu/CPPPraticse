#ifndef _INCREASE_H_
#define _INCREASE_H_

class Increase{
public:
	Increase(int val);
	~Increase();

	Increase& operator++();//prefix
	Increase operator++(int);//postfix

	int getVal() const{
		return m_val;
	}

private:
	int m_val;
};

#endif
