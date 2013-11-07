#include <cstdio>

#include "CIPParse.h"

#include <iostream>
using std::endl;
using std::cout;
using std::cerr;

CIPParse::CIPParse()
{
	this->ip = "";
}

CIPParse::CIPParse(const string ip)
{
	this->ip = ip;
}

CIPParse::~CIPParse()
{

}

void CIPParse::setIP(const string ip)
{
	this->ip = ip;
}

string CIPParse::getIP() const
{
	return this->ip;
}

bool CIPParse::checkIpFormat(const string ip_)
{
	if (ip_.size() == 0)
	{
		return false;
	}

	u_char addr[16];
	sscanf(ip_.c_str(),"%d.%d.%d.%d",addr,addr+1,addr+2,addr+3);

	if((u_long)addr[3] < 0 || (u_long)addr[3] > 255)
	{
		return false;
	}

	return true;
}

void CIPParse::getIPList(vector<string>& ips)
{
	string::size_type pos = this->ip.find("/");	

	ips.clear();

	if(pos != string::npos)
	{
	
		string buffer = this->ip.substr(0,pos);
		
		int mask_len = atoi(this->ip.substr(pos + 1).c_str());
		int seg = 32 - mask_len;
		int ipcnts = pow2(seg) - 1;
		
		if(seg > 0)
		{

			for(int i = 0;i <= ipcnts;i++)
			{
				u_long mask = ~0;
				mask = mask >> seg;
				mask = mask << seg;
				
				u_long  iplv = ip2long(buffer.c_str()) & mask; //NetWork ID  = IP & MASK

				iplv += i;
				char ipstr[16] ={0};
				long2ip(iplv,ipstr);

				if(true == checkIpFormat(ipstr))
				{
					ips.push_back(string(ipstr));
				}
				
			}
		}
		else
		{
			ips.push_back(buffer);
		}
	}
	else
	{
		if (true == checkIpFormat(this->ip))
		{
			ips.push_back(this->ip);
		}
	}
}

int CIPParse::pow2(int n)
{
	if(n == 0)
	{
		return 1;
	}

	return 2 * pow2(n - 1);	
}

int CIPParse::long2ip(u_long in,char* ipstr)
{
    u_char *p;
    p = (u_char*)(&in);

    char output[16] = {0};
    snprintf(output,16,"%d.%d.%d.%d",p[3],p[2],p[1],p[0]);
    memcpy(ipstr,output,strlen(output));

    return 0;
}

u_long CIPParse::ip2long(const char *ip)
{
    u_char addr[16];
    sscanf(ip,"%d.%d.%d.%d",addr,addr+1,addr+2,addr+3);
    u_long vl = ((u_long)addr[0])<<24 | ((u_long)addr[1])<<16 | ((u_long)addr[2])<<8 | (u_long)addr[3];

	return vl;
}
