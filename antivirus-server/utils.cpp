#include "Utils.h"
#include "Message.h"

#include <sstream>
#include <Windows.h>

using namespace Antivirus;
using namespace std::chrono;

int64_t Antivirus::timeSinceEpochMillis() {
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
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

	ByteBuffer byteBuffer(sizeof(message.body));
	body->write(&byteBuffer);
	byteBuffer.getInt8(message.body, sizeof(message.body));

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

	ByteBuffer byteBuffer(sizeof(message.body));
	body->write(&byteBuffer);
	byteBuffer.getInt8(message.body, sizeof(message.body));

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

void Antivirus::printBytes(int8_t* bytes, int length) {
	for (int i = 0; i < length; i++) {
		printf("%d ", bytes[i]);
	}

	printf("\n");
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