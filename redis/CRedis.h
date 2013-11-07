#ifndef CREDIS_H
#define CREDIS_H

#include <hiredis/hiredis.h>

#include <cstdio>
#include <cstdlib>

#include <string>
using std::string;

class CRedis
{
public:
	CRedis(const char* hostname,const int port);
	~CRedis();

	void setTimeout(const int sec,const int microsec = 0);
	bool Connect();
	void Close();
	string getError();	

	string Get(const string key);
	bool Set(const string key,const string value);
	bool Delete(const string key);

private:
    	redisContext *c;
    	redisReply *reply;
	string errorMsg;
	struct timeval timeout;
	const char* hostname;
	const int port;
};
#endif /*CREDIS_H*/
