#ifndef UGW_CHTTP_H
#define UGW_CHTTP_H

#include <string>
using std::string;

#include <curl/curl.h>
#include <curl/types.h>


class CHttp
{
public:
	CHttp(const string url,int timeout=5);	
	CHttp(int timeout = 5);
	~CHttp();

	bool GetHttpRequestData(string& requestData);
	bool GetHttpRequestData(const string url,string& requestData);

	bool GetHttpRequestData(const string url,const string postdata,string& requestData);

private:
	string url;
	char errorBuffer[CURL_ERROR_SIZE];
	int timeout;
};
#endif /*UGW_CHTTP_H*/
