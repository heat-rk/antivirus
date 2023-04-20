#include "LogWriter.h"
#include "LogConstants.h"

#include <stdio.h>
#include <string>

using namespace Antivirus;

HANDLE LogWriter::m_pipe = INVALID_HANDLE_VALUE;

void LogWriter::log(const wchar_t* format, ...) {
#ifdef _DEBUG
    va_list args;
    va_start(args, format);

    wchar_t message[PIPE_LOG_BUFFSIZE];
    vswprintf(message, PIPE_LOG_BUFFSIZE, format, args);

    wprintf(message);

    if (!openPipe()) {
        return;
    }
    
    if (WriteFile(m_pipe, message, PIPE_LOG_BUFFSIZE, NULL, NULL) != TRUE) {
        wprintf(L"[LogWriter] Write failed, GLE=%d.\n", GetLastError());
    }

    va_end(args);
#endif
}

void LogWriter::log(const char* format, ...) {
#ifdef _DEBUG
    va_list args;
    va_start(args, format);

    char message[PIPE_LOG_BUFFSIZE];
    vsprintf_s(message, PIPE_LOG_BUFFSIZE, format, args);

    int len = strlen(message) + 1;
    wchar_t* wc_string = new wchar_t[len];

    MultiByteToWideChar(CP_UTF8, 0, message, -1, wc_string, len);

    printf(message);

    if (!openPipe()) {
        return;
    }

    if (WriteFile(m_pipe, wc_string, PIPE_LOG_BUFFSIZE, NULL, NULL) != TRUE) {
        wprintf(L"[LogWriter] Write failed, GLE=%d.\n", GetLastError());
    }

    va_end(args);
    delete[] wc_string;
#endif
}

void LogWriter::log(int8_t* bytes, int32_t size) {
#ifdef _DEBUG
    std::wstring bytesString;

    for (int i = 0; i < size; i++) {
        bytesString += std::to_wstring(bytes[i]) + L" ";
    }

    bytesString.push_back('\n');

    wchar_t message[PIPE_LOG_BUFFSIZE];
    wcscpy_s(message, PIPE_LOG_BUFFSIZE, bytesString.c_str());

    wprintf(message);

    if (!openPipe()) {
        return;
    }

    if (WriteFile(m_pipe, message, PIPE_LOG_BUFFSIZE, NULL, NULL) != TRUE) {
        wprintf(L"[LogWriter] Write failed, GLE=%d.\n", GetLastError());
    }
#endif
}

bool LogWriter::openPipe() {
    if (m_pipe != INVALID_HANDLE_VALUE) {
        return true;
    }

    m_pipe = CreateFile(
        PIPE_LOG_PATH,
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    if (m_pipe == INVALID_HANDLE_VALUE) {
        if (GetLastError() != ERROR_FILE_NOT_FOUND) {
            wprintf(L"[LogWriter] Pipe connection failed, GLE=%d.\n", GetLastError());
        }

        return false;
    }

    return true;
}