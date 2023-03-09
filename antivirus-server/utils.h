#pragma once

#include <stdio.h>
#include <chrono>

#include "Message.h"

namespace Antivirus {
	int64_t timeSinceEpochMillis();

	Message generateMessage(
		char* method,
		char* uuid,
		int8_t status,
		Serializable* body
	);

	bool cmpstrs(char const* const target, char* current, int length);

	void printBytes(int8_t* bytes, int length);
}