#include "ModbusRegisters.h"

xtd::Net::Modbus::ModbusRegisters::ModbusRegisters(uint16_t irCount, uint16_t hrCount, uint16_t crCount) : InputRegisters(irCount, 0), HoldingRegisters(hrCount, 0), CoilRegister(crCount, 0)
{
}

xtd::Net::Modbus::ModbusRegisters::ModbusRegisters(std::pair<uint16_t, uint16_t> irParams, std::pair<uint16_t, uint16_t> hrParams, std::pair<uint16_t, uint16_t> diParams, std::pair<uint16_t, uint16_t> crParams) :
	InputRegisters(irParams.first,0) , irStartAddress(irParams.second), HoldingRegisters(hrParams.first), hrStartAddress(hrParams.second), DiscreteInputs(diParams.first),diStartAddress(diParams.second) ,CoilRegister(crParams.first), crStartAddress(crParams.second)
{
}

void xtd::Net::Modbus::ModbusRegisters::setStartAddress(RegisterType type, uint16_t address)
{
	switch (type) {
	case RegisterType::INPUT_REGISTER:
		this->irStartAddress = address;
		return;
	case RegisterType::HOLDING_REGISTER:
		this->hrStartAddress = address;
		return;
	case RegisterType::COIL_REGISTER:
		this->crStartAddress = address;
		return;
	default:
		return;
	}
}

void xtd::Net::Modbus::ModbusRegisters::setStartAddresses(uint16_t irStartAddress, uint16_t hrStartAddress, uint16_t diStartAddress,uint16_t crStartAddress)
{
	this->irStartAddress = irStartAddress;
	this->hrStartAddress = hrStartAddress;
	this->diStartAddress = diStartAddress;
	this->crStartAddress = crStartAddress;
}

void xtd::Net::Modbus::ModbusRegisters::setRegisterValues(RegisterType type, uint16_t StartAddress, std::initializer_list<uint16_t> values)
{
	switch (type) {
		case RegisterType::INPUT_REGISTER:
			this->setInputRegisterValues(StartAddress, values);
			return;
		case RegisterType::HOLDING_REGISTER:
			this->setHoldingRegisterValues(StartAddress, values);
			return;
		case RegisterType::DISCRETE_INPUTS:
			this->setDiscreteInputs(StartAddress, values);
			return;
		case RegisterType::COIL_REGISTER:
			this->setCoilValues(StartAddress, values);
			return;
		default:
			return;
	}
}

void xtd::Net::Modbus::ModbusRegisters::setInputRegisterValues(uint16_t StartAddress, std::initializer_list<uint16_t> values)
{
	std::lock_guard<std::mutex> guard(this->InputRegisterMutex);
	for (auto value : values) {
		this->InputRegisters[StartAddress - this->irStartAddress] = value;
		++StartAddress;
	}
}

void xtd::Net::Modbus::ModbusRegisters::setHoldingRegisterValues(uint16_t StartAddress, std::initializer_list<uint16_t> values)
{
	std::lock_guard<std::mutex> guard(this->HoldingRegisterMutex);
	for (auto value : values) {
		this->HoldingRegisters[StartAddress - this->hrStartAddress] = value;
		++StartAddress;
	}
}

void xtd::Net::Modbus::ModbusRegisters::setDiscreteInputs(uint16_t StartAddress, std::initializer_list<uint16_t> values)
{
	std::lock_guard<std::mutex> guard(this->DiscreteInputsMutex);
	for (auto value : values) {
		this->DiscreteInputs[StartAddress - this->hrStartAddress] = value;
		++StartAddress;
	}
}

void xtd::Net::Modbus::ModbusRegisters::setCoilValues(uint16_t StartAddress, std::initializer_list<uint16_t> values)
{
	std::lock_guard<std::mutex> guard(this->CoilRegisterMutex);
	for (auto value : values) {
		this->CoilRegister[StartAddress - this->crStartAddress] = static_cast<bool>(value);
		++StartAddress;
	}
}

void xtd::Net::Modbus::ModbusRegisters::writeSingleRegister(uint16_t StartAddress, uint16_t value)
{

}

