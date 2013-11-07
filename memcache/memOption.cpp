#include "CMemcache.h"

#include <iostream>
using std::cout;
using std::endl;
using std::cerr;

#include <cstdio>
#include <cstdlib>
#include <cstring>

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
        return -1; 
    }

    string Option = argv[2];
    if ((Option == "add" || Option == "set" || Option == "replace") && argv[4] == NULL)
    {
        cerr << "Must input the Value!" << endl;
        return -1;
    }

    if (Option == "add")
    {
        if (false == mm->Add(argv[3],argv[4]))
        {
            cerr << "Add key:" << argv[3] << " value:" << argv[4] << " failed" << endl;
            return -1;
        }
    }
    else if (Option == "set")
    {
        if (false == mm->Set(argv[3],argv[4]))
        {
            cerr << "Set key:" << argv[3] << " value:" << argv[4] << " failed"<< endl;
            return -1;
        }
    }
    else if (Option == "replace")
    {
        if (false == mm->Replace(argv[3],argv[4]))
        {
            cerr << "Replace key:" << argv[3] << " value:" << argv[4] << " failed" << endl;
            return -1;
        }
    }
    else if (Option == "delete")
    {
        if (false == mm->Delete(argv[3]))
        {
            cerr << "Delete key:" << argv[3] << " failed" << endl;
            return -1;
        }
    }
    else if (Option == "get")
    {
        string value = mm->Get(argv[3]);
        if ("" == value) value = "(null)";
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
