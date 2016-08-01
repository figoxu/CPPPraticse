#ifndef IOTEK_STRING_H_
#define IOTEK_STRING_H_

#include <stdio.h>

namespace iotek{


class String{
public:
	String(const char * = NULL);
	~String();

	String(const String&);

	//String a; a = b;
	String& operator=(const String &);
	//String a; a = "hello";
	String& operator=(const char*);


	String& operator+=(const String&);
	String operator+(const String&) const;
	
	String& operator+=(const char *);
	String operator+(const char *) const;
	inline const char* data() const
	{
		return m_data;
	}

private:
	char *m_data;
};

}

#endif
