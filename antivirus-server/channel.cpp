#include <stdio.h>
#include "channel.h"

using namespace std;
using namespace Antivirus;

#define PIPE_PATH "\\\\.\\pipe\\antivirus-pipe"
#define PIPE_BUFFSIZE 1024

struct PipeThreadArgument {
    Channel* channel;
    HANDLE pipe;
    void (*func)(Channel*, byte*);
};

DWORD WINAPI pipeThreadHandler(LPVOID lpvParam) {
    PipeThreadArgument* arg = (PipeThreadArgument*) lpvParam;

    while (arg->pipe != INVALID_HANDLE_VALUE) {
        printf("Waiting for client...\n");

        if (ConnectNamedPipe(arg->pipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED)) {
            printf("Client connected.\n");

            byte buffer[PIPE_BUFFSIZE];

            while (ReadFile(arg->pipe, buffer, PIPE_BUFFSIZE, NULL, NULL) != FALSE) {
                arg->func(arg->channel, buffer);
            }

            DisconnectNamedPipe(arg->pipe);

            printf("Client disconnected.\n");
        } else {
            printf("Client connection failed, GLE=%d.\n", GetLastError());
        }

        Sleep(1000);
    }

    free(arg);

    return 1;
}

Channel::Channel() {
    pipe = INVALID_HANDLE_VALUE;
    pipeThread = INVALID_HANDLE_VALUE;
}

void Channel::connect(void (*func)(Channel*, byte*)) {
    pipe = CreateNamedPipe(
        TEXT(PIPE_PATH),
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        PIPE_BUFFSIZE,
        PIPE_BUFFSIZE,
        NMPWAIT_USE_DEFAULT_WAIT,
        NULL
    );

    struct PipeThreadArgument* arg = (PipeThreadArgument*) malloc(sizeof(struct PipeThreadArgument));

    arg->channel = this;
    arg->func = func;
    arg->pipe = pipe;

    if (!arg) {
        return;
    }

    pipeThread = CreateThread(NULL, 0, pipeThreadHandler, (LPVOID)arg, 0, NULL);

    if (pipeThread == NULL) {
        printf("Client thread creation failed, GLE=%d.\n", GetLastError());
    }
}

void Channel::write(byte* bytes) {
    printf("Writing...\n");

    if (pipe == INVALID_HANDLE_VALUE) {
        printf("Write failed because of pipe INVALID_HANDLE_VALUE.\n");
        return;
    }

    if (WriteFile(pipe, bytes, PIPE_BUFFSIZE, NULL, NULL) == FALSE) {
        printf("Write failed, GLE=%d.\n", GetLastError());
        return;
    }

    printf("Writing succeed.\n");
}

void Channel::disconnect() {
    CloseHandle(pipe);
    CloseHandle(pipeThread);
}