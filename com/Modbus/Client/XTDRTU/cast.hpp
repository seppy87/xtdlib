#pragma once

#include<type_traits>
#include<tuple>
#include<cassert>
#include<memory>
#include<array>
#include<vector>

namespace xtd {
	template<size_t target>
	class uintCast {
	private:
		uint8_t primarBitSize;
		bool upcast = false;
		void* result = nullptr;
	public:
		explicit uintCast(uint8_t value_1, uint8_t value_2, uint8_t value_3 = 0, uint8_t value_4 = 0, uint8_t value_5 = 0, uint8_t value_6 = 0, uint8_t value_7 = 0, uint8_t value_8 =0) {
			//only upcast is possible!
			this->primarBitSize = 8;
					//FOR CLEANUP!
			switch (target) {
			case 8:
				return;
			case 16:
			{
				uint16_t* arr = new uint16_t;
				*arr = (uint16_t)(value_1 << 8) | (uint16_t)value_2;
				this->result = arr;
			}
			return;
			case 32:
			{
				auto firstPair = (xtd::uintCast<16>(value_1, value_2)).asValue<uint16_t>();
				auto secondPair = (xtd::uintCast<16>(value_3, value_4)).asValue<uint16_t>();
				uint32_t* arr = new uint32_t;
				*arr = (xtd::uintCast<32>(firstPair, secondPair).asValue<uint32_t>());
				this->result = arr;
			}
			return;
			case 64: {
				auto first16Pair = (xtd::uintCast<16>(value_1, value_2)).asValue<uint16_t>();
				auto second16Pair = (xtd::uintCast<16>(value_3, value_4)).asValue<uint16_t>();
				auto third16Pair = (xtd::uintCast<16>(value_5, value_6)).asValue<uint16_t>();
				auto fourth16Pair = (xtd::uintCast<16>(value_7, value_8)).asValue<uint16_t>();
				//to 32 Bit
				auto first32Pair = (xtd::uintCast<32>(first16Pair, second16Pair)).asValue<uint32_t>();
				auto second32Pair = (xtd::uintCast<32>(third16Pair, fourth16Pair)).asValue<uint32_t>();
				//to 64 Bit!
				uint64_t* arr = new uint64_t;
				*arr = (xtd::uintCast<64>(first32Pair, second32Pair)).asValue<uint64_t>();
			}
			return;
			default:
				return;
			}
		}

		explicit uintCast(uint16_t value_1, uint16_t value_2 = 0, uint16_t value_3 = 0, uint16_t value_4 = 0) {
			this->primarBitSize = 16;
			switch (target) {
			case 8: {
				uint8_t* arr = new uint8_t[2];
				arr[0] = (uint8_t)(value_1 >> 8);
				arr[1] = (uint8_t)(value_1 & 0xFFuL);
				this->result = arr;
			}
			return;
			case 16: return;
			case 32: {
				uint32_t* arr = new uint32_t;
				*arr = (uint32_t)(value_1 << 16) | (uint32_t)value_2;
				this->result = arr;
			}
			return;
			case 64:
			{
				auto first32Pair = (xtd::uintCast<32>(value_1, value_2)).asValue<uint32_t>();
				auto second32Pair = (xtd::uintCast<32>(value_3, value_4)).asValue<uint32_t>();
				uint64_t* arr = new uint64_t;
				*arr = (xtd::uintCast<64>(first32Pair, second32Pair)).asValue<uint64_t>();
				this->result = arr;
			}
			return;
			default: return;
			}
		}

		explicit uintCast(uint32_t value_1, uint32_t value_2 = 0) {
			this->primarBitSize = 32;
			switch (target) {
			case 8:
			{
				uint8_t* arr = new uint8_t[4];
				//first step 32 -> 16 Bit resulting in 2 Fields
				auto firstStep = (xtd::uintCast<16>(value_1)).asStdVector<uint16_t>();
				//second step 16 -> 8 bit resulting in 2x2 Fields;
				//High
				auto secondStepOne = (xtd::uintCast<8>(firstStep[0])).asStdVector<uint8_t>();
				auto secondStepTwo = (xtd::uintCast<8>(firstStep[1])).asStdVector<uint8_t>();
				arr[0] = secondStepOne[0];
				arr[1] = secondStepOne[1];
				arr[2] = secondStepTwo[0];
				arr[3] = secondStepTwo[1];
				this->result = arr;
			}
			return;
			case 16:
			{
				uint16_t* arr = new uint16_t[2];
				arr[0] = (uint16_t)(value_1 >> 16);
				arr[1] = (uint16_t)(value_1 & 0x0000FFFFuL);
				this->result = arr;
			}
			return;
			case 32: return;
			case 64:
			{
				uint64_t* arr = new uint64_t;
				*arr = ((uint64_t)(value_1 << 32)) | (uint64_t)value_2;
				this->result = arr;
			}
			return;
			default: return;
			}

		}

