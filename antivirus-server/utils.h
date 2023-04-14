#pragma once

#include <stdio.h>
#include <chrono>
#include <string>

#include "Message.h"

namespace Antivirus {
	int64_t timeSinceEpochMillis();

	Message generateMessage(
		char* method,
		char* uuid,
		int8_t status,
		Serializable* body
	);

	Message generateMessage(
		char* method,
		int8_t status,
		Serializable* body
	);

	Message generateMessage(
		char* method,
		int8_t status
	);

	bool cmpstrs(char const* const target, char* current, int length);

	void printBytes(int8_t* bytes, int length);

	bool isFileExist(wchar_t* filename);

	int8_t toInt8(std::string str);
	int32_t toInt32(std::string str);
}