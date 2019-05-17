#pragma once
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"iphlpapi.lib")
#define POCO_STATIC
#include<poco/Net/ServerSocket.h>
#include<Poco/Net/SocketAddress.h>
#include"config.hpp"
#include<memory>
#include<map>
#include<functional>
#include"ModbusRegisters.h"
#include<any>
#include<mutex>
#include<thread>


namespace xtd {
	namespace Net {
		namespace Modbus {
			
			class DLL Server : public Poco::Net::ServerSocket
			{
			public:
				Server(const Poco::Net::SocketAddress& s) : Poco::Net::ServerSocket(s, true) {	
				}
				Server(const std::string& ip, uint16_t port) : Poco::Net::ServerSocket(Poco::Net::SocketAddress(ip, port), true) {

				}
				~Server() = default;

				void initRegisters(std::pair< uint16_t, uint16_t> InputRegister, std::pair< uint16_t, uint16_t> HoldingRegister, std::pair< uint16_t, uint16_t> DiscreteInputs, std::pair< uint16_t, uint16_t> CoilRegister);
				void setCustomFunctionCode(uint16_t FunctionCode, ModbusCallback callback, bool overwriteIfExist = false);
				void updateRegisters(RegisterType type, uint16_t StartAddress, std::initializer_list<uint16_t> values);
				void run();
				void reply(std::any data, Poco::Net::StreamSocket& socket);
				void ErrorReply(ErrorType type, Poco::Net::StreamSocket& socket);

			protected:
				void preparePacket(char buffer[], std::size_t size);
				void processRequest(Poco::Net::StreamSocket& socket);

				//FC

			private:
				std::mutex HeaderMutex;
				ModbusRawHeader rawHeader;
				std::mutex RegistersMutex;
				std::unique_ptr<ModbusRegisters> registers;
				bool condition = true;
				Poco::Net::SocketAddress client;
				std::map<int, ModbusCallback> CustomFunctionCodes;
				std::vector<std::pair<std::unique_ptr<std::thread>, std::function<void()>>> ThreadPool;
				//static functions
			public:
				//static uint8_t devideModbusRegister(ModbusRegister reg);
				//static ModbusRegister fuseModbusRegister(uint8_t x, uint8_t y);
			};
		}
	}
}
