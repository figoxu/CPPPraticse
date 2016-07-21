#include <iostream>
using namespace std;


class Line {
public:
    Line(double len);

    void setLength(double len);
    double getLength(void);
    Line();
    ~Line();

private:
    double length;
};



Line::Line(void){
    cout << "Object is being createed " << endl;
}

Line::Line(double len) {
    length = len;
}

Line::~Line(void){
    cout << "Object is being deleted " << endl;
}

void Line::setLength(double len) {
    length = len;
}
double Line::getLength() {
    return length;
}


int main() {
    Line line2(108.0);
    Line line;

    line.setLength(101.0);
    cout << "Length of line : " << line.getLength() << endl;
    cout << "length of line2 : " << line2.getLength() << endl;
    return 0;
}