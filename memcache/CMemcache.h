#ifndef _mCMEMCACHE_H_
#define _mCMEMCACHE_H_

#include <libmemcached/memcached.h>
#include <string>
using std::string;

#ifndef EXPIRATION
#define EXPIRATION 0x0
#endif

class CMemcache
{
  public:
    CMemcache ();
	CMemcache (unsigned int time);
    ~CMemcache ();
	void SetTime (unsigned int time);
    bool AddrServer (string ip, unsigned int port);
    bool Connect ();
    void Close ();
    bool IsVaild ();

    string Get (string key);
    int GetAsNum(string key);
    bool Add (string key, string value);
    bool AddAsNum(string key,int value);
    bool Replace (string key, string value);
    bool Set (string key, string value);
    bool Delete (string key);
    bool Increment(string key,uint64_t offset);

  private:
    memcached_st * Sock;
    memcached_server_st *server;
	unsigned int timeout;

};

#endif /*_mCMEMCACHE_H_*/