void xtd::Net::Modbus::ModbusRegisters::writeMultipleRegisters(uint16_t StartAddress, std::initializer_list<uint16_t> values)
{
	if (StartAddress < this->hrStartAddress) {
		this->setInputRegisterValues(StartAddress, values);
		return;
	}
	if (StartAddress < this->diStartAddress) {
		this->setHoldingRegisterValues(StartAddress, values);
		return;
	}
	this->setDiscreteInputs(StartAddress, values);
}

std::vector<xtd::Net::Modbus::ModbusRegister> xtd::Net::Modbus::ModbusRegisters::getRegisterValues(RegisterType type, uint16_t StartAddress, std::size_t count)
{
	switch (type) {
	case RegisterType::INPUT_REGISTER:
		return this->getInputRegisterValues(StartAddress,count);
	case RegisterType::HOLDING_REGISTER:
		return this->getHoldingRegisterValues(StartAddress, count);
	case RegisterType::DISCRETE_INPUTS:
		return this->getDiscreteInputs(StartAddress, count);
	case RegisterType::COIL_REGISTER:
		return this->getCastedCoilValues(StartAddress, count);
	default:
		return std::initializer_list<ModbusRegister>();
	}
	
}

std::vector<xtd::Net::Modbus::ModbusRegister> xtd::Net::Modbus::ModbusRegisters::getInputRegisterValues(uint16_t StartAddress, std::size_t count)
{
	std::lock_guard<std::mutex> guard(this->InputRegisterMutex);
	std::vector<ModbusRegister> result;
	for (uint16_t i = 0; i < count; i++) {
		result.insert(result.end(), this->InputRegisters[StartAddress - this->irStartAddress]);
		++StartAddress;
	}
	return result;
}

std::vector<xtd::Net::Modbus::ModbusRegister> xtd::Net::Modbus::ModbusRegisters::getHoldingRegisterValues(uint16_t StartAddress, std::size_t count)
{
	std::lock_guard<std::mutex> guard(this->HoldingRegisterMutex);
	std::vector<ModbusRegister> result;
	for (uint16_t i = 0; i < count; i++) {
		result.insert(result.end(), this->HoldingRegisters[StartAddress - this->irStartAddress]);
		++StartAddress;
	}
	return result;
}

std::vector<xtd::Net::Modbus::ModbusRegister> xtd::Net::Modbus::ModbusRegisters::getDiscreteInputs(uint16_t StartAddress, std::size_t count)
{
	std::lock_guard<std::mutex> guard(this->DiscreteInputsMutex);
	std::vector<ModbusRegister> result;
	for (uint16_t i = 0; i < count; i++) {
		result.insert(result.end(), this->DiscreteInputs[StartAddress - this->irStartAddress]);
		++StartAddress;
	}
	return result;
}

std::vector<xtd::Net::Modbus::ModbusRegister> xtd::Net::Modbus::ModbusRegisters::getCastedCoilValues(uint16_t StartAddress, std::size_t count)
{
	std::lock_guard<std::mutex> guard(this->CoilRegisterMutex);
	std::vector<ModbusRegister> result;
	for (uint16_t i = 0; i < count; i++) {
		result.insert(result.end(), ModbusRegisters::RegisterCast(this->CoilRegister[StartAddress+1 - this->irStartAddress]));
		++StartAddress;
	}
	return result;
}

std::deque<bool> xtd::Net::Modbus::ModbusRegisters::getCoilValues(uint16_t StartAddress, std::size_t count)
{
	std::lock_guard<std::mutex> guard(this->CoilRegisterMutex);
	std::deque<bool> result;
	for (uint16_t i = 0; i < count; i++) {
		result.insert(result.end(), this->CoilRegister[StartAddress+1 - this->crStartAddress]);
		++StartAddress;
	}
	return result;
}

std::deque<bool> xtd::Net::Modbus::ModbusRegisters::CoilCast(std::vector<ModbusRegister> vec)
{
	std::deque<bool> result;
	for (auto item : vec) {
		result.insert(result.end(), item.test(0));
	}
	return result;
}

xtd::Net::Modbus::ModbusRegister xtd::Net::Modbus::ModbusRegisters::RegisterCast(bool value)
{
	ModbusRegister r;
	r.set(0, value);
	return r;
}
