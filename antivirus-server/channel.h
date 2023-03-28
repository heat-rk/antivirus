#pragma once

#include <Windows.h>
#include <cstddef>
#include <functional>

#include "Utils.h"
#include "Message.h"

using namespace std;

namespace Antivirus {
    class Channel {
    private:
        HANDLE inputPipe;
        HANDLE outputPipe;
        HANDLE pipeThread;
    public:
        void init();
        void listen(std::function<void(Message)> onMessage);
        void write(Message message);
        void disconnect();
    };
}
