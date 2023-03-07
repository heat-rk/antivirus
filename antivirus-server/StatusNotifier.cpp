#include <iostream>

#include "StatusNotifier.h"
#include "IncomingMessageTypes.h"
#include "OutgoingMessageTypes.h"
#include "OutgoingMessageBodyStatusStruct.h"
#include "MessagingParticipant.h"
#include "Utils.h"

using namespace Antivirus;

void StatusNotifier::handleIncomingMessage(MessageStruct message) {
	if (message.type == IncomingMessageType::E_STATUS_REQUEST) {
		OutgoingMessageBodyStatusStruct body;
		body.status = MessageStatus::E_OK;

		MessageStruct message = generateMessage(
			MessagingParticipant::E_SERVER_STATUS_NOTIFIER,
			MessagingParticipant::E_CLIENT_STATUS_RECEIVER,
			OutgoingMessageType::E_STATUS,
			&body
		);

		outgoingMessagesHandler(message);
	}
}

void StatusNotifier::setOutgoingMessagesHandler(std::function<void(MessageStruct)> onMessage) {
	this->outgoingMessagesHandler = onMessage;
}