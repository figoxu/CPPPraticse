#ifndef _SINGLETON_H_
#define _SINGLETON_H_

class Singleton{

public:
	static Singleton* getInstance();
	
	void doSomething();

	void destroy();

private:
	Singleton();
	~Singleton();

	Singleton(const Singleton&);
	Singleton& operator=(const Singleton&);

	static Singleton* instance;

};

#endif
