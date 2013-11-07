#ifndef CENCRYPT_H
#define CENCRYPT_H

#include <string>
using std::string;

class CEncrypt
{
public:
	CEncrypt(const string key);
	~CEncrypt();
	
	string EncodeXOR(const char* str,unsigned int len);
	string DecodeXOR(const string& str);

private:
	const string key;
	
	unsigned char EncodeChar(unsigned char c,unsigned char k);
	unsigned char DecodeChar(unsigned char c,unsigned char k);
	
};
#endif /*CENCRYPT_H*/
