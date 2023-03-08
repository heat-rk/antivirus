#pragma once

#include <stdio.h>
#include <chrono>

#include "MessageStruct.h"

namespace Antivirus {
	int64_t timeSinceEpochMillis();

	MessageStruct generateMessage(
		const char* source,
		const char* target,
		int8_t type,
		void* body
	);

	bool cmpstrs(char const* const target, char* current, int length);
}