#include "CMemcache.h"

#include <iostream>
using std::cerr;
using std::endl;

#include <sstream>
using std::stringstream;

CMemcache::CMemcache ()
{
    server = NULL;
    Sock = NULL;
	timeout = EXPIRATION;
}

CMemcache::CMemcache (unsigned int time)
{
    server = NULL;
    Sock = NULL;
	timeout = time;
}

CMemcache::~CMemcache ()
{
    memcached_server_list_free (server);
}

void CMemcache::SetTime (unsigned int time)
{
	timeout = time;
}

bool CMemcache::IsVaild ()
{
    return (NULL == Sock) ? false : true;
}

bool CMemcache::AddrServer (string ip, unsigned int port)
{

    memcached_return result;

    server = memcached_server_list_append (server, (char*)ip.c_str (), port, &result);
    if (MEMCACHED_SUCCESS != result)
    {
        cerr << "Add memcache server failed!" << endl;
        return false;
    }

    return true;
}

bool CMemcache::Connect ()
{
    Sock = memcached_create (NULL);
    if (NULL == Sock)
    {
        cerr << "Create memcache handler failed!" << endl;
        return false;
    }

    if (MEMCACHED_SUCCESS != memcached_server_push (Sock, server))
    {
        cerr << "Push server failed!" << endl;
        memcached_server_list_free (server);
        Close ();
        return false;
    }

    if (MEMCACHED_SUCCESS != memcached_behavior_set (Sock,
            MEMCACHED_BEHAVIOR_NO_BLOCK, 1))
    {
        cerr << "Set noblock failed!" << endl;
        Close ();
        return false;
    }
    return true;
}

void CMemcache::Close ()
{
    memcached_free (Sock);
    Sock = NULL;
}

string CMemcache::Get (string key)
{
    char *buffer = NULL;
    string value = "";

    memcached_return result;
    size_t value_length;
    uint32_t flag = 32;

    buffer =
        memcached_get (Sock, key.c_str (), key.size (), &value_length, &flag,
        &result);

    if (MEMCACHED_SUCCESS == result)
    {
        value = string (buffer);
        free (buffer);
    }

    return value;
}

int CMemcache::GetAsNum(string key)
{
    string tmp = this->Get(key);

    if ("" == tmp)
        return -1;

    int res;

    stringstream ss(tmp);
    ss >> res;

    return res;
}

bool CMemcache::Add (string key, string value)
{
    memcached_return result;

    result =
        memcached_add (Sock, key.c_str (), key.size (), value.c_str (),
        value.size (), (time_t) timeout, (uint32_t) 0);
    return (MEMCACHED_SUCCESS == result) ? true : false;
}

bool CMemcache::AddAsNum (string key,int value)
{

    stringstream ss;
    ss << value;

    return  Add(key,ss.str());
}

bool CMemcache::Replace (string key, string value)
{
    memcached_return result;

    result =
        memcached_replace (Sock, key.c_str (), key.size (), value.c_str (),
        value.size (), (time_t) timeout, (uint32_t) 0);
    return (MEMCACHED_SUCCESS == result) ? true : false;
}

bool CMemcache::Set (string key, string value)
{
    memcached_return result;

    result =
        memcached_set (Sock, key.c_str (), key.size (), value.c_str (),
        value.size (), (time_t) timeout, (uint32_t) 0);
    return (MEMCACHED_SUCCESS == result) ? true : false;
}

bool CMemcache::Delete (string key)
{
    memcached_return result;

    result = memcached_delete (Sock, key.c_str (), key.size (), EXPIRATION);
    return (MEMCACHED_SUCCESS == result) ? true : false;
}

bool CMemcache::Increment(string key,uint64_t offset)
{
    uint64_t value = 0;
    memcached_return result;
    result = memcached_increment(Sock,key.c_str(),key.size(),offset,&value);
    return (MEMCACHED_SUCCESS == result) ? true : false;
}
