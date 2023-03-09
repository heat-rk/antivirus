#include "Utils.h"
#include "Message.h"

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