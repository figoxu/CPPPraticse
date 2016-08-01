#ifndef _POINT_H_
#define _POINT_H_

class Point{
public:
	void Set(double x,double y);
	double xOffset();
	double yOffset();
	double angle();
	double radius;
private:
	double mx;
	double my;
};

#endif

