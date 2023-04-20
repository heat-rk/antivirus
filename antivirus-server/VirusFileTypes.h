#pragma once

#include <cstdint>

namespace Antivirus {
	namespace FileType {
		const int8_t E_UNKNOWN = -1;
		const int8_t E_MZ = 0;

		int8_t SUPPORTED[] = { E_MZ };
	}
}