#ifndef _CDBOPTION_H_
#define _CDBOPTION_H_

#include <string>
#include <cstring>
#include <vector>

#ifndef OTL_ORA10G
#define OTL_ORA10G
#endif

#define OTL_STREAM_READ_ITERATOR_ON
#define OTL_STL

#include "otlv4.h"

using std::string;
using std::vector;

struct emaildomain
{
	char domain[128];
	char email[128];

	emaildomain()
	{
		memset(domain,'\0',128);
		memset(email,'\0',128);
	}
};

class CDBOption
{
  public:
    CDBOption (const string conn);
     ~CDBOption ();

    void WriteError (const char *type, otl_exception & e);
    void Commit ();
    
    bool GetEmailDomain(vector<struct emaildomain> &email);
	bool SetEmailDomain(vector<struct emaildomain> &email);

  private:
    otl_connect* m_conn;
    otl_stream* m_stream;

};

#endif /*_CDBOPTION_H_*/
