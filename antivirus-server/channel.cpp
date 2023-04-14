#include "Channel.h"
#include "Utils.h"
#include "ByteBuffer.h"
#include "LogWriter.h"

using namespace Antivirus;

#define PIPE_SERVICE_INPUT_PATH "\\\\.\\pipe\\antivirus-pipe-service-input"
#define PIPE_SERVICE_OUTPUT_PATH "\\\\.\\pipe\\antivirus-pipe-service-output"
#define PIPE_BUFFSIZE 4096

void Channel::init() {
    m_inputPipe = CreateNamedPipe(
        TEXT(PIPE_SERVICE_INPUT_PATH),
        PIPE_ACCESS_INBOUND,
        PIPE_READMODE_BYTE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        0,
        PIPE_BUFFSIZE,
        NMPWAIT_USE_DEFAULT_WAIT,
        NULL
    );

    m_outputPipe = CreateNamedPipe(
        TEXT(PIPE_SERVICE_OUTPUT_PATH),
        PIPE_ACCESS_OUTBOUND,
        PIPE_TYPE_BYTE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        PIPE_BUFFSIZE,
        0,
        NMPWAIT_USE_DEFAULT_WAIT,
        NULL
    );

    if (m_inputPipe == INVALID_HANDLE_VALUE) {
        LogWriter::log(L"Input pipe creation failed, GLE=%d.\n", GetLastError());
        return;
    }

    if (m_outputPipe == INVALID_HANDLE_VALUE) {
        LogWriter::log(L"Output pipe creation failed, GLE=%d.\n", GetLastError());
        return;
    }
}

void Channel::listen(std::function<void(Message)> onMessage) {
    LogWriter::log(L"Waiting for client...\n");

    if (ConnectNamedPipe(m_inputPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED)) {
        if (ConnectNamedPipe(m_outputPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED)) {
            LogWriter::log(L"[OUTPUT] Client connected.\n");
        }
        else {
            LogWriter::log(L"[OUTPUT] Client connection failed, GLE=%d.\n", GetLastError());
        }

        LogWriter::log(L"[INPUT] Client connected.\n");

        int8_t buffer[PIPE_BUFFSIZE];

        while (ReadFile(m_inputPipe, buffer, PIPE_BUFFSIZE, NULL, NULL) != FALSE) {
            int8_t* readedBytes;
            uint32_t readedBytesLength;

            Message message = messageDeserializer.createFromBytes(
                buffer,
                sizeof(buffer),
                &readedBytes,
                &readedBytesLength
            );

            LogWriter::log(L"[INPUT] Message received.\n[INPUT] Body -------------------------\n");
            LogWriter::log(L"[INPUT] Size of message in bytes: %d\n", readedBytesLength);
            LogWriter::log(readedBytes, readedBytesLength);
            LogWriter::log(L"[INPUT] Body end ---------------------\n");

            delete readedBytes;

            onMessage(message);
        }

        DisconnectNamedPipe(m_inputPipe);
        DisconnectNamedPipe(m_outputPipe);

        LogWriter::log(L"[INPUT] Client disconnected.\n");
        LogWriter::log(L"[OUTPUT] Client disconnected.\n");
    }
    else {
        LogWriter::log(L"[INPUT] Client connection failed, GLE=%d.\n", GetLastError());
    }
}

void Channel::write(Message message) {
    LogWriter::log(L"Writing...\n");

    LogWriter::log(L"Body -------------------------\n");

    ByteBuffer byteBuffer(0);
    message.write(&byteBuffer);

    int8_t bytes[PIPE_BUFFSIZE] = { 0 };
    byteBuffer.getInt8(bytes, byteBuffer.size());

    LogWriter::log(bytes, byteBuffer.size());

    LogWriter::log(L"Body end ---------------------\n");

    if (m_outputPipe == INVALID_HANDLE_VALUE) {
        LogWriter::log(L"Write failed because of pipe INVALID_HANDLE_VALUE.\n");
        return;
    }

    if (WriteFile(m_outputPipe, bytes, PIPE_BUFFSIZE, NULL, NULL) == FALSE) {
        LogWriter::log(L"Write failed, GLE=%d.\n", GetLastError());
        return;
    }

    LogWriter::log(L"Writing succeed.\n");
}

void Channel::disconnect() {
    CloseHandle(m_inputPipe);
    CloseHandle(m_outputPipe);
}