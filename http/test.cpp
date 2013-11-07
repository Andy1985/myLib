#include <iostream>
using std::cout;
using std::endl;
using std::cerr;

#include "CHttp.h"
int main(int argc,char* argv[])
{
	CHttp myHttp(10);	
	string urlReturn("");
	if (myHttp.GetHttpRequestData("https://ers.trendmicro.com/reputations",
		string("_method=POST&data%5BReputation%5D%5Bip%5D=") + argv[1],urlReturn))
	{
		cout << urlReturn << endl;
	}

	return 0;
}
