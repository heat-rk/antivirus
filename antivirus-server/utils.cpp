#include "utils.h"
#include "MessageStruct.h"

using namespace Antivirus;

int64_t Antivirus::timeSinceEpochMillis() {
	using namespace std::chrono;
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

MessageStruct Antivirus::generateMessage(
	const char* source,
	const char* target,
	int8_t type,
	void* body
) {
	MessageStruct message;
	snprintf(message.source, sizeof(message.source), "%s", source);
	snprintf(message.target, sizeof(message.target), "%s", target);

	message.type = type;
	message.timestamp = timeSinceEpochMillis();

	int8_t* bytes = reinterpret_cast<int8_t*>(body);

	for (int i = 0; i < sizeof(message.body); i++) {
		message.body[i] = bytes[i];
	}

	return message;
}