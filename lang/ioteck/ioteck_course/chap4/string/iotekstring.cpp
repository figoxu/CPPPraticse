#include "iotekstring.h"

#include <string.h>
#include <iostream>
using namespace::std;

namespace iotek{

String::String(const char* str)
{
	if(NULL == str){
		m_data = new char[1];
		*m_data = '\0';
	}else{
		int length = strlen(str);
		m_data = new char[length + 1];
		strcpy(m_data,str);
	}
}

String::~String()
{
	delete[] m_data;
}

String::String(const String &other)
{
	int length = strlen(other.m_data);
	m_data = new char[length + 1];
	strcpy(m_data, other.m_data);
}

String& String::operator=(const String& other)
{
	if(this == &other){
		return *this;
	}

	delete[] m_data;

	int length = strlen(other.m_data);
	m_data = new char[length + 1];
	strcpy(m_data, other.m_data);

	return *this;
}

String& String::operator=(const char* other)
{
	delete[] m_data;
	
	if(other == NULL){
		m_data = new char[1];
		*m_data = '\0';
	}else{
		int length = strlen(other);
		m_data = new char[length + 1];
		strcpy(m_data, other);
	}
	
	return *this;
}

String& String::operator+=(const String& other)
{
	char* tmp = m_data;
	int length = strlen(m_data) + strlen(other.m_data);
	m_data = new char[length + 1];
	strcpy(m_data, tmp);
	strcat(m_data,other.m_data);
	
	delete[] tmp;


	return *this;
}

String String::operator+(const String& other) const
{
	String result;
	result += *this;
	result += other;

	return result;
}

String& String::operator+=(const char* other)
{
	String tmp(other);
	*this += tmp;

	return *this;
}

String String::operator+(const char* other) const
{
	String result = *this;
	result += other;

	return result;
}

}//end of iotek
