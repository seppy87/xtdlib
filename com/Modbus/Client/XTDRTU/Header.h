#pragma once
#include"dllexport.hpp"
#include<string>
#include<cstdint>
#include<array>
#include<vector>

namespace xtd::com::Modbus {
	class DLL Header
	{
	public:
		Header(uint8_t buffer[], std::size_t size);
		Header() = delete;
		Header(const Header&) = delete;
		~Header();

		uint8_t getSlaveID() const {
			return this->SlaveID;
		}

		uint8_t getFunctionCode() const {
			return this->FunctionCode;
		}

		std::size_t getLength() const {
			return this->length;
		}

		std::pair<uint8_t, uint8_t> getCRC() const {
			return { this->crc[0],this->crc[1] };
		}

		uint8_t operator[](uint16_t index) const {
			if (index >= this->length) throw std::runtime_error("Index of of Boundaries");
			return this->data[index];
		}

		bool isFailed() const noexcept{
			return error;
		}

		uint8_t getErrorCode() const noexcept {
			return this->errorCode;
		}

		std::vector<uint16_t> getResponseData() const;

	protected:
		void parseError(uint8_t buffer[], std::size_t size);

	private:
		bool error = false;
		uint8_t SlaveID;
		uint8_t FunctionCode;
		uint8_t errorCode = 0;
		std::size_t length;
		std::array<uint8_t, 2> crc;
		std::vector<uint8_t> data;
	};

}