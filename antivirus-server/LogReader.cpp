#include "LogReader.h"
#include "LogConstants.h"

#include <stdio.h>

using namespace Antivirus;

DWORD WINAPI LogReaderThread(LPVOID lpParam) {
    HANDLE* pipe = reinterpret_cast<HANDLE*>(lpParam);

    wchar_t* buffer[PIPE_LOG_BUFFSIZE];

    wprintf(L"LogReader: Connected\n");

    while (ReadFile(pipe, buffer, PIPE_LOG_BUFFSIZE, NULL, NULL) != FALSE) {
        wprintf(L"%ls", buffer);
    }

    DisconnectNamedPipe(pipe);

    wprintf(L"LogReader: Disconnected\n");

    return ERROR_SUCCESS;
}

void LogReader::connect() {
    HANDLE pipe;

    for (;;) {
        pipe = CreateNamedPipe(
            PIPE_LOG_PATH,
            PIPE_ACCESS_INBOUND,
            PIPE_TYPE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,
            0,
            PIPE_LOG_BUFFSIZE,
            NMPWAIT_USE_DEFAULT_WAIT,
            NULL
        );

        if (pipe == INVALID_HANDLE_VALUE) {
            wprintf(L"LogReader: Pipe creation failed, GLE=%d.\n", GetLastError());
            break;
        }

        if (ConnectNamedPipe(pipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED)) {
            HANDLE clientThread = CreateThread(
                NULL,
                0,
                LogReaderThread,
                (LPVOID) pipe,
                0,
                NULL
            );

            if (clientThread == NULL) {
                wprintf(L"LogReader: Client thread creation failed, GLE=%d.\n", GetLastError());
                return;
            }

            CloseHandle(clientThread);
        }
    }
}