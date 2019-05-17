#pragma once

#ifdef XTD_MB_TEST
	#define DLL /**/
#else
	#ifdef XTD_STATIC
		#define DLL /**/
	#else
		#ifdef XTD_EXPORT
			#define DLL __declspec(dllexport)
		#else
			#define DLL __declspac(dllimport)
		#endif
	#endif
#endif

#ifdef XTD_IMPORT
	#ifdef XTD_STATIC
		#ifdef _DEBUG
			#pragma comment(lib,"Modbus_static_debug.lib")
		#else
			#pragma comment(lib,"Modbus_static.lib")
		#endif
	#else
		#ifdef _DEBUG
			#pragma comment(lib,"Modbusd.lib")
		#else
			#pragma comment(lib,"Modbus.lib")
		#endif
	#endif
#endif

namespace xtd::Net::Modbus {

	enum class ErrorType : unsigned int {
		/*
			The function code received in the query is not an allowable action for the slave.
			This may be because the function code is only applicable to newer devices, and was not implemented in the unit selected.
			It could also indicate that the slave is in the wrong state to process a request of this type, for example because it is
			unconfigured and is being asked to return register values. If a Poll Program Complete command was issued, this code indicates
			that no program function preceded it.
		*/
		ILLEGAL_function = 0x01,
		/*
			The data address received in the query is not an allowable address for the slave. More specifically, the combination of reference
			number and transfer length is invalid. For a controller with 100 registers, a request with offset 96 and length 4 would succeed, a
			request with offset 96 and length 5 will generate exception 02.
		*/
		ILLEGAL_data_address = 0x02,
		/*
			A value contained in the query data field is not an allowable value for the slave.  This indicates a fault in the structure of remainder
			of a complex request, such as that the implied length is incorrect. It specifically does NOT mean that a data item submitted for storage
			in a register has a value outside the expectation of the application program, since the MODBUS protocol is unaware of the significance of
			any particular value of any particular register.
		*/
		ILLEGAL_data_value = 0x03,
		/*
			An unrecoverable error occurred while the slave was attempting to perform the requested action.
		*/
		slave_device_ERROR = 0x04,
		/*
			Specialized use in conjunction with programming commands.
			The slave has accepted the request and is processing it, but a long duration of time will be required to do so.  This response is returned to
			prevent a timeout error from occurring in the master. The master can next issue a Poll Program Complete message to determine if processing is completed.
		*/
		ACKNOLEDGE = 0x05,
		/*
			Specialized use in conjunction with programming commands.
			The slave is engaged in processing a long-duration program command.  The master should retransmit the message later when the slave is free..
		*/
		slave_device_BUSY = 0x06,
		/*
			The slave cannot perform the program function received in the query. This code is returned for an unsuccessful programming request using function code 13 or 14 decimal.
			The master should request diagnostic or error information from the slave.
		*/
		NEGATIVE_acknowledge = 0x07,
		/*
			Specialized use in conjunction with function codes 20 and 21 and reference type 6, to indicate that the extended file area failed to pass a consistency check.
			The slave attempted to read extended memory or record file, but detected a parity error in memory. The master can retry the request, but service may be required on the slave device.
		*/
		memory_parity_ERROR = 0x08,
		/*
			Specialized use in conjunction with gateways, indicates that the gateway was unable to allocate an internal communication path from the input port to the output port for processing
			the request. Usually means the gateway is misconfigured or overloaded.
		*/
		gateway_path_UNAVAILABLE = 0x0A,
		/*
			Specialized use in conjunction with gateways, indicates that no response was obtained from the target device. Usually means that the device is not present on the network.
		*/
		gateway_target_device_FAILED_TO_RESPONSE = 0x0B,

		/*
			NO ERROR
		*/
		modbus_OK = 0xFF
	};

	constexpr const char* mbError2str(ErrorType type) {
		switch (type) {
		case ErrorType::ILLEGAL_function:
			return "ILLEGAL_FUNCTION";
		case ErrorType::ILLEGAL_data_address:
			return "ILLEGAL_DATA_ADDRESS";
		case ErrorType::ILLEGAL_data_value:
			return "ILLEGAL_DATA_VALUE";
		case ErrorType::slave_device_ERROR:
			return "SLAVE_DEVICE_FAILURE";
		case ErrorType::ACKNOLEDGE:
			return "ACKNOLEDGE";
		case ErrorType::slave_device_BUSY:
			return "SLAVE_DEVICE_BUSY";
		case ErrorType::NEGATIVE_acknowledge:
			return "NEGATIVE_ACKNOWLEDGE";
		case ErrorType::memory_parity_ERROR:
			return "MEMORY_PARITY_ERROR";
		case ErrorType::gateway_path_UNAVAILABLE:
			return "GATEWAY_PATH_UNAVAILABLE";
		case ErrorType::gateway_target_device_FAILED_TO_RESPONSE:
			return "GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND";
		default:
			return "NO_ERROR_STRING";
		}
	}
}