#pragma once

#include <Windows.h>
#include <cstddef>
#include "utils.h"

using namespace std;

namespace Antivirus {
    class Channel {
    private:
        HANDLE pipe;
        HANDLE pipeThread;
    public:
        Channel();
        void connect(void (*func)(Channel*, byte*));
        void write(byte* bytes);
        void disconnect();
    };
}
