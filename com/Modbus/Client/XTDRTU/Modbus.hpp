#pragma once
#pragma comment(lib,"libs\\boost_date_time-vc141-mt-gd-x64-1_69.lib")
namespace xtd::com::Modbus {
	enum class ModbusError : uint8_t {
		MB_ERROR_SUCCESS = 0x00,
		MB_ILLEGAL_FUNCTION = 0x01,
		MB_ILLEGAL_DATA_ADDRESS = 0x02,
		MB_ILLEGAL_DATA_VALUE = 0x03,
		MB_SLAVE_DEVICE_FAILUTE = 0x04,
		MB_ACKNOWLEDGE = 0x05,
		MB_SLAVE_DEVICE_BUSY = 0x06,
		MB_NEGATIVE_ACKNOWLEDGE = 0x07,
		MB_MEMORY_PARITY_ERROR = 0x08,
		MB_GATEWAY_PATH_UNAVAILABLE = 0x0A,
		MB_GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND = 0x0B,
	};
}
