#include "Utils.h"
#include "Message.h"

#include <sstream>
#include <Windows.h>

using namespace Antivirus;

int64_t Antivirus::timeSinceEpochMillis() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

Message Antivirus::generateMessage(
	char* method,
	char* uuid,
	int8_t status,
	Serializable* body
) {
	Message message;

	snprintf(message.method, sizeof(message.method), "%s", method);
	snprintf(message.uuid, sizeof(message.uuid), "%s", uuid);

	message.status = status;
	message.timestamp = timeSinceEpochMillis();

	ByteBuffer byteBuffer(0);
	body->write(&byteBuffer);
	byteBuffer.getInt8(&message.body, byteBuffer.size());

	return message;
}

Message Antivirus::generateMessage(
	char* method,
	int8_t status,
	Serializable* body
) {
	Message message;

	snprintf(message.method, sizeof(message.method), "%s", method);

	message.status = status;
	message.timestamp = timeSinceEpochMillis();

	ByteBuffer byteBuffer(0);
	body->write(&byteBuffer);
	byteBuffer.getInt8(&message.body, byteBuffer.size());

	return message;
}

Message Antivirus::generateMessage(
	char* method,
	int8_t status
) {
	Message message;

	snprintf(message.method, sizeof(message.method), "%s", method);

	message.status = status;
	message.timestamp = timeSinceEpochMillis();

	return message;
}

bool Antivirus::cmpstrs(char const* const target, char* current, int length) {
	for (int i = 0; i < length; i++) {
		if (target[i] == '\0') {
			break;
		}

		if (target[i] != current[i]) {
			return false;
		}
	}

	return true;
}

bool Antivirus::isFileExist(wchar_t* filename) {
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, filename, -1, NULL, 0, NULL, NULL);
	char* str = new char[size_needed]; 
	WideCharToMultiByte(CP_UTF8, 0, filename, -1, str, size_needed, NULL, NULL);
	bool isExists = GetFileAttributesA(str) != DWORD(-1);
	delete[] str;
	return isExists;
};

int8_t Antivirus::toInt8(std::string str) {
	return std::stoi(str);
}

int32_t Antivirus::toInt32(std::string str) {
	return static_cast<int32_t>(std::stoul(str));
}