#include "CHttp.h"

#include <iostream>
using std::cout;
using std::endl;

int writer( char *data, size_t size, size_t nmemb, string * writerData )
{
    unsigned long sizes = size * nmemb;

    if( writerData == NULL )
    {
        return 0;
    }

    writerData->append( data, sizes );

    return sizes;
}

CHttp::CHttp(const string url_,int timeout_):url(url_),timeout(timeout_)
{
    curl_global_init(CURL_GLOBAL_ALL);
	memset(errorBuffer,'\0',sizeof(errorBuffer));
}

CHttp::CHttp(int timeout_):timeout(timeout_)
{
    curl_global_init(CURL_GLOBAL_ALL);
	memset(errorBuffer,'\0',sizeof(errorBuffer));
}

CHttp::~CHttp()
{
    curl_global_cleanup();
}

bool CHttp::GetHttpRequestData(string& urlContent)
{
    CURLcode code;
    CURL* conn;

    conn = curl_easy_init(  );
    if( conn == NULL )
    {
        return false;
    }

    curl_easy_setopt( conn, CURLOPT_ERRORBUFFER, errorBuffer );
    curl_easy_setopt(conn,CURLOPT_URL,url.c_str());
    curl_easy_setopt( conn, CURLOPT_FOLLOWLOCATION, 1 );
    curl_easy_setopt( conn, CURLOPT_MAXREDIRS, 3 );
    curl_easy_setopt(conn,CURLOPT_WRITEFUNCTION,writer);
    curl_easy_setopt( conn, CURLOPT_WRITEDATA, &urlContent);
    curl_easy_setopt( conn, CURLOPT_TIMEOUT, timeout );
    curl_easy_setopt( conn, CURLOPT_CONNECTTIMEOUT, timeout );
    curl_easy_setopt( conn, CURLOPT_FORBID_REUSE, 1 );
    curl_easy_setopt( conn, CURLOPT_BUFFERSIZE, 1024);

    code = curl_easy_perform( conn );
	if(code != CURLE_OK)
	{
      fprintf(stderr,"%s\n",curl_easy_strerror(code));
	}
 

    curl_easy_cleanup(conn);
	
	if (&urlContent == NULL) urlContent = "";

	return true;
}

bool CHttp::GetHttpRequestData(const string url_,string& urlContent)
{
	this->url = url_;
	return GetHttpRequestData(urlContent);
}

bool CHttp::GetHttpRequestData(const string url_,const string postdata,string& urlContent)
{
    this->url = url_;
    CURLcode code;
    CURL* conn;

    conn = curl_easy_init(  );
    if( conn == NULL )
    {
        return false;
    }

    curl_easy_setopt( conn, CURLOPT_ERRORBUFFER, errorBuffer );
    curl_easy_setopt(conn,CURLOPT_URL,url.c_str());
    curl_easy_setopt(conn, CURLOPT_POSTFIELDS, postdata.c_str());
    curl_easy_setopt( conn, CURLOPT_SSL_VERIFYPEER, 0 );
    curl_easy_setopt( conn, CURLOPT_FOLLOWLOCATION, 1 );
    curl_easy_setopt( conn, CURLOPT_MAXREDIRS, 3 );
    curl_easy_setopt(conn,CURLOPT_WRITEFUNCTION,writer);
    curl_easy_setopt( conn, CURLOPT_WRITEDATA, &urlContent);
    //curl_easy_setopt( conn, CURLOPT_REDIR_PROTOCOLS, 1 );
    curl_easy_setopt( conn, CURLOPT_TIMEOUT, timeout );
    curl_easy_setopt( conn, CURLOPT_CONNECTTIMEOUT, timeout );
    curl_easy_setopt( conn, CURLOPT_FORBID_REUSE, 1 );
    curl_easy_setopt( conn, CURLOPT_BUFFERSIZE, 1024);

    code = curl_easy_perform( conn );
	if(code != CURLE_OK)
	{
      fprintf(stderr,"%s\n",curl_easy_strerror(code));
	}
 

    curl_easy_cleanup(conn);
	
	if (&urlContent == NULL) urlContent = "";

	return true;
}
