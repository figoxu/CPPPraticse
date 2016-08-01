#ifndef _SMART_PTR
#define _SMART_PTR
#include <stdio.h>

#ifdef TRACE_SMPTR
#define TRACE printf
#else
#define TRACE
#endif

template <typename T> 
class SmartPtr
{
public:
	SmartPtr(T* p_) : p(p_) 
	{
		TRACE("constructor SmartPtr\n");
		p->upcount();
	}

	~SmartPtr(void)
	{
		TRACE("deconstructor SmartPtr\n");
		p->downcount();
	}

	operator T*(void){
		TRACE("T*(void)\n");
		return p;
	}

	T& operator*(void){
		TRACE("%s,%d\n",__func__,__LINE__);
		return *p;
	}

	T* operator->(void){
		TRACE("%s,%d\n",__func__,__LINE__);
		return p;
	}

	SmartPtr& operator=(SmartPtr<T> &p_) {
		TRACE("%s,%d\n",__func__,__LINE__);
		return operator=((T*)p_);
	}

	SmartPtr& operator=(T* p_){
		TRACE("%s,%d\n",__func__,__LINE__);
		p_->upcount();
		p->downcount();
		p=p_;
		return *this;
	}

	SmartPtr(const SmartPtr<T> &p_) {
		p = p_.p;
		p->upcount();
	}
private:
	T* p;
};
#endif
