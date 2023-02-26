#pragma once

#include <cstdint>

namespace Antivirus {
	class Message {
	private:
		Message();
	public:
		char sourceParticipant[32];
		char targetParticipant[32];
		int64_t timestamp;
	};
}