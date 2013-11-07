#include "CIPParse.h"
#include <iostream>
using std::cout;
using std::endl;
using std::cerr;

int main(int argc,char* argv[])
{
	CIPParse myIPParse(argv[1]);
	vector<string> ips_buffer;
	
	myIPParse.getIPList(ips_buffer);

	for (vector<string>::iterator i = ips_buffer.begin(); i != ips_buffer.end(); ++i)
	{
		cout << *i << endl;
	}
	
	exit(0);
}
