#include "ChannelClient.h"
#include "Utils.h"
#include "ByteBuffer.h"
#include "LogWriter.h"

using namespace Antivirus;

void ChannelClient::init(int type) {
    wchar_t* inputPath;
    wchar_t* outputPath;

    switch (type) {
    case CHANNEL_TYPE_EXTERNAL:
        inputPath = (wchar_t*)PIPE_SERVICE_OUTPUT_PATH;
        outputPath = (wchar_t*)PIPE_SERVICE_INPUT_PATH;
        break;
    case CHANNEL_TYPE_INTERNAL:
        inputPath = (wchar_t*)PIPE_SERVICE_OUTPUT_INTERNAL_PATH;
        outputPath = (wchar_t*)PIPE_SERVICE_INPUT_INTERNAL_PATH;
        break;
    default:
        LogWriter::log(L"ChannelClient: Invalid channel type value = %d\n", type);
        return;
    }

    m_inputPipe = CreateFile(
        inputPath,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    m_outputPipe = CreateFile(
        outputPath,
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    if (m_inputPipe == INVALID_HANDLE_VALUE) {
        LogWriter::log(L"ChannelClient: Input pipe creation failed, GLE=%d.\n", GetLastError());
        return;
    }

    if (m_outputPipe == INVALID_HANDLE_VALUE) {
        LogWriter::log(L"ChannelClient: Output pipe creation failed, GLE=%d.\n", GetLastError());
        return;
    }
}

void ChannelClient::listen(std::function<void(Message)> onMessage) {
    while (m_inputPipe != INVALID_HANDLE_VALUE) {
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

            LogWriter::log(L"ChannelClient: Message received.\nChannelClient: Body -------------------------\n");
            LogWriter::log(L"ChannelClient: Size of message in bytes: %d\n", readedBytesLength);
            LogWriter::log(readedBytes, readedBytesLength);
            LogWriter::log(L"ChannelClient: Body end ---------------------\n");

            delete readedBytes;

            onMessage(message);
        }
    }
}

void ChannelClient::write(Message message) {
    LogWriter::log(L"ChannelClient: Writing...\n");

    LogWriter::log(L"ChannelClient: Body -------------------------\n");

    ByteBuffer byteBuffer(0);
    message.write(&byteBuffer);

    int8_t bytes[PIPE_BUFFSIZE] = { 0 };
    byteBuffer.getInt8(bytes, byteBuffer.size());

    LogWriter::log(bytes, byteBuffer.size());

    LogWriter::log(L"ChannelClient: Body end ---------------------\n");

    if (m_outputPipe == INVALID_HANDLE_VALUE) {
        LogWriter::log(L"ChannelClient: Write failed because of pipe INVALID_HANDLE_VALUE.\n");
        return;
    }

    if (WriteFile(m_outputPipe, bytes, PIPE_BUFFSIZE, NULL, NULL) == FALSE) {
        if (GetLastError() != ERROR_PIPE_LISTENING) {
            LogWriter::log(L"ChannelClient: Write failed, GLE=%d.\n", GetLastError());
        }
        return;
    }

    LogWriter::log(L"ChannelClient: Writing succeed.\n");
}

void ChannelClient::disconnect() {
    if (m_inputPipe != INVALID_HANDLE_VALUE) {
        CloseHandle(m_inputPipe);
    }

    if (m_outputPipe != INVALID_HANDLE_VALUE) {
        CloseHandle(m_outputPipe);
    }
}