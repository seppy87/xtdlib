#include"Client.hpp"
#include"dllexport.hpp"
#include"com.hpp"

extern "C" {
	DLL void* CreateModbusClient(const char* port, unsigned int baud, unsigned int bits, unsigned int stopBits, unsigned int parity) {
		return new xtd::com::Modbus::Client(port, baud, bits, static_cast<xtd::com::stopBits>(stopBits), static_cast<xtd::com::parity>(parity));
	}
}