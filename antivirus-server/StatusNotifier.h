#pragma once

#include <functional>

#include "MessageStruct.h"

namespace Antivirus {
	class StatusNotifier {
	private:
		std::function<void(MessageStruct)> outgoingMessagesHandler;
	public:
		void handleIncomingMessage(MessageStruct message);
		void setOutgoingMessagesHandler(std::function<void(MessageStruct)> onMessage);
	};
}
