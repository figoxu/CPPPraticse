#include "increase.h"

Increase::Increase(int val)
	:m_val(val)
{
}

Increase::~Increase()
{
}

Increase& Increase::operator++()
{
	++m_val;
	return *this;
}

Increase Increase::operator++(int){
	Increase ret(m_val);
	++m_val;

	return ret;
}
