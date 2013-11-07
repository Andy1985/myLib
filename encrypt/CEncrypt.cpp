#include "CEncrypt.h"

#include <string>
using std::string;

CEncrypt::CEncrypt(const string key_) : key(key_)
{

}

CEncrypt::~CEncrypt()
{

}

string CEncrypt::EncodeXOR(const char* str,unsigned int len)
{
	string result;
	result.assign(str,len);
	for (unsigned int i = 0; i < len; ++i)
	{
		result.at(i) = EncodeChar(*(str + i),key.at(i % key.size()));
	}

	return result;
}

string CEncrypt::DecodeXOR(const string& str)
{
	string result(str);
	for (unsigned int i = 0; i < str.size(); ++i)
	{
		result.at(i) = DecodeChar(str.at(i),key.at(i % key.size()));
	}

	return result;
}

inline unsigned char CEncrypt::EncodeChar(unsigned char c,unsigned char k)
{
	return c ^ k;
}

inline unsigned char CEncrypt::DecodeChar(unsigned char c,unsigned char k)
{
	return c ^ k;
}

