#include "Channel.h"
#include "Utils.h"
#include "ByteBuffer.h"
#include "LogWriter.h"

using namespace Antivirus;

DWORD WINAPI ChannelClientInputThread(LPVOID lpParam) {
    Channel::ThreadParams* params = reinterpret_cast<Channel::ThreadParams*>(lpParam);

    LogWriter::log(L"Channel:%ls: Client input connected.\n", params->wstrType);

    int8_t buffer[PIPE_BUFFSIZE];

    while (ReadFile(params->inputPipe, buffer, PIPE_BUFFSIZE, NULL, NULL) != FALSE) {
        int8_t* readedBytes;
        uint32_t readedBytesLength;

        Message message = messageDeserializer.createFromBytes(
            buffer,
            sizeof(buffer),
            &readedBytes,
            &readedBytesLength
        );

        LogWriter::log(L"Channel:%ls: Message received.\nChannel:%ls: Body -------------------------\n", params->wstrType, params->wstrType);
        LogWriter::log(L"Channel:%ls: Size of message in bytes: %d\n", params->wstrType, readedBytesLength);
        LogWriter::log(readedBytes, readedBytesLength);
        LogWriter::log(L"Channel:%ls: Body end ---------------------\n", params->wstrType);

        delete readedBytes;

        params->onMessage(message);
    }

    DisconnectNamedPipe(params->inputPipe);

    LogWriter::log(L"Channel:%ls: Client input disconnected.\n", params->wstrType);

    delete params;

    return ERROR_SUCCESS;
}

void Channel::init(int type) {
    m_type = type;
    m_wstrType = wstrOfChannelType(type);

    switch (type) {
    case CHANNEL_TYPE_EXTERNAL:
        m_inputPath = PIPE_SERVICE_INPUT_PATH;
        m_outputPath = PIPE_SERVICE_OUTPUT_PATH;
        break;
    case CHANNEL_TYPE_INTERNAL:
        m_inputPath = PIPE_SERVICE_INPUT_INTERNAL_PATH;
        m_outputPath = PIPE_SERVICE_OUTPUT_INTERNAL_PATH;
        break;
    default:
        LogWriter::log(L"Channel:%ls: Invalid channel type value = %d\n", m_wstrType, type);
        return;
    }
}

void Channel::handleInputClient(std::function<void(Message)> onMessage) {
    LogWriter::log(L"Channel:%ls: Waiting for client...\n", m_wstrType);

    HANDLE inputPipe = CreateNamedPipe(
        m_inputPath,
        PIPE_ACCESS_DUPLEX,
        PIPE_READMODE_BYTE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        0,
        PIPE_BUFFSIZE,
        NMPWAIT_USE_DEFAULT_WAIT,
        NULL
    );

    if (inputPipe == INVALID_HANDLE_VALUE) {
        LogWriter::log(L"Channel:%ls: pipe creation failed, GLE=%d.\n", m_wstrType, GetLastError());
        return;
    }

    if (ConnectNamedPipe(inputPipe, NULL) || GetLastError() == ERROR_PIPE_CONNECTED) {
        ThreadParams* threadParams = new ThreadParams;
        threadParams->inputPipe = inputPipe;
        threadParams->wstrType = m_wstrType;
        threadParams->onMessage = onMessage;

        HANDLE clientThread = CreateThread(
            NULL,
            0,
            ChannelClientInputThread,
            threadParams,
            0,
            NULL
        );

        if (clientThread == NULL) {
            LogWriter::log(L"Channel:%ls: Client input thread creation failed, GLE=%d.\n", m_wstrType, GetLastError());
            return;
        }

        CloseHandle(clientThread);
    } else {
        LogWriter::log(L"Channel:%ls: Client input connection failed, GLE=%d.\n", m_wstrType, GetLastError());
    }
}

void Channel::handleOutputClient() {
    HANDLE outputPipe = CreateNamedPipe(
        m_outputPath,
        PIPE_ACCESS_OUTBOUND,
        PIPE_TYPE_BYTE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        PIPE_BUFFSIZE,
        0,
        NMPWAIT_USE_DEFAULT_WAIT,
        NULL
    );

    if (outputPipe == INVALID_HANDLE_VALUE) {
        LogWriter::log(L"Channel:%ls: Output pipe creation failed, GLE=%d.\n", m_wstrType, GetLastError());
        return;
    }

    if (ConnectNamedPipe(outputPipe, NULL) || GetLastError() == ERROR_PIPE_CONNECTED) {
        m_outputClients.push_back(outputPipe);
    } else {
        LogWriter::log(L"Channel:%ls: Client output connection failed, GLE=%d.\n", m_wstrType, GetLastError());
    }
}

void Channel::write(Message message) {
    LogWriter::log(L"Channel:%ls: Writing...\n", m_wstrType);

    LogWriter::log(L"Channel:%ls: Body -------------------------\n", m_wstrType);

    ByteBuffer byteBuffer(0);
    message.write(&byteBuffer);

    int8_t bytes[PIPE_BUFFSIZE] = { 0 };
    byteBuffer.getInt8(bytes, byteBuffer.size());

    LogWriter::log(bytes, byteBuffer.size());

    LogWriter::log(L"Channel:%ls: Body end ---------------------\n", m_wstrType);

    for (int i = m_outputClients.size() - 1; i >= 0; i--) {
        HANDLE clientPipe = m_outputClients[i];

        if (clientPipe == INVALID_HANDLE_VALUE) {
            LogWriter::log(L"Channel:%ls: Write failed because of pipe INVALID_HANDLE_VALUE.\n", m_wstrType);
            continue;
        }

        if (WriteFile(clientPipe, bytes, PIPE_BUFFSIZE, NULL, NULL) == FALSE) {
            if (GetLastError() == ERROR_BROKEN_PIPE) {
                DisconnectNamedPipe(clientPipe);

                std::vector<HANDLE>::iterator position =
                    std::find(m_outputClients.begin(), m_outputClients.end(), clientPipe);

                if (position != m_outputClients.end()) {
                    m_outputClients.erase(position);
                }
            } else {
                LogWriter::log(L"Channel:%ls: Write failed, GLE=%d.\n", m_wstrType, GetLastError());
                continue;
            }
        }

        LogWriter::log(L"Channel:%ls: Writing succeed.\n", m_wstrType);
    }
}