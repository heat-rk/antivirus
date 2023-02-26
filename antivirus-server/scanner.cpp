#include <stdio.h>

#include "scanner.h"

using namespace Antivirus;
using namespace std;

DWORD WINAPI scannerThreadHandler(LPVOID lpvParam) {
	for (;;) {
		// do dome long running work
		Sleep(1000);
	}
}

Scanner::Scanner() {
	scannerThread = INVALID_HANDLE_VALUE;
}

void Scanner::start() {
	scannerThread = CreateThread(
        NULL,
        0,
        scannerThreadHandler,
        NULL,
        0,
        NULL
    );

    if (scannerThread == NULL) {
        printf("Client thread creation failed, GLE=%d.\n", GetLastError());
        return;
    }

    WaitForSingleObject(scannerThread, INFINITE);
}