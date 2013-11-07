#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using std::stringstream;

#include "CDBOption.h"

const unsigned int MAX_SQL_SIZE = 1024 * 1024 * 2; // 2M

using std::endl;
using std::cout;
using std::cerr;
using std::string;
using std::vector;

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

bool CDBOption::GetEmailDomain(vector<struct emaildomain> &email)
{
    try
    {
        struct emaildomain buffer;
        char szData[128] = {'\0'};
		char szData1[128] = {'\0'};

        email.clear();

        string query = "select email,domain from eoss.tgpview";

        m_stream->open(1,query.data(),*m_conn);

        otl_stream_read_iterator < otl_stream, otl_exception,
            otl_lob_stream > rs;

        rs.attach (*m_stream);

        while (rs.next_row ())
        {
            rs.get ("email", szData);
			rs.get ("domain",szData1);

            strcpy(buffer.email,szData);
			strcpy(buffer.domain,szData1);

            email.push_back (buffer);
        }

        rs.detach ();
        m_stream->close();
        return true;
    }
    catch (otl_exception & p)
    {
        m_stream->close();
        this->WriteError ("GetEmailDomain:", p);
        return false;
    }
}

bool CDBOption::SetEmailDomain(vector<struct emaildomain> &email)
{
    try
    {
        char strSql[] = "BEGIN "
                        "sp_domain_email_update(:domain<char[128],in>,:email<char[128],in>); "
                        "END;";
        m_stream->open(1,strSql,*m_conn);

        for (vector<struct emaildomain>::iterator i = email.begin(); i != email.end(); i++)
        {
                *m_stream << i->domain << i->email;
        }

        m_stream->close();
        return true;
    } 
    catch (otl_exception &p)
    {
        m_stream->close();
        this->WriteError("SetEmailDomain Error: ",p);
        return false;
    }
}
