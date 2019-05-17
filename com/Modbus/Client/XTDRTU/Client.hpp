#pragma once
#include"dllexport.hpp"
#include<boost/asio/serial_port.hpp>
#include<boost/asio.hpp>
#include<string>
#include<array>
#include<deque>
#include"com.hpp"
#include"Modbus.hpp"
#include"Header.h"
#include<bitset>

namespace xtd::com::Modbus {
	

	class DLL Client
	{
	public:
		Client(const std::string& port, uint16_t baud, uint16_t databits, com::stopBits stopbit = com::stopBits::ONE, com::parity parity = com::parity::NONE);
		~Client();

		//READ
		std::deque<bool> ReadCoils(uint16_t startAddress, uint16_t count, uint8_t DeviceID = 1);
		std::deque<bool> ReadDiscreteInputs(uint16_t startAddress, uint16_t count, uint8_t DeviceID = 1);
		std::vector<uint16_t> ReadHoldingRegisters(uint16_t startAddress, uint16_t count, uint8_t DeviceID = 1);
		std::vector<uint16_t> ReadInputRegisters(uint16_t startAddress, uint16_t count, uint8_t DeviceID = 1);

		//write
		ModbusError WriteSingleCoil(uint16_t startAddress, bool value, uint8_t DeviceID = 1);
		ModbusError WriteSingleRegister(uint16_t startAddress, uint16_t value, uint8_t DeviceID = 1);
		ModbusError WriteMultipleCoils(uint16_t startAddress, std::initializer_list<bool> values, uint8_t DeviceID = 1);
		ModbusError WriteMultipleRegisters(uint16_t startAddress, std::initializer_list<uint16_t> values, uint8_t DeviceID = 1);
		ModbusError InvokeCustomFunctionCode(std::vector<uint16_t>& reply, uint8_t FunctionCode, uint16_t startAddress, std::initializer_list<uint16_t> values, uint8_t DeviceID = 1);

	private:
		boost::asio::io_service io;
		boost::asio::serial_port port;
		//uint8_t buffer[256];
		//uint8_t DeviceID = 1;

		Header sendRequest(const std::vector<uint8_t>& data);
		

	public:
		static std::string to_hex(uint16_t number) {
			std::stringstream stream;
			stream << std::hex << number;
			return stream.str();
		}
		static void fill2CorrectFormat(std::string& str);
		static std::deque<bool> fromBitsetVector(const std::vector<std::bitset<8>>& vec);
	};

}