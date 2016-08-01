#ifndef _STACK_H_
#define _STACK_H_

template<typename T> class stackIter;

template<typename T> 
class Stack
{
public:
	Stack() : top(0) {
		array[top] = 0;
	}
	void push(const T&i);
	T pop();

	friend class stackIter<T>;
private:
	enum {ssize = 100};
	T array[ssize];
	int top;
};

template<typename T> 
void Stack<T>::push(const T& i)
{
	if(top < ssize) array[top++] = i;
}

template<typename T> 
T Stack<T>::pop() {
	return array[top > 0 ? --top : top];
}

template<typename T> 
class stackIter {
public:
	stackIter(Stack<T>& is)
		:S(is),index(0)
	{
	}

	T& operator++() { //Prefix form
		if(index < S.top - 1) {
			index++;
		}
		return S.array[index];
	}

	T& operator++(int){
		int returnIndex = index;
		if(index < S.top - 1) 
		{
			index++;
		}
		return S.array[returnIndex];
	}

private:
	Stack<T>& S;
	int index;
};


#endif
