#pragma once

#include "Message.h"

#define STATUS_MESSAGE_OK 0
#define STATUS_MESSAGE_ERROR 1

namespace Antivirus {
	class ErrorMessage : public Message {
	public:
		char message[64];
	};
}