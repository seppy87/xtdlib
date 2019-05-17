#include "Client.hpp"
#include<iostream>
#include<boost/bind.hpp>
#include"cast.hpp"
#include<bitset>

WORD CRC16(const BYTE* nData, WORD wLength);

xtd::com::Modbus::Client::Client(const std::string& port, uint16_t baud, uint16_t databits, com::stopBits stopbit, com::parity parity) : port(this->io)
{
	using namespace boost;
	this->port.open(port);
	this->port.set_option(asio::serial_port_base::baud_rate(baud));
	this->port.set_option(asio::serial_port_base::character_size(databits));
	//STOPBIT!
	switch (stopbit) {
	case stopBits::ONE:
		this->port.set_option(asio::serial_port_base::stop_bits(asio::serial_port_base::stop_bits::one));
		break;
	case stopBits::ONEPOINTFIVE:
		this->port.set_option(asio::serial_port_base::stop_bits(asio::serial_port_base::stop_bits::onepointfive));
		break;
	case stopBits::TWO:
		this->port.set_option(asio::serial_port_base::stop_bits(asio::serial_port_base::stop_bits::two));
		break;
	default:
		std::cerr << "ERROR setting default\n";
		this->port.set_option(asio::serial_port_base::stop_bits(asio::serial_port_base::stop_bits::one));
	}
	//SET PARITY
	switch (parity) {
	case parity::NONE:
		this->port.set_option(asio::serial_port_base::parity(asio::serial_port_base::parity::none));
		break;
	case parity::EVEN:
		this->port.set_option(asio::serial_port_base::parity(asio::serial_port_base::parity::even));
		break;
	case parity::ODD:
		this->port.set_option(asio::serial_port_base::parity(asio::serial_port_base::parity::odd));
		break;
	default:
		std::cerr << "ERROR SETTING DEFAULTS\n";
		this->port.set_option(asio::serial_port_base::parity(asio::serial_port_base::parity::none));
	}
	//this->port.re
	//this->port.async_read_some(boost::asio::buffer(this->buffer, 256), boost::bind(&Client::handleReply, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

xtd::com::Modbus::Client::~Client()
{
	this->port.close();
}

std::deque<bool> xtd::com::Modbus::Client::ReadCoils(uint16_t startAddress, uint16_t count, uint8_t DeviceID)
{
	std::vector<uint8_t> packet{ DeviceID,1 };
	
	auto addr = xtd::uintCast<8>(startAddress).asStdVector<uint8_t>();
	auto number = xtd::uintCast<8>(count).asStdVector<uint8_t>();
	packet.insert(packet.end(), { addr[0],addr[1],number[0],number[1] });
	auto crc = CRC16(reinterpret_cast<const unsigned char*>(&packet[0]), packet.size());
	auto prepCRC = xtd::uintCast<8>(static_cast<uint16_t>(crc)).asStdVector<uint8_t>();
	packet.insert(packet.end(), { prepCRC[1],prepCRC[0] });
	//boost::system::error_code error;
	for (auto c : packet) {
		std::cout<< std::hex << (uint16_t)c;
	}
	std::cout << '\n';
	/*boost::asio::write(this->port, boost::asio::buffer(&packet[0], packet.size()), boost::asio::transfer_at_least(packet.size()), error);
	uint8_t buffer[256];
	memset(buffer, 0, sizeof(buffer));
	auto bytes = this->port.read_some(boost::asio::buffer(buffer, sizeof(buffer)));
	for (uint16_t i = 0; i < bytes; i++) {
		std::cout << (uint16_t)buffer[i];
	}
	std::cout << '\n';
	Header pack(buffer, bytes);*/
	auto pack = this->sendRequest(packet);
	std::vector<std::bitset<8>> vec;
	for (uint16_t i = 0; i < pack.getLength(); i++) {
		vec.insert(vec.end(),std::bitset<8>(pack[i]));
	}
	return Client::fromBitsetVector(vec);
}

std::deque<bool> xtd::com::Modbus::Client::ReadDiscreteInputs(uint16_t startAddress, uint16_t count, uint8_t DeviceID)
{
	std::vector<uint8_t> packet({ DeviceID,2 });
	auto addr = xtd::uintCast<8>(startAddress).asStdVector<uint8_t>();
	auto number = xtd::uintCast<8>(count).asStdVector<uint8_t>();
	packet.insert(packet.end(), { addr[0],addr[1],number[0],number[1] });
	auto tempCRC = CRC16(&packet[0], packet.size());
	auto crc = xtd::uintCast<8>(static_cast<uint16_t>(tempCRC)).asStdVector<uint8_t>();
	packet.insert(packet.end(), { crc[1],crc[0] });
	/*boost::system::error_code error;
	boost::asio::write(this->port, boost::asio::buffer(&packet[0], packet.size()), error);
	uint8_t buffer[256];
	memset(buffer, 0, sizeof(buffer));
	auto bytes = this->port.read_some(boost::asio::buffer(buffer, sizeof(buffer)));
	Header pack(buffer, bytes);*/
	auto pack = this->sendRequest(packet);
	std::vector<std::bitset<8>> vec;
	for (uint16_t i = 0; i < pack.getLength(); i++) {
		vec.insert(vec.end(), std::bitset<8>(pack[i]));
	}
	return Client::fromBitsetVector(vec);
}

std::vector<uint16_t> xtd::com::Modbus::Client::ReadHoldingRegisters(uint16_t startAddress, uint16_t count, uint8_t DeviceID)
{
	std::vector<uint8_t> packet({ DeviceID,3 });
	auto addr = xtd::uintCast<8>(startAddress).asStdVector<uint8_t>();
	auto num = xtd::uintCast<8>(count).asStdVector<uint8_t>();
	packet.insert(packet.end(), { addr[0],addr[1],num[0],num[1] });
	auto tempCRC = CRC16(&packet[0], packet.size());
	auto crc = xtd::uintCast<8>(static_cast<uint16_t>(tempCRC)).asStdVector<uint8_t>();
	packet.insert(packet.end(), { crc[1],crc[0] });
	auto pack = this->sendRequest(packet);
	std::vector<uint16_t> reg;
	for (uint16_t i = 0; i < pack.getLength(); i=i+2) {
		reg.insert(reg.end(), xtd::uintCast<16>(pack[i], pack[i + 1]).asValue<uint16_t>());
	}
	return std::vector<uint16_t>();
}

std::vector<uint16_t> xtd::com::Modbus::Client::ReadInputRegisters(uint16_t startAddress, uint16_t count, uint8_t DeviceID)
{
	std::vector<uint8_t> packet({ DeviceID,4 });
	auto addr = xtd::uintCast<8>(startAddress).asStdVector<uint8_t>();
	auto num = xtd::uintCast<8>(count).asStdVector<uint8_t>();
	packet.insert(packet.end(), { addr[0],addr[1],num[0],num[1] });
	auto tempCRC = CRC16(&packet[0], packet.size());
	auto crc = xtd::uintCast<8>(static_cast<uint16_t>(tempCRC)).asStdVector<uint8_t>();
	packet.insert(packet.end(), { crc[1],crc[0] });
	auto pack = this->sendRequest(packet);
	std::vector<uint16_t> reg;
	for (uint16_t i = 0; i < pack.getLength(); i = i + 2) {
		reg.insert(reg.end(), xtd::uintCast<16>(pack[i], pack[i + 1]).asValue<uint16_t>());
	}
	return reg;
}

xtd::com::Modbus::ModbusError xtd::com::Modbus::Client::WriteSingleCoil(uint16_t startAddress, bool value, uint8_t DeviceID)
{
	std::vector<uint8_t> packet({ DeviceID,5 });
	auto addr = xtd::uintCast<8>(startAddress).asStdVector<uint8_t>();
	packet.insert(packet.end(), { addr[0],addr[1] });
	if (value)
		packet.insert(packet.end(), { 255,0 });
	else
		packet.insert(packet.end(), {0, 255});
	auto tempCRC = CRC16(&packet[0], packet.size());
	auto crc = xtd::uintCast<8>(static_cast<uint16_t>(tempCRC)).asStdVector<uint8_t>();
	packet.insert(packet.end(), { crc[1], crc[0] });
	auto reply = this->sendRequest(packet);
	return static_cast<ModbusError>(reply.getErrorCode());
}

xtd::com::Modbus::ModbusError xtd::com::Modbus::Client::WriteSingleRegister(uint16_t startAddress, uint16_t value, uint8_t DeviceID)
{
	std::vector<uint8_t> packet({ DeviceID,6 });
	auto addr = xtd::uintCast<8>(startAddress).asStdVector<uint8_t>();
	auto val = xtd::uintCast<8>(value).asStdVector<uint8_t>();
	packet.insert(packet.end(), { addr[0],addr[1],val[0],val[1] });
	auto tempCRC = CRC16(&packet[0], packet.size());
	auto crc = xtd::uintCast<8>(static_cast<uint16_t>(tempCRC)).asStdVector<uint8_t>();
	packet.insert(packet.end(), { crc[1],crc[0] });
	auto reply = this->sendRequest(packet);
	return static_cast<ModbusError>(reply.getErrorCode());
}

xtd::com::Modbus::ModbusError xtd::com::Modbus::Client::WriteMultipleCoils(uint16_t startAddress, std::initializer_list<bool> values, uint8_t DeviceID)
{
	std::vector<uint8_t> packet({ DeviceID, 15 });
	auto addr = xtd::uintCast<8>(startAddress).asStdVector<uint8_t>();
	auto num = xtd::uintCast<8>(static_cast<uint16_t>(values.size())).asStdVector<uint8_t>();
	packet.insert(packet.end(), { addr[0],addr[1],num[0],num[1] });
	packet.insert(packet.end(), static_cast<uint8_t>(values.size()));
	for (auto value : values) {
		if (value) {
			packet.insert(packet.end(), { 255,0 });
		}
		else {
			packet.insert(packet.end(), { 0,255 });
		}
	}
	auto tempCRC = CRC16(&packet[0], packet.size());
	auto crc = xtd::uintCast<8>(static_cast<uint16_t>(tempCRC)).asStdVector<uint8_t>();
	packet.insert(packet.end(), { crc[1],crc[0] });
	auto reply = this->sendRequest(packet);
	return static_cast<ModbusError>(reply.getErrorCode());
}

xtd::com::Modbus::ModbusError xtd::com::Modbus::Client::WriteMultipleRegisters(uint16_t startAddress, std::initializer_list<uint16_t> values, uint8_t DeviceID)
{
	std::vector<uint8_t> packet({ DeviceID,16 });
	auto addr = xtd::uintCast<8>(startAddress).asStdVector<uint8_t>();
	auto num = xtd::uintCast<8>(static_cast<uint16_t>(values.size())).asStdVector<uint8_t>();
	packet.insert(packet.end(), { addr[0],addr[1],num[0],num[1],static_cast<uint8_t>(values.size()*2) });
	for (auto value : values) {
		auto v = xtd::uintCast<8>(value).asStdVector<uint8_t>();
		packet.insert(packet.end(), { v[0],v[1] });
	}
	auto tempCRC = CRC16(&packet[0], packet.size());
	auto crc = xtd::uintCast<8>(static_cast<uint16_t>(tempCRC)).asStdVector<uint8_t>();
	packet.insert(packet.end(), { crc[1],crc[0] });
	auto reply = this->sendRequest(packet);
	return static_cast<ModbusError>(reply.getErrorCode());
}

xtd::com::Modbus::ModbusError xtd::com::Modbus::Client::InvokeCustomFunctionCode(std::vector<uint16_t>& reply, uint8_t FunctionCode, uint16_t startAddress, std::initializer_list<uint16_t> values, uint8_t DeviceID)
{
	std::vector<uint8_t> packet({ DeviceID, FunctionCode });
	auto addr = xtd::uintCast<8>(startAddress).asStdVector<uint8_t>();
	auto num = xtd::uintCast<8>(values.size()).asStdVector<uint8_t>();
	packet.insert(packet.end(), { addr[0],addr[1],num[0],num[1],static_cast<uint8_t>(values.size() * 2) });
	for (auto value : values) {
		auto v = xtd::uintCast<8>(value).asStdVector<uint8_t>();
		packet.insert(packet.end(), { v[0],v[1] });
	}
	auto tempCRC = CRC16(&packet[0], packet.size());
	auto crc = xtd::uintCast<8>(static_cast<uint16_t>(tempCRC)).asStdVector<uint8_t>();
	packet.insert(packet.end(), { crc[1],crc[0] });
	auto res = this->sendRequest(packet);
	reply = res.getResponseData();
	return static_cast<ModbusError>(res.getErrorCode());
}



xtd::com::Modbus::Header xtd::com::Modbus::Client::sendRequest(const std::vector<uint8_t>& data) 
{
	boost::system::error_code error;
	boost::asio::write(this->port, boost::asio::buffer(&data[0], data.size()), error);
	uint8_t buffer[256];
	memset(buffer, 0, sizeof(buffer));
	auto bytes = this->port.read_some(boost::asio::buffer(buffer, sizeof(buffer)));
	return Header(buffer,bytes);
}



void xtd::com::Modbus::Client::fill2CorrectFormat(std::string& str)
{
	if (str.size() > 4) throw std::runtime_error("String too long");
	while (str.size() < 4) {
		str = "0" + str;
	}
}

std::deque<bool> xtd::com::Modbus::Client::fromBitsetVector(const std::vector<std::bitset<8>>& vec)
{
	std::deque<bool> ret;
	for (auto bs : vec) {
		for (uint16_t i = 0; i < 8; i++) {
			ret.insert(ret.end(), bs.test(i));
		}
	}
	return ret;
}
