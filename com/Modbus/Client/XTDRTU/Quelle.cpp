
#pragma comment(lib,"libs\\boost_date_time-vc141-mt-gd-x64-1_69.lib")
#include"Client.hpp"
#include<iostream>
#include <boost/exception/all.hpp>
using namespace xtd::com::Modbus;

int main(int argc, char** argv) {
	Client cl("COM3", 19200, 8);
	//auto r = cl.ReadHoldingRegisters(0, 3);
	auto r = cl.WriteMultipleRegisters(1, { 20,30,40 });
	
	::system("pause");

	return ERROR_SUCCESS;
}