#pragma once

#include <Windows.h>
#include <cstddef>
#include <functional>

#include "Utils.h"
#include "Message.h"
#include "ChannelCore.h"

namespace Antivirus {
    class ChannelClient {
    private:
        HANDLE m_inputPipe;
        HANDLE m_outputPipe;
    public:
        void init(int type);
        void listen(std::function<void(Message)> onMessage);
        void write(Message message);
        void disconnect();
    };
}
