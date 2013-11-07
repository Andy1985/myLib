#ifndef CIPPARSE_H
#define CIPPARSE_H

#include <string>
using std::string;

#include <vector>
using std::vector;

typedef unsigned long u_long;
typedef unsigned char u_char;

class CIPParse
{
	public:
		CIPParse(const string ip);
		CIPParse();
		~CIPParse();
		
		void setIP(const string ip);
		string getIP() const;

		void getIPList(vector<string>& ips);

	private:
		string ip;

		bool checkIpFormat(const string ip_);
		int pow2(int n);
		int long2ip(u_long in,char* ipstr);
		u_long ip2long(const char *ip);
		
};
#endif /*CIPPARSE_H*/
