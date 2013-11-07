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

	//执行sql
	bool ExecuteSql(const string sql);

	//添加数据
	bool InsertName(const int id,const string name);
	//删除数据
	bool DeleteName(const int id);	
	//更新数据
	bool UpdateName(const int id,const string name);
	//查找数据
    string GetName(const int id);
	

  private:
    otl_connect* m_conn;
    otl_stream* m_stream;


};

#endif /*_CDBOPTION_H_*/
