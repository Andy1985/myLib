#include <iostream>
using std::endl;
using std::cout;
using std::cerr;
#include <string>
using std::string;
#include "CDBOption.h"

CDBOption::CDBOption (const string conn)
{
    try
    {
        otl_connect::otl_initialize (1);
        m_conn = new otl_connect;
        m_stream = new otl_stream;
		m_conn->rlogon(conn.data(),0);
		m_conn->auto_commit_off();
    }
    catch (otl_exception & p)
    {
        WriteError ("Constuctor: ", p);
    }
}

CDBOption::~CDBOption ()
{
    m_conn->logoff();
    delete m_stream;
    delete m_conn;
}

void CDBOption::WriteError (const char *type, otl_exception & e)
{
    cerr << type << " " << e.msg << " " << e.stm_text << " " << e.var_info << endl;
}

void CDBOption::Commit ()
{
    try
    {
        m_conn->commit ();
    }
    catch (otl_exception & p)
    {
        WriteError ("Commit: ", p);
    }
}

bool CDBOption::ExecuteSql(const string sql)
{
	try {
		otl_cursor::direct_exec(*m_conn,sql.data());
		return true;
	}
    catch (otl_exception & p)
    {
        WriteError ("ExecuteSql: ", p);
		return false;
    }
}

bool CDBOption::InsertName(const int id,const string name)
{
    try
    {
        string query = "insert into test(id,name) values "
					"(:id<int>,:name<char[30]>)";
        m_stream->open(1,query.data(),*m_conn);
        *m_stream << id << name.data();
        m_stream->close();
		return true;
    }
    catch (otl_exception & p)
    {
        m_stream->close();
        this->WriteError ("InsertName Error: ", p);
        return false;
    }
}

bool CDBOption::DeleteName(const int id)
{
    try
    {
        string query = "delete from test where id = :id<int>";
        m_stream->open(1,query.data(),*m_conn);
        *m_stream << id;
        m_stream->close();
		return true;
    }
    catch (otl_exception & p)
    {
        m_stream->close();
        this->WriteError ("DeleteName Error: ", p);
        return false;
    }

}

bool CDBOption::UpdateName(const int id,const string name)
{
    try
    {
        string query = "update test set name = :name<char[30]> where id = :id<int>";
        m_stream->open(1,query.data(),*m_conn);
        *m_stream << name.data() << id;
        m_stream->close();
		return true;
    }
    catch (otl_exception & p)
    {
        m_stream->close();
        this->WriteError ("UpdateName Error: ", p);
        return false;
    }
}

string CDBOption::GetName(const int id)
{
    try
    {
        char szData[128];
        string query = "select name from test "
					"where id = :id<int>";
        m_stream->open(1,query.data(),*m_conn);

        *m_stream << id;
        otl_stream_read_iterator < otl_stream, otl_exception,
            otl_lob_stream > rs;

        rs.attach (*m_stream);
    
        while (rs.next_row ())
        {
            rs.get ("name", szData);
        }

        rs.detach ();
        m_stream->close();

        return string(szData);
    }
    catch (otl_exception & p)
    {
        m_stream->close();
        this->WriteError ("GetName Error: ", p);
        return "";
    }
}