		explicit uintCast(uint64_t value) {
			//only downcast possible
			this->primarBitSize = 64;
			switch (target) {
			case 8: {
				//firststep 64->32 1x2 Fields;
				auto firststep = (xtd::uintCast<32>(value)).asStdVector<uint32_t>();
				//SecondStep 32->16 2x2 fields;
				auto secondStepOne = (xtd::uintCast<16>(firststep[0])).asStdVector<uint16_t>();
				auto secondStepTwo = (xtd::uintCast<16>(firststep[1])).asStdVector<uint16_t>();
				//thirdStep 16->8 4x2 fields;
				auto thirdStepOne = (xtd::uintCast<8>(secondStepOne[0])).asStdVector<uint8_t>();
				auto thirdStepTwo = (xtd::uintCast<8>(secondStepOne[1])).asStdVector<uint8_t>();
				auto thirdStepThree = (xtd::uintCast<8>(secondStepTwo[0])).asStdVector<uint8_t>();
				auto thirdStepFour = (xtd::uintCast<8>(secondStepTwo[1])).asStdVector<uint8_t>();
				uint8_t* arr = new uint8_t[8];
				arr[0] = thirdStepOne[0];
				arr[1] = thirdStepOne[1];
				arr[2] = thirdStepTwo[0];
				arr[3] = thirdStepTwo[1];
				arr[4] = thirdStepThree[0];
				arr[5] = thirdStepThree[1];
				arr[6] = thirdStepFour[0];
				arr[7] = thirdStepFour[1];
				this->result = arr;

			}
			return;
			case 16: {
				//firststep 64 -> 32 
				auto firstStep = (xtd::uintCast<32>(value)).asStdVector<uint32_t>();
				//second Step 32 -> 16 Bit
				auto secondStepOne = (xtd::uintCast<16>(firstStep[0])).asStdVector<uint16_t>();
				auto secondStepTwo = (xtd::uintCast<16>(firstStep[1])).asStdVector<uint16_t>();
				uint16_t* arr = new uint16_t[4];
				arr[0] = secondStepOne[0];
				arr[1] = secondStepOne[1];
				arr[2] = secondStepTwo[0];
				arr[3] = secondStepTwo[1];
				this->result = arr;
			}
			return;
			case 32: {
				uint32_t* arr = new uint32_t[2];
				arr[0] = (uint32_t)(value >> 32);
				arr[1] = (uint32_t)(value & 0x00000000FFFFFFFFuL);
				this->result = arr;
			}
			return;
			case 64: return;
			default: return;
			}
		}

		uintCast() = delete;
		uintCast(int, int = 0, int = 0, int = 0, int = 0, int = 0, int = 0, int = 0) = delete;

		~uintCast() {
			if (target > primarBitSize) this->upcast = true;
			else this->upcast = false;
			if (result != nullptr) {
				switch (target) {
				case 8:
					delete[]((uint8_t*)this->result);
					return;
				case 16:
					if (upcast) {
						delete ((uint16_t*)this->result);
						return;
					}
					delete[]((uint16_t*)this->result);
					return;
				case 32:
					if (upcast) {
						delete ((uint32_t*)this->result);
						return;
					}
					delete[]((uint32_t*)this->result);
					return;
				case 64:
					delete ((uint64_t*)this->result);
					return;
				default:
					return;
				}
			}
		}

		

		template<class T>
		T asValue() {
			return *((T*)this->result);
		}

		template<class T>
		std::vector<T> asStdVector() {
			auto fieldCount = primarBitSize / (sizeof(T) * 8);
			std::vector<T> res;
			for (uint8_t i = 0; i < fieldCount; i++) {
				res.insert(res.end(), ((T*)this->result)[i]);
			}
			return res;
		}
		
	};
}