#pragma once

#include <cstdint>

namespace Antivirus {
	namespace MessageStatus {
		const int8_t E_OK = 0;
		const int8_t E_ERROR = 1;
	}

	struct OutgoingMessageBodyStatusStruct {
		int8_t status;
	};
}