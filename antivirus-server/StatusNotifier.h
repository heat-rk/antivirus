#pragma once

#include <functional>

#include "Message.h"

namespace Antivirus {
	class StatusNotifier {
	private:
		std::function<void(Message)> m_outgoingMessagesHandler;
	public:
		void handleIncomingMessage(Message message);
		void setOutgoingMessagesHandler(std::function<void(Message)> onMessage);
	};
}
