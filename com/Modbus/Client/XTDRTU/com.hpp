#pragma once
#pragma comment(lib,"libs\\boost_date_time-vc141-mt-gd-x64-1_69.lib")
namespace xtd::com {
	enum class parity : unsigned int {
		NONE = 0,
		ODD = 1,
		EVEN = 2,
	};

	enum class stopBits : unsigned int{
		ONE = 1,
		TWO = 2,
		ONEPOINTFIVE = 3,
		
	};
}
