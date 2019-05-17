#pragma once
#define POCO_STATIC
#include"config.hpp"
#include"ModbusRawHeader.h"
#include<initializer_list>
#include<deque>
#include<cstdio>
#include<tuple>
#include<mutex>


namespace xtd {
	namespace Net {
		namespace Modbus {
			enum class RegisterType : unsigned char {
				INPUT_REGISTER = 1,
				HOLDING_REGISTER = 2,
				DISCRETE_INPUTS = 3,
				COIL_REGISTER = 4
				
			};

			
			
			class DLL ModbusRegisters
			{
			public:
				ModbusRegisters(uint16_t irCount, uint16_t hrCount, uint16_t crCount);
				ModbusRegisters(std::pair<uint16_t, uint16_t> irParams, std::pair<uint16_t, uint16_t> hrParams,std::pair<uint16_t,uint16_t> diParams ,std::pair<uint16_t, uint16_t> crParams);
				~ModbusRegisters() = default;
				
				
				void setStartAddress(RegisterType type, uint16_t address);
				void setStartAddresses(uint16_t irStartAddress, uint16_t hrStartAddress, uint16_t diStartAddress, uint16_t crStartAddress);
				
				//SET Functions
				void setRegisterValues(RegisterType typr, uint16_t StartAddress, std::initializer_list<uint16_t> values);
				void setInputRegisterValues(uint16_t StartAddress, std::initializer_list < uint16_t> values);
				void setHoldingRegisterValues(uint16_t StartAddress, std::initializer_list<uint16_t> values);
				void setDiscreteInputs(uint16_t StartAddress, std::initializer_list<uint16_t> values);
				void setCoilValues(uint16_t StartAddress, std::initializer_list<uint16_t> values);


				void writeSingleRegister(uint16_t StartAddress, uint16_t value);
				void writeMultipleRegisters(uint16_t StartAddress, std::initializer_list<uint16_t> values);


				//GET Functions

				std::vector<ModbusRegister> getRegisterValues(RegisterType type, uint16_t StartAddress, std::size_t count);
				std::vector<ModbusRegister> getInputRegisterValues(uint16_t StartAddress, std::size_t count);
				std::vector<ModbusRegister> getHoldingRegisterValues(uint16_t StartAddress, std::size_t count);
				std::vector<ModbusRegister> getDiscreteInputs(uint16_t StartAddress, std::size_t count);
				std::vector<ModbusRegister> getCastedCoilValues(uint16_t StartAddress, std::size_t count);

				std::deque<bool> getCoilValues(uint16_t StartAddress, std::size_t count);

			private:
				uint16_t irStartAddress;
				std::mutex InputRegisterMutex;
				std::vector<ModbusRegister> InputRegisters;
				std::mutex HoldingRegisterMutex;
				uint16_t hrStartAddress;
				std::vector<ModbusRegister> HoldingRegisters;
				uint16_t diStartAddress;
				std::mutex DiscreteInputsMutex;
				std::vector<ModbusRegister> DiscreteInputs;
				uint16_t crStartAddress;
				//std::vector<ModbusCoil> CoilRegister;
				std::mutex CoilRegisterMutex;
				std::deque<bool> CoilRegister;

				//Public STATIC
			public:
				static std::deque<bool> CoilCast(std::vector<ModbusRegister> vec);
				static ModbusRegister RegisterCast(bool value);
			};
		}
	}
}

