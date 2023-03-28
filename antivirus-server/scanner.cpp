#include <stdio.h>

#include "Scanner.h"

using namespace Antivirus;
using namespace std;

DWORD WINAPI scannerThreadHandler(LPVOID lpvParam) {
	for (;;) {
		// do dome long running work
		Sleep(1000);
	}
}

Scanner::Scanner() {
	m_scannerThread = INVALID_HANDLE_VALUE;
    m_isScanning = false;
}

void Scanner::start(char* path) {
	m_scannerThread = CreateThread(
        NULL,
        0,
        scannerThreadHandler,
        NULL,
        0,
        NULL
    );

    if (m_scannerThread == NULL) {
        printf("Client thread creation failed, GLE=%d.\n", GetLastError());
        return;
    }

    WaitForSingleObject(m_scannerThread, INFINITE);
}

void Scanner::pause() {

}

void Scanner::stop() {

}

bool Scanner::isScanning() {
    return false;
}