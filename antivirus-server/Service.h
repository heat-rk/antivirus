#pragma once

#include "channel.h"
#include "scanner.h"
#include "StatusNotifier.h"
#include "MessagingParticipant.h"

using namespace Antivirus;

class Service {
private:
    Channel channel;
    Scanner scanner;
    StatusNotifier statusNotifier;

    void handleClientMessage(MessageStruct message);
public:
    void start();
};
