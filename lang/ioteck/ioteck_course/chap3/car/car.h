#ifndef _CAR_H_
#define _CAR_H_

class Car{
public:
	Car(int pric, int carNum);
	void run();
	void stop();

	void setProperty(int price, int carNum);

	void print();

private:
	int m_price;
	int m_carNum;
};


#endif
