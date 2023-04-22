#pragma once

#include <Windows.h>
#include <cstddef>
#include <functional>

#include "Utils.h"
#include "Message.h"
#include "ChannelCore.h"

namespace Antivirus {
    class Channel {
    public:
        struct ThreadParams {
            HANDLE inputPipe;
            HANDLE outputPipe;
            const wchar_t* wstrType;
            std::function<void(Message)> onMessage;
            std::vector<HANDLE>* outputClients;
        };
    private:
        int m_type;
        const wchar_t* m_wstrType;
        const wchar_t* m_inputPath;
        const wchar_t* m_outputPath;
        std::vector<HANDLE> m_outputClients;
    public:
        void init(int type);
        void handleInputClient(std::function<void(Message)> onMessage);
        void handleOutputClient();
        void write(Message message);
    };
}
