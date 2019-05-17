#include "ModbusClient.h"
#include"cast.hpp"

nlohmann::json xtd::Net::Modbus::ModbusClient::readRequest(uint16_t functionCode, uint16_t startingAddress, uint16_t numberOfValues)
{
	++this->TransactionID;
	std::vector<uint8_t> data;
	auto transid = xtd::uintCast<8>(this->TransactionID).asStdVector<uint8_t>();
	data.insert(data.end(), { transid[0],transid[1] });
	data.insert(data.end(), { 0,0 });
	data.insert(data.end(), { 0,6 });
	data.insert(data.end(), static_cast<uint8_t>(this->UnitID));
	data.insert(data.end(), static_cast<uint8_t>(functionCode));
	
	auto sAddr = xtd::uintCast<8>(startingAddress).asStdVector<uint8_t>();
	data.insert(data.end(), { sAddr[0],sAddr[1] });
	auto number = xtd::uintCast<8>(numberOfValues).asStdVector<uint8_t>();
	data.insert(data.end(), { number[0],number[1] });
	this->sendBytes(&data[0], data.size());
	char buffer[128];
	auto bytes = this->receiveBytes(buffer, sizeof(buffer));
	buffer[bytes] = '\0';
	auto ret = this->analyseResponse(buffer, bytes);
	if (ret == ErrorType::modbus_OK)
		return this->processResponse(buffer, bytes, startingAddress,numberOfValues);

	//in case of Error
	nlohmann::json ejson;
	ejson["request"] = functionCode;
	ejson["StartingAddress"] = startingAddress;
	ejson["NumberOfValues"] = numberOfValues;
	ejson["ErrorCode"] = static_cast<unsigned int>(ret);
	ejson["ErrorString"] = mbError2str(ret);
	return ejson;
}

bool xtd::Net::Modbus::ModbusClient::setRequest(uint16_t functionCode, uint16_t startingAddress, std::any valuesList)
{
	if (functionCode == 5 || functionCode == 15)
		return this->setCoils(functionCode, startingAddress, std::any_cast<std::deque<bool>>(valuesList));
	std::vector<uint16_t> values = std::any_cast<std::vector<uint16_t>>(valuesList);
	++this->TransactionID;
	std::vector<uint8_t> data;
	auto transid = xtd::uintCast<8>(this->TransactionID).asStdVector<uint8_t>();
	data.insert(data.end(), { transid[0],transid[1] });
	data.insert(data.end(), { 0,0 });
	uint16_t length = 2 + 2 + values.size()*2;
	auto len = xtd::uintCast<8>(length).asStdVector<uint8_t>();
	data.insert(data.end(), { len[0],len[1] });
	data.insert(data.end(), this->UnitID);
	data.insert(data.end(), functionCode);
	auto sAddr = xtd::uintCast<8>(startingAddress).asStdVector<uint8_t>();
	data.insert(data.end(), { sAddr[0],sAddr[1] });
	data.insert(data.end(), { 0, (unsigned char)values.size() });
	data.insert(data.end(), values.size()*2);
	//data.insert(data.end(), 0);
	for (auto value : values) {
		auto v = xtd::uintCast<8>(value).asStdVector<uint8_t>();
		data.insert(data.end(), { v[0],v[1] });
	}
	
	this->sendBytes(&data[0], data.size());
	char buffer[512];
	memset(buffer, 0, sizeof(buffer));
	auto bytes = this->receiveBytes(buffer, sizeof(buffer));
	buffer[bytes] = '\0';
	
	return this->analyseSetResponse(buffer, bytes);
}

xtd::Net::Modbus::ErrorType xtd::Net::Modbus::ModbusClient::analyseResponse(char buffer[], std::size_t size)
{
	if (size > 9)
		return ErrorType::modbus_OK;
	return static_cast<ErrorType>(buffer[size - 1]);
}

bool xtd::Net::Modbus::ModbusClient::analyseSetResponse(char buffer[], std::size_t size)
{
	if (size != 9) return true;
	return false;
}

