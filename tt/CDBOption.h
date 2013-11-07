#ifndef _CDBOPTION_H_
#define _CDBOPTION_H_

#include <string>
using std::string;
#include <cstring>

#define OTL_ODBC_TIMESTEN_UNIX
#define OTL_STREAM_READ_ITERATOR_ON
#define OTL_STL

#include "otlv4.h"


class CDBOption
{
  public:
    CDBOption (const string conn);
     ~CDBOption ();

    void WriteError (const char *type, otl_exception & e);
    void Commit ();

	//ִ��sql
	bool ExecuteSql(const string sql);

	//�������
	bool InsertName(const int id,const string name);
	//ɾ������
	bool DeleteName(const int id);	
	//��������
	bool UpdateName(const int id,const string name);
	//��������
    string GetName(const int id);
	

  private:
    otl_connect* m_conn;
    otl_stream* m_stream;


};

#endif /*_CDBOPTION_H_*/
