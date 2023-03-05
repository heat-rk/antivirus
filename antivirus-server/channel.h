#pragma once

#include <Windows.h>
#include <cstddef>
#include <functional>

#include "utils.h"

using namespace std;

namespace Antivirus {
    class Channel {
    private:
        HANDLE inputPipe;
        HANDLE outputPipe;
        HANDLE pipeThread;
    public:
        Channel();
        void listen(std::function<void(MessageStruct)> onMessage);
        void write(MessageStruct message);
        void disconnect();
    };
}
