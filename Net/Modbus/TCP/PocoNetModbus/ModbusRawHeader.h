#pragma once
#define POCO_STATIC
#include<vector>
#include<array>
#include<bitset>
#include<functional>


namespace xtd {
	namespace Net {
		namespace Modbus {
			using ModbusRegister = std::bitset<16>;
			using ModbusCoil = std::bitset<1>;
			using ModbusReplyBody = std::vector<uint8_t>;
			using ModbusCallback = std::function < ModbusReplyBody(const std::vector<uint8_t>& data, uint16_t FunctionCode)>;
			struct ModbusRawHeader {
				std::array<unsigned char, 2> transactionID;
				std::array<unsigned char, 2> protocolID;
				std::array<unsigned char, 2> Length;
				unsigned char UnitIdentifier;
				unsigned char FunctionCode;
				std::vector<unsigned char> data;
			};
		}
	}
}