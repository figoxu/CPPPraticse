#ifndef _EXAMLPLE_H_
#define _EXAMLPLE_H_

namespace iotek{

class Test{
public:
	Test() {}
	~Test() {}
private:
	Test(const Test&);
	Test& operator=(const Test&);
};

template<typename T> // int, char,class type
class Example{
public:
	explicit Example(T val);
	~Example();

	T getVal() const;
	void setVal(T val);

private:
	T m_val;
};

template<typename T>
Example<T>::Example(T val)
	:m_val(val)
{
}

template<typename T>
Example<T>::~Example()
{
}

template<typename T>
void Example<T>::setVal(T val)
{
	m_val = val;
}

template<typename T>
T Example<T>::getVal() const{
	return m_val;
}

}

#endif
