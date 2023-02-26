#pragma once

#include <cstdint>

namespace Antivirus {
	struct MessageStruct {
		char source[32];
		char target[32];
		int64_t timestamp;
		int8_t type;
		int8_t body[256];
	};
}
