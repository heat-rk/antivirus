#include "LogReader.h"
#include "LogConstants.h"

#include <stdio.h>

using namespace Antivirus;

void LogReader::connect() {
    HANDLE m_pipe = INVALID_HANDLE_VALUE;

    for (;;) {
        if (m_pipe == INVALID_HANDLE_VALUE) {
            m_pipe = CreateFile(
                TEXT(PIPE_LOG_PATH),
                GENERIC_READ,
                0,
                NULL,
                OPEN_EXISTING,
                0,
                NULL
            );
        }

        if (m_pipe == INVALID_HANDLE_VALUE) {
            Sleep(10);
            continue;
        }

        wchar_t* buffer[PIPE_LOG_BUFFSIZE];

        wprintf(L"[LogReader] Connected\n");

        while (ReadFile(m_pipe, buffer, PIPE_LOG_BUFFSIZE, NULL, NULL) != FALSE) {
            wprintf(L"%ls", buffer);
        }

        DisconnectNamedPipe(m_pipe);

        m_pipe = INVALID_HANDLE_VALUE;

        wprintf(L"[LogReader] Disconnected\n");
    }
}