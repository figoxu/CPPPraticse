#ifndef _CRECT_H_
#define _CRECT_H_

class Rect{
public:
	Rect();
	void set(double ,double,double,double );
	double area();
private:
	double left;
	double top;
	double width;
	double height;
};

#endif

