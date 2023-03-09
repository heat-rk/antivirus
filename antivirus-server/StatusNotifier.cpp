#include <iostream>

#include "StatusNotifier.h"
#include "MessageStatus.h"
#include "OutgoingMessageBodyStatus.h"
#include "MessageMethod.h"
#include "Utils.h"

using namespace Antivirus;

void StatusNotifier::handleIncomingMessage(Message incomingMessage) {
	if (cmpstrs(MessageMethod::E_GET_STATUS, incomingMessage.method, sizeof(incomingMessage.method))) {
		OutgoingMessageBodyStatus body;
		body.status = StatusValue::E_STATUS_OK;

		Message response = generateMessage(
			incomingMessage.method,
			incomingMessage.uuid,
			MessageStatus::E_OK,
			&body
		);

		outgoingMessagesHandler(response);
	}
}

void StatusNotifier::setOutgoingMessagesHandler(std::function<void(Message)> onMessage) {
	this->outgoingMessagesHandler = onMessage;
}