nlohmann::json xtd::Net::Modbus::ModbusClient::processResponse(char* buffer, std::size_t size, std::size_t startingAddress, std::size_t count)
{
	if (static_cast<uint16_t>(buffer[6]) == 1)
		return this->processReadCoils(buffer, size, startingAddress,count);
	nlohmann::json obj;
	obj["Header"]["TransactionID"] = this->TransactionID;
	obj["Header"]["ProtocolID"] = 0;
	obj["Header"]["Length"] = xtd::uintCast<16>(static_cast<uint8_t>(buffer[4]), static_cast<uint8_t>(buffer[5])).asValue<uint16_t>();
	obj["Header"]["UnitID"] = 1;
	obj["Header"]["FunctionCode"] = static_cast<uint8_t>(buffer[6]);
	obj["FieldCount"] = buffer[7];
	for (uint16_t i = 9; i < size - 1; i = i + 2) {
		obj[std::to_string(startingAddress)] = xtd::uintCast<16>(static_cast<uint8_t>(buffer[i]), static_cast<uint8_t>(buffer[i+1])).asValue<uint16_t>();
		++startingAddress;
	}

	return obj;
}

nlohmann::json xtd::Net::Modbus::ModbusClient::processReadCoils(char buffer[], std::size_t size, std::size_t startAddress, std::size_t count)
{
	nlohmann::json obj;
	obj["Header"]["TransactionID"] = this->TransactionID;
	obj["Header"]["ProtocolID"] = 0;
	obj["Header"]["Length"] = xtd::uintCast<16>(static_cast<uint8_t>(buffer[4]), static_cast<uint8_t>(buffer[5])).asValue<uint16_t>();
	obj["Header"]["UnitID"] = 1;
	obj["Header"]["FunctionCode"] = static_cast<uint8_t>(buffer[6]);
	obj["FieldCount"] = buffer[8];
	unsigned int bytes = obj["FieldCount"].get<unsigned int>();
	for (uint16_t i = 0; i < bytes; i++) {
		std::bitset<8> b(static_cast<uint8_t>(buffer[9 + i]));
		for (uint16_t j = 0; j < (count-8*i) && j < 8; j++) {
			obj["Byte" + std::to_string(i)]["Bit" + std::to_string(j)] = b.test(j);
		}
	}
	return obj;
}

bool xtd::Net::Modbus::ModbusClient::setCoils(uint16_t functionCode, uint16_t startingAddress, std::deque<bool> values)
{
	++this->TransactionID;
	std::vector<std::bitset<8>> coils;
	for (int i = 0; i < values.size(); i=i+8) {
		std::bitset<8> bs(0);
		for (int j = 0; j < 8; j++) {
			if ((i + j) > values.size() - 1) break;
			bs.set(j, values[i + j]);
		}
		coils.insert(coils.end(), bs);
	}
	std::vector<uint8_t> data;
	auto transid = xtd::uintCast<8>(this->TransactionID).asStdVector<uint8_t>();
	data.insert(data.end(), {transid[0],transid[1]});
	data.insert(data.end(), { 0,0 });
	uint16_t length = 4+ coils.size()+2;
	auto len = xtd::uintCast<8>(length).asStdVector<uint8_t>();
	data.insert(data.end(), { len[0],len[1] });
	//data.insert(data.end(), static_cast<uint8_t>(this->UnitID));
	data.insert(data.end(), { static_cast<uint8_t>(this->UnitID),static_cast<uint8_t>(functionCode) });
	data.insert(data.end(), {0,static_cast<uint8_t>(startingAddress),0,static_cast<uint8_t>(values.size()),1});
	for (auto coil : coils) {
		//auto val = xtd::uintCast<8>(static_cast<uint16_t>(coil.to_ulong())).asStdVector<uint8_t>();
		data.insert(data.end(), static_cast<uint8_t>(coil.to_ulong()));
	}
	this->sendBytes(&data[0], data.size());
	char buffer[512];
	memset(buffer, 0, sizeof(buffer));
	int bytes = 0;
	try {
		bytes = this->receiveBytes(buffer, sizeof(buffer));
	}
	catch (Poco::Exception & ex) {
		std::cerr << ex.displayText() << '\n';
	}
	return this->analyseSetResponse(buffer, bytes);
}
