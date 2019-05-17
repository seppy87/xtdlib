#include "ModbusServer.h"
#include"cast.hpp"
#include"genInitializerList.hpp"

void xtd::Net::Modbus::Server::initRegisters(std::pair<uint16_t, uint16_t> InputRegister, std::pair<uint16_t, uint16_t> HoldingRegister, std::pair< uint16_t, uint16_t> DiscreteInputs, std::pair<uint16_t, uint16_t> CoilRegister)
{
	this->registers = std::make_unique<ModbusRegisters>(InputRegister, HoldingRegister,DiscreteInputs, CoilRegister);
}

void xtd::Net::Modbus::Server::setCustomFunctionCode(uint16_t FunctionCode, ModbusCallback callback, bool overwriteIfExist)
{
	switch (FunctionCode) {
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 15:
	case 16:
		std::cerr << "Cannot override standard Codes!";
		throw std::exception("Cannot override Standard code = " + FunctionCode);
		break;
	default:
		if (overwriteIfExist) {
			this->CustomFunctionCodes[FunctionCode] = callback;
			return;
		}
		if (this->CustomFunctionCodes.count(FunctionCode) != 0) {
			throw std::exception("No Overwriting allowed");
		}
		this->CustomFunctionCodes.insert(std::make_pair(FunctionCode, callback));
	}
}

void xtd::Net::Modbus::Server::updateRegisters(RegisterType type, uint16_t StartAddress, std::initializer_list<uint16_t> values)
{
	this->registers->setRegisterValues(type, StartAddress, values);
}

void xtd::Net::Modbus::Server::run()
{
	this->listen();
	std::shared_ptr ss = std::make_shared<Poco::Net::StreamSocket>(this->acceptConnection());
	auto lam = [=](std::shared_ptr<Poco::Net::StreamSocket> s) {
		while (true) {
			try {
				char buffer[1024];
				std::memset(buffer, 0, sizeof(buffer));
				int bytes = s->receiveBytes(buffer, sizeof(buffer));
				if (bytes > 0) {
					std::cout << "Received Bytes=" << bytes << '\n';
					this->preparePacket(buffer, bytes);
					this->processRequest(*s);
				}
			}
			catch (Poco::Exception & ex) {
				std::cout << ex.displayText() << '\n';
				break;
			}
		}
	};
	auto shutdown = [=]() {ss->shutdown(); };
	this->ThreadPool.insert(this->ThreadPool.end(), std::make_pair<std::unique_ptr<std::thread>, std::function<void()>>(std::make_unique<std::thread>(lam, ss), shutdown));
}

void xtd::Net::Modbus::Server::reply(std::any data, Poco::Net::StreamSocket& socket)
{
	std::lock_guard<std::mutex> guard(this->HeaderMutex);
	std::vector<uint8_t> replyData;
	replyData.insert(replyData.end(), { rawHeader.transactionID[0],rawHeader.transactionID[1] }); //Transaktion ID
	replyData.insert(replyData.end(), { rawHeader.protocolID[0],rawHeader.protocolID[1] }); //ProtocolID
	uint8_t length[2];
	uint8_t UnitID = rawHeader.UnitIdentifier;
	uint8_t FunctionCode = rawHeader.FunctionCode;
	if (FunctionCode > 6) {
		length[0] = 2;
		replyData.insert(replyData.end(), { length[0],length[1],UnitID,FunctionCode });
		socket.sendBytes(&replyData[0], replyData.size());
		return;
	}
	if (FunctionCode == 1) {
		std::vector<uint8_t> coils;
		auto ret = std::any_cast<std::deque<bool>>(data);
		for (auto coil : ret)
		{
			coils.insert(coils.end(), coil);
			coils.insert(coils.end(), 0);
		}
		auto bytes = static_cast<uint8_t>(coils.size());
		length[0] = 2 + 1 + bytes;
		replyData.insert(replyData.end(), { length[0],length[1] });
		replyData.insert(replyData.end(), UnitID);
		replyData.insert(replyData.end(), FunctionCode);
		replyData.insert(replyData.end(), bytes);
		replyData.insert(replyData.end(), coils.begin(), coils.end());
		socket.sendBytes(&replyData[0], replyData.size());
		return;
	}
	auto registers = std::any_cast<std::vector<ModbusRegister>>(data);
	std::vector<uint8_t> body;
	for (auto value : registers) {
		auto ret = (xtd::uintCast<8>(static_cast<uint16_t>(value.to_ulong())).asStdVector<uint8_t>());
		body.insert(body.end(), ret[0]);
		body.insert(body.end(), ret[1]);
	}
	length[0] = 2 + 1 + body.size();
	replyData.insert(replyData.end(), { 0,length[0] });
	replyData.insert(replyData.end(), { UnitID,FunctionCode });
	replyData.insert(replyData.end(), static_cast<uint8_t>(body.size()));
	replyData.insert(replyData.end(), body.begin(), body.end());
	socket.sendBytes(&replyData[0], replyData.size());
}

