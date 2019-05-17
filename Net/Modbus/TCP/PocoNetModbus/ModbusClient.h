#pragma once
#include"ModbusRawHeader.h"

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"iphlpapi.lib")
#define POCO_STATIC
#include<poco/Net/StreamSocket.h>
#include<Poco/Net/SocketAddress.h>
#include"config.hpp"
#include"nlohmann/json.hpp"
#include<any>


namespace xtd::Net::Modbus {

	class ModbusClient : public Poco::Net::StreamSocket
	{
	public:
		ModbusClient(const Poco::Net::SocketAddress sa){
			this->connect(sa);
		}
		~ModbusClient() = default;

		nlohmann::json readRequest(uint16_t functionCode, uint16_t startingAddress, uint16_t numberOfValues);
		bool setRequest(uint16_t functionCode, uint16_t startingAddress, std::any valuesList);

	protected:
		ErrorType analyseResponse(char buffer[], std::size_t size);
		bool analyseSetResponse(char buffer[], std::size_t size);
		nlohmann::json processResponse(char *buffer, std::size_t size, std::size_t startingAddress, std::size_t count);
		nlohmann::json processReadCoils(char buffer[], std::size_t size, std::size_t startAddress, std::size_t count);

		bool setCoils(uint16_t functionCode, uint16_t startingAddress, std::deque<bool> values);

	private:
		uint16_t TransactionID = 0;
		const uint16_t ProtocolID = 0x0;
		const uint16_t UnitID = 0x1;
		std::function<void(void*,uint16_t)> OnErrorEvent;

	};
}
