#include "CDBOption.h"

#include <iostream>
using std::cout;
using std::endl;

#include <string>
using std::string;

int main(int argc,char* argv[])
{
	CDBOption myDB("lixueming/lixueming@TT_1123");
	
	
	/*string create_table = "create table test(id int not null,name varchar(30))";
	if (false == myDB.ExecuteSql(create_table))
	{
		
	}
*/

	myDB.InsertName(2,"muhe");

	cout << myDB.GetName(2) << endl;

	//myDB.DeleteName(2);

	myDB.UpdateName(2,"mehe2");
	
	cout << myDB.GetName(2) << endl;

	
	return 0;
}
