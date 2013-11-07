#include "Common.h"

#include <string>
using std::string;

#include <iostream>

#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <sstream>
using std::stringstream;

string GetCurrentTime ()
{
    char current_time[32] = { 0 };
    time_t now = time (NULL);
    struct tm *ptr = localtime (&now);

    strftime (current_time, 32, "%Y-%m-%d %H:%M:%S", ptr);

    return string (current_time);
}

string GetCurrentDate ()
{
    char current_time[32] = { 0 };
    time_t now = time (NULL);
    struct tm *ptr = localtime (&now);

    strftime (current_time, 32, "%Y-%m-%d", ptr);

    return string (current_time);
}

ssize_t LoopRead( int fd, const void *vptr, const size_t n )
{
    int nread = 0;
    int nleft = n;
    char *ptr = ( char * ) vptr;

    if( n <= 0 )
        return -1;
    while( nleft > 0 )
    {
        if( ( nread = read( fd, ptr, nleft ) ) < 0 )
        {
            if( errno == EINTR )
                nread = 0;
            else
                return -1;
        }
        else if( nread == 0 )
            break;
        nleft = nleft - nread;
        ptr = ptr + nread;
    }
    return ( n - nleft );
}

int read_tmp_file( char *filename, string * p_buffer )
{
    struct stat statbuf;
    void *read_buf = NULL;
    int file_read = -1;

    memset( &statbuf, 0, sizeof( struct stat ) );

    if( !filename )
    {
        return -1;
    }

    if( strstr( filename, "../" ) || strstr( filename, "..\\" ) )
    {
        return -1;
    }

    if( !stat( filename, &statbuf ) )
    {
        if( statbuf.st_size == 0 )
            return -1;

        read_buf = ( void * ) malloc( statbuf.st_size + 1 );
        if( !read_buf )
        {
            return -1;
        }
        memset( read_buf, 0, statbuf.st_size + 1 );


        file_read = open( filename, O_RDONLY );
        if( ( !file_read == -1 )
            || ( LoopRead( file_read, read_buf, statbuf.st_size ) !=
                 statbuf.st_size ) )
        {
            if( read_buf )
            {
                free( read_buf );
                read_buf = NULL;
            }

            if( file_read == -1 )
            {
            }
            else
            {
                close( file_read );
            }
            return -1;
        }
        close( file_read );

        *p_buffer = string( ( char * ) read_buf );
        if( read_buf )
        {
            free( read_buf );
            read_buf = NULL;
        }
    }

    return 0;
}

void string_tolower(string &src)
{
    std::use_facet< std::ctype <char> >(std::locale()).tolower(
                    &(*src.begin()),&(*src.end()));
}

int my_str2int(const string& s)
{
	stringstream ss;
	ss << s;
	int ii = 0;
	ss >> ii;

	return ii;
}

string my_int2str(const int i)
{
	stringstream ss;
	ss << i;
	string s;
	ss >> s;

	return s;
}

bool is_tail(const string& str,const string& tail)
{
	if (str.find(tail,-tail.size()) != string::npos)
		return true;
	else
		return false;
}

string& replace_all(string& str,const string& old_value,const string& new_value)          
{          
    while (true)
    {          
        string::size_type pos(0);          

        if ((pos = str.find(old_value)) != string::npos)          
            str.replace(pos,old_value.length(),new_value);          
        else
           break;          
    }          

    return str;          
}

string UrlEncode(const string& szToEncode)
{
	string src = szToEncode;
	char hex[] = "0123456789ABCDEF";
	string dst;

	for (size_t i = 0; i < src.size(); ++i)
	{
		unsigned char cc = src[i];
		if (isascii(cc))
		{
			if (cc == ' ')
			{
				dst += "%20";
			}
			else
				dst += cc;
		}
		else
		{
			unsigned char c = static_cast<unsigned char>(src[i]);
			dst += '%';
			dst += hex[c / 16];
			dst += hex[c % 16];
		}
	}
	return dst;
}

string UrlDecode(const string& szToDecode)
{
	string result;
	int hex = 0;
	for (size_t i = 0; i < szToDecode.length(); ++i)
	{
		switch (szToDecode[i])
		{
		case '+':
			result += ' ';
			break;
		case '%':
			if (isxdigit(szToDecode[i + 1]) && isxdigit(szToDecode[i + 2]))
			{
				string hexStr = szToDecode.substr(i + 1, 2);
				hex = strtol(hexStr.c_str(), 0, 16);
				if (!((hex >= 48 && hex <= 57) ||	//0-9
					(hex >=97 && hex <= 122) ||	//a-z
					(hex >=65 && hex <= 90) ||	//A-Z
					hex == 0x21 || hex == 0x24 || hex == 0x26 || hex == 0x27 || hex == 0x28 || hex == 0x29
					|| hex == 0x2a || hex == 0x2b|| hex == 0x2c || hex == 0x2d || hex == 0x2e || hex == 0x2f
					|| hex == 0x3A || hex == 0x3B|| hex == 0x3D || hex == 0x3f || hex == 0x40 || hex == 0x5f
					))
				{
					result += char(hex);
					i += 2;
				}
				else result += '%';
			}else {
				result += '%';
			}
			break;
		default:
			result += szToDecode[i];
			break;
		}
	}
	return result;
}

int timeval_subtract(struct timeval* x,struct timeval* y)
{   
	struct timeval result;
	if (x->tv_sec>y->tv_sec)   
	{
    	return -1;   
	}

	if ((x->tv_sec==y->tv_sec) && (x->tv_usec>y->tv_usec))   
	{
        return -1;   
	}

	result.tv_sec = (y->tv_sec - x->tv_sec );   
	result.tv_usec = (y->tv_usec - x->tv_usec);   

	if (result.tv_usec < 0)   
	{   
        result.tv_sec--;   
        result.tv_usec += 1000000;   
	}   
	
	return (result.tv_sec * 1000000 + result.tv_usec) / 1000;
}
