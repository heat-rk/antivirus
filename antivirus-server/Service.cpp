#include "Service.h"

using namespace Antivirus;

bool isEquals(char const* const target, char* current, int length) {
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

void Service::handleClientMessage(MessageStruct message) {
    if (isEquals(MessagingParticipant::E_SERVER_STATUS_NOTIFIER, message.target, sizeof(message.target))) {
        statusNotifier.handleIncomingMessage(message);
    }
}

void Service::start() {
    statusNotifier.setOutgoingMessagesHandler([this](MessageStruct message) { channel.write(message); });
    channel.connect([this](MessageStruct message) { handleClientMessage(message); });
    scanner.start();
}