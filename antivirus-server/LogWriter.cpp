#include "LogWriter.h"
#include "LogConstants.h"

#include <stdio.h>
#include <string>

using namespace Antivirus;

HANDLE LogWriter::m_pipe = INVALID_HANDLE_VALUE;

void LogWriter::log(const wchar_t* format, ...) {
    va_list args;
    va_start(args, format);

    wchar_t message[PIPE_LOG_BUFFSIZE];
    vswprintf(message, PIPE_LOG_BUFFSIZE, format, args);

    openPipe();
    wprintf(message);
    
    if (WriteFile(m_pipe, message, PIPE_LOG_BUFFSIZE, NULL, NULL) != TRUE) {
        if (GetLastError() != ERROR_PIPE_LISTENING) {
            wprintf(L"[LogWriter] Write failed, GLE=%d.\n", GetLastError());
        }
    }

    va_end(args);
}

void LogWriter::log(const char* format, ...) {
    va_list args;
    va_start(args, format);

    char message[PIPE_LOG_BUFFSIZE];
    vsprintf_s(message, PIPE_LOG_BUFFSIZE, format, args);

    int len = strlen(message) + 1;
    wchar_t* wc_string = new wchar_t[len];

    MultiByteToWideChar(CP_UTF8, 0, message, -1, wc_string, len);

    openPipe();
    printf(message);

    if (WriteFile(m_pipe, wc_string, PIPE_LOG_BUFFSIZE, NULL, NULL) != TRUE) {
        if (GetLastError() != ERROR_PIPE_LISTENING) {
            wprintf(L"[LogWriter] Write failed, GLE=%d.\n", GetLastError());
        }
    }

    va_end(args);
    delete[] wc_string;
}

void LogWriter::log(int8_t* bytes, int32_t size) {
    openPipe();

    std::wstring bytesString;

    for (int i = 0; i < size; i++) {
        bytesString += std::to_wstring(bytes[i]) + L" ";
    }

    bytesString.push_back('\n');

    wchar_t message[PIPE_LOG_BUFFSIZE];
    wcscpy_s(message, PIPE_LOG_BUFFSIZE, bytesString.c_str());

    wprintf(message);

    if (WriteFile(m_pipe, message, PIPE_LOG_BUFFSIZE, NULL, NULL) != TRUE) {
        wprintf(L"[LogWriter] Write failed, GLE=%d.\n", GetLastError());
    }
}

void LogWriter::openPipe() {
    if (m_pipe != INVALID_HANDLE_VALUE) {
        return;
    }

    m_pipe = CreateNamedPipe(
        TEXT(PIPE_LOG_PATH),
        PIPE_ACCESS_OUTBOUND,
        PIPE_TYPE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        PIPE_LOG_BUFFSIZE,
        0,
        NMPWAIT_USE_DEFAULT_WAIT,
        NULL
    );

    if (m_pipe == INVALID_HANDLE_VALUE) {
        wprintf(L"[LogWriter] Pipe creation failed, GLE=%d.\n", GetLastError());
    }
}