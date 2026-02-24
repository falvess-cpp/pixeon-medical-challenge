// User-defined exception class

#include <iostream>
#include <string>

#include "fsdate.hpp"

using namespace std;

int main( )
{
	std::cout << __FILE__ << __func__ << __LINE__ << std::endl;
	FsDate fsDate;
	cout << endl << fsDate.today(DT_FORMAT::DATE) << endl;
	cout << endl << fsDate.today(DT_FORMAT::DATETIME) << endl;
	cout << endl << fsDate.today(DT_FORMAT::DATETIMEMS) << endl;
	cout << endl << fsDate.getDate(DT_FORMAT::DATE) << endl;
	cout << endl << fsDate.getDate(DT_FORMAT::DATETIME) << endl;
	cout << endl << fsDate.getDate(DT_FORMAT::DATETIMEMS) << endl;
	
	return 0 ;
}