#include "CMemcache.h"
#include "CIPParse.h"

#include <iostream>
using std::cerr;
using std::cout;
using std::endl;

#include <vector>
using std::vector;

#include <fstream>
using std::fstream;

void Usage(const string process_name)
{
    cerr << "Usage: " << process_name << " <ip:port> <filename> <time>" << endl;
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

	fstream fp(argv[2]);
	string line_buffer;

	int count = 0;

	while(getline(fp,line_buffer))
	{
		count++;
		if (count % 1000 == 0)
		{
			cerr << count << endl;
		}

		CIPParse myIPParse(line_buffer);
		vector<string> ips_buffer;

		myIPParse.getIPList(ips_buffer);

		if (ips_buffer.size() < 1)
		{
			cerr << line_buffer << endl;
			cerr << "Invalid Ip format!" << endl;
			continue;
		}

		for(vector<string>::iterator i = ips_buffer.begin(); i != ips_buffer.end(); i++)
		{
			if (false == mm->Set(*i,argv[3]))
			{
				cerr << "Set key:" << *i << " value:" << argv[4] << " failed" << endl;
				continue;
			}
		}
	}

	delete mm;

	exit(0);
}
