#include "CMemcache.h"

#include <iostream>
using std::cout;
using std::endl;
using std::cerr;

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "CIPParse.h"

void Usage(const string process_name)
{
    cerr << "Usage: " << process_name << " <ip:port> [add|set|get|delete|replace] <key> [value]" << endl;
}

int main(int argc,char* argv[])
{
    if (argc < 4)
    {
        Usage(argv[0]);
        exit(1);
    }    
    
    char* p = strstr(argv[1],":");
	if (NULL == p) Usage(argv[0]);
    unsigned int port = atoi(p+1);
    *p = '\0';
    string ip = string(argv[1]);


    CMemcache* mm = new CMemcache();
    
    mm->AddrServer(ip,port);
    if (false == mm->Connect())
    {
        cerr << "Cann't connect to " << ip << ":" << port << endl;
		delete mm;
        return -1; 
    }

    string Option = argv[2];
    if ((Option == "add" || Option == "set" || Option == "replace") && argv[4] == NULL)
    {
        cerr << "Must input the Value!" << endl;
		delete mm;
        return -1;
    }

	//Parse IP seg
	CIPParse myIPParse(argv[3]);
	vector<string> ips_buffer;

	myIPParse.getIPList(ips_buffer);

	if (ips_buffer.size() < 1)
	{
		cerr << "Invalid Ip format!" << endl;
		delete mm;
		return -1;
	}

    if (Option == "add")
    {
		for(vector<string>::iterator i = ips_buffer.begin(); i != ips_buffer.end(); i++)
		{
			if (false == mm->Add(*i,argv[4]))
			{
				cerr << "Add key:" << *i << " value:" << argv[4] << " failed" << endl;
				delete mm;
				return -1;
			}
		}
    }
    else if (Option == "set")
    {
		for(vector<string>::iterator i = ips_buffer.begin(); i != ips_buffer.end(); i++)
		{
			if (false == mm->Set(*i,argv[4]))
			{
				cerr << "Set key:" << *i << " value:" << argv[4] << " failed" << endl;
				delete mm;
				return -1;
			}
		}
    }
    else if (Option == "replace")
    {
		for(vector<string>::iterator i = ips_buffer.begin(); i != ips_buffer.end(); i++)
		{
			if (false == mm->Replace(*i,argv[4]))
			{
				cerr << "Set key:" << *i << " value:" << argv[4] << " failed" << endl;
				delete mm;
				return -1;
			}
		}
    }
    else if (Option == "delete")
    {
		for(vector<string>::iterator i = ips_buffer.begin(); i != ips_buffer.end(); i++)
		{
			if (false == mm->Delete(*i))
			{
				cerr << "delete key:" << *i << " failed" << endl;
				continue;
			}
		}
    }
    else if (Option == "get")
    {
		string value = "";
		for(vector<string>::iterator i = ips_buffer.begin(); i != ips_buffer.end(); i++)
		{
			value = mm->Get(*i);
			if ("" == value)
			{
				value = "(null)";
				break;
			}
		}

        cout << value << endl;
    }
    else 
    {
        cout << "commond: " << Option << " error!" << endl;
        return -1;
    }
    
    if (Option == "delete" || Option == "replace" || Option == "set" || Option == "add")
        cout << Option << " OK" << endl;

    mm->Close();
    delete mm;

    exit(0);
}
