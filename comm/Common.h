#ifndef _myCOMMON_H
#define _myCOMMON_H

#include <string>
using std::string;

#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

string GetCurrentTime ();
string GetCurrentDate ();
ssize_t LoopRead( int fd, const void *vptr, const size_t n );
int read_tmp_file( char *filename, string * p_buffer );
void string_tolower(string &src);
int my_str2int(const string& s);
string my_int2str(const int i);
bool is_tail(const string& str,const string& tail);
string& replace_all(string& str,const string& old_value,const string& new_value);
string UrlEncode(const string& szToEncode);
string UrlDecode(const string& szToDecode);
int timeval_subtract(struct timeval* start,struct timeval* stop);

#endif /*_myCOMMON_H*/