void xtd::Net::Modbus::Server::ErrorReply(ErrorType type, Poco::Net::StreamSocket& socket)
{
	std::vector<uint8_t> data;
	data.insert(data.end(), { this->rawHeader.transactionID[0],this->rawHeader.transactionID[1] }); // Transaction ID
	data.insert(data.end(), { this->rawHeader.protocolID[0],this->rawHeader.protocolID[1] });	//Protocol ID
	data.insert(data.end(), { 0,4 });
	data.insert(data.end(), this->rawHeader.UnitIdentifier);
	data.insert(data.end(), this->rawHeader.FunctionCode);
	data.insert(data.end(), static_cast<unsigned int>(type));
	socket.sendBytes(&data[0], data.size());
}

void xtd::Net::Modbus::Server::preparePacket(char buffer[], std::size_t size)
{
	std::lock_guard<std::mutex> guard(this->HeaderMutex);
	this->rawHeader.data.clear();
	//Transaction ID
	this->rawHeader.transactionID = { static_cast<uint8_t>(buffer[0]),static_cast<uint8_t>(buffer[1]) };
	//Protocol ID
	this->rawHeader.protocolID = { static_cast<uint8_t>(buffer[2]),static_cast<uint8_t>(buffer[3]) };
	//Length
	this->rawHeader.Length = { static_cast<uint8_t>(buffer[4]),static_cast<uint8_t>(buffer[5]) };
	//Unit Identifier
	this->rawHeader.UnitIdentifier = static_cast<uint8_t>(buffer[6]);
	//Function Code
	this->rawHeader.FunctionCode = static_cast<uint8_t>(buffer[7]);
	//data
	for (std::size_t i = 8; i < size; i++) {
		this->rawHeader.data.insert(this->rawHeader.data.end(), buffer[i]);
	}
}

void xtd::Net::Modbus::Server::processRequest(Poco::Net::StreamSocket& socket)
{
	auto StartAddress = (xtd::uintCast<16>(this->rawHeader.data[0], this->rawHeader.data[1]).asValue<uint16_t>());
	auto size = (xtd::uintCast<16>(this->rawHeader.data[2], this->rawHeader.data[3]).asValue<uint16_t>());
	switch (this->rawHeader.FunctionCode) {
	case 1://REad Coils
		this->reply(this->registers->getCoilValues(StartAddress, size),socket);
		break;
	case 2: //read Discrete Inputs
		this->reply(this->registers->getDiscreteInputs(StartAddress, size), socket);
		break;
	case 4: //ReadInputRegisters
		this->updateRegisters(RegisterType::INPUT_REGISTER, 0, { 200,100,50 });
		this->reply(this->registers->getInputRegisterValues(StartAddress, size), socket);
		break;
	case 5: //Write Single Coil
	{
		this->registers->setCoilValues(StartAddress, { (xtd::uintCast<16>(this->rawHeader.data[5],this->rawHeader.data[6]).asValue<uint16_t>()) });
		this->reply(0, socket);
	}
		break;
	case 6: //write Single Register
		this->registers->writeSingleRegister(StartAddress, (xtd::uintCast<16>(this->rawHeader.data[5],this->rawHeader.data[6]).asValue<uint16_t>()));
		this->reply(0, socket);
		break;
	case 15://write multiple coils
	{
		std::vector<uint16_t> paravalues;
		for (uint16_t i = 5; i < this->rawHeader.data.size(); i = i+2) {
			paravalues.insert(paravalues.end(), (xtd::uintCast<16>(this->rawHeader.data[i], this->rawHeader.data[i + 1]).asValue<uint16_t>()));
		}
		this->registers->setCoilValues(StartAddress, range_to_initializer_list(paravalues.begin(),paravalues.end()));
		this->reply(0, socket);
	}
		break;
	case 16://write multiple registers
	{
		std::vector<uint16_t> paravalues;
		for (uint16_t i = 5; i < this->rawHeader.data.size(); i = i + 2) {
			paravalues.insert(paravalues.end(), (xtd::uintCast<16>(this->rawHeader.data[i], this->rawHeader.data[i + 1]).asValue<uint16_t>()));
		}
		this->registers->writeMultipleRegisters(StartAddress, range_to_initializer_list(paravalues.begin(), paravalues.end()));
		this->reply(0, socket);
	}
		break;
	default: //Custom Function Codes
		if (this->CustomFunctionCodes.count(this->rawHeader.FunctionCode) != 0) {
			auto result = this->CustomFunctionCodes[this->rawHeader.FunctionCode](this->rawHeader.data, this->rawHeader.FunctionCode);
		}
		else {
			this->ErrorReply(ErrorType::ILLEGAL_function, socket);
		}
		break;
	}
}


