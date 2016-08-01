#include "rmb.h"
//rmb1 = ramb + rmb2 + rmb3;

RMB RMB::operator+(const RMB& val)
{
	this->jf += val.jf;
	this->yuan += val.yuan;
	if(jf >= 100){
		++yuan;
		jf-=100;
	}

	return RMB(yuan, jf);
}

bool RMB::operator>(const RMB& val)
{
	bool ret = false;
	if(yuan > val.yuan){
		ret = true;
	}else if(yuan == val.yuan){
		if(jf > val.jf){
			ret = true;
		}
	}

	return ret;
}

RMB::RMB(uint y, uint val)
	:yuan(y),jf(val)
{
}

RMB::~RMB()
{

}
