#include "Header.h"
#include<bitset>
#include"cast.hpp"

xtd::com::Modbus::Header::Header(uint8_t buffer[], std::size_t size)
{
	if (size > 1) {
		std::bitset<8> bs(buffer[1]);
		if (bs.test(7)) {
			this->parseError(buffer, size);
			return;
		}
	}
	if (size < 1) return;
	this->SlaveID = buffer[0];
	if (size < 2) return;
	this->FunctionCode = buffer[1];
	if (size < 3) return;
	this->length = buffer[2];
	//this->data = new uint8_t[this->length];
	for (uint16_t i = 0; i < this->length; i++) {
		this->data.insert(data.end(),buffer[3 + i]);
	}
	uint16_t index = 3 + this->length;
	this->crc[0] = buffer[index];
	this->crc[1] = buffer[index + 1];
}

xtd::com::Modbus::Header::~Header()
{
	
}

std::vector<uint16_t> xtd::com::Modbus::Header::getResponseData() const
{
	std::vector<uint16_t> resp;
	for (uint16_t i = 0; i < this->data.size() - 1; i = i + 2) {
		resp.insert(resp.end(), xtd::uintCast<16>(this->data[i], this->data[i + 1]).asValue<uint16_t>());
	}
	return resp;
}

void xtd::com::Modbus::Header::parseError(uint8_t buffer[], std::size_t size)
{
	this->error = true;
	this->SlaveID = buffer[0];
	this->FunctionCode = buffer[1];
	this->errorCode = buffer[2];
	this->crc[0] = buffer[3];
	this->crc[1] = buffer[4];
}
