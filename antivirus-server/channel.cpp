#include "Channel.h"
#include "Utils.h"

using namespace std;
using namespace Antivirus;

#define PIPE_SERVICE_INPUT_PATH "\\\\.\\pipe\\antivirus-pipe-service-input"
#define PIPE_SERVICE_OUTPUT_PATH "\\\\.\\pipe\\antivirus-pipe-service-output"
#define PIPE_BUFFSIZE 512

void printBytes(int8_t* bytes, int length) {
    for (int i = 0; i < length; i++) {
        printf("%d ", bytes[i]);
    }

    printf("\n");
}

Channel::Channel() {
    inputPipe = CreateNamedPipe(
        TEXT(PIPE_SERVICE_INPUT_PATH),
        PIPE_ACCESS_INBOUND,
        PIPE_READMODE_BYTE | PIPE_WAIT,
        1,
        0,
        PIPE_BUFFSIZE,
        NMPWAIT_USE_DEFAULT_WAIT,
        NULL
    );

    outputPipe = CreateNamedPipe(
        TEXT(PIPE_SERVICE_OUTPUT_PATH),
        PIPE_ACCESS_OUTBOUND,
        PIPE_TYPE_BYTE | PIPE_WAIT,
        1,
        PIPE_BUFFSIZE,
        0,
        NMPWAIT_USE_DEFAULT_WAIT,
        NULL
    );

    if (inputPipe == INVALID_HANDLE_VALUE) {
        printf("Input pipe creation failed, GLE=%d.\n", GetLastError());
        return;
    }

    if (outputPipe == INVALID_HANDLE_VALUE) {
        printf("Output pipe creation failed, GLE=%d.\n", GetLastError());
        return;
    }
}

void Channel::listen(std::function<void(MessageStruct)> onMessage) {
    printf("Waiting for client...\n");

    if (ConnectNamedPipe(inputPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED)) {
        if (ConnectNamedPipe(outputPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED)) {
            printf("[OUTPUT] Client connected.\n");
        }
        else {
            printf("[OUTPUT] Client connection failed, GLE=%d.\n", GetLastError());
        }

        printf("[INPUT] Client connected.\n");

        int8_t buffer[PIPE_BUFFSIZE];

        while (ReadFile(inputPipe, buffer, PIPE_BUFFSIZE, NULL, NULL) != FALSE) {
            printf("[INPUT] Message received.\n[INPUT] Body -------------------------\n");
            printf("[INPUT] Size of message in bytes: %d\n", sizeof(buffer));
            printBytes(buffer, sizeof(buffer));
            printf("[INPUT] Body end ---------------------\n");
            onMessage(*(MessageStruct*)buffer);
        }

        DisconnectNamedPipe(inputPipe);
        DisconnectNamedPipe(outputPipe);

        printf("[INPUT] Client disconnected.\n");
        printf("[OUTPUT] Client disconnected.\n");
    }
    else {
        printf("[INPUT] Client connection failed, GLE=%d.\n", GetLastError());
    }
}

void Channel::write(MessageStruct message) {
    printf("Writing...\n");

    printf("Body -------------------------\n");

    int8_t* bytes = reinterpret_cast<int8_t*>(&message);

    printf("Size of message in bytes: %d\n", sizeof(message));

    printBytes(reinterpret_cast<int8_t*>(&message), sizeof(message));

    printf("Body end ---------------------\n");

    if (outputPipe == INVALID_HANDLE_VALUE) {
        printf("Write failed because of pipe INVALID_HANDLE_VALUE.\n");
        return;
    }

    if (WriteFile(outputPipe, &message, PIPE_BUFFSIZE, NULL, NULL) == FALSE) {
        printf("Write failed, GLE=%d.\n", GetLastError());
        return;
    }

    printf("Writing succeed.\n");
}

void Channel::disconnect() {
    CloseHandle(inputPipe);
    CloseHandle(outputPipe);
    CloseHandle(pipeThread);
}