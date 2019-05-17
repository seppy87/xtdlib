#define POCO_STATIC
#include"ModbusServer.h"
#include"ModbusClient.h"

#ifdef XTD_EXPORT
extern "C" {
	DLL void* createModbusServer(const char* ip, unsigned int port) {
		return new xtd::Net::Modbus::Server(ip, port);
	}
}
#endif

#ifndef XTD_EXPORT

int main()
{
	using namespace Poco::Net;
	using namespace xtd::Net::Modbus;
	initializeNetwork();
	ModbusClient client(Poco::Net::SocketAddress("127.0.0.1", 502));
	//auto x = client.readRequest(1, 0, 20);
	//std::cout << x.dump();
	std::cout << client.setRequest(15, 2, std::deque<bool>({ true,true,false,false }));
	//Server s(SocketAddress(IPAddress("127.0.0.1"), 502));
	//s.initRegisters({ 6,0 }, { 6,6 }, { 6,12 }, { 6,18 });
	//s.run();
	system("pause");
	return 0;
}
#endif // !XTD_EXPORT