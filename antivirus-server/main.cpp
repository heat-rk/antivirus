#include <stdio.h>

#include "Channel.h"
#include "StatusNotifier.h"
#include "MessagingParticipant.h"

using namespace Antivirus;

wchar_t const* const SERVICE_NAME = TEXT("AntivirusService");

SERVICE_STATUS          g_ServiceStatus = { 0 };
SERVICE_STATUS_HANDLE   g_StatusHandle = NULL;
HANDLE                  g_ServiceStopEvent = INVALID_HANDLE_VALUE;

Channel                 channel;
StatusNotifier          statusNotifier;

bool isEquals(char const* const target, char* current, int length) {
    for (int i = 0; i < length; i++) {
        if (target[i] == '\0') {
            break;
        }

        if (target[i] != current[i]) {
            return false;
        }
    }

    return true;
}

void handleClientMessage(MessageStruct message) {
    if (isEquals(MessagingParticipant::E_SERVER_STATUS_NOTIFIER, message.target, sizeof(message.target))) {
        statusNotifier.handleIncomingMessage(message);
    }
}

void init() {
    statusNotifier.setOutgoingMessagesHandler([](MessageStruct message) { channel.write(message); });
}

VOID WINAPI ServiceCtrlHandler(DWORD dwControl) {
    
    switch (dwControl) {
    case SERVICE_CONTROL_STOP:
        if (g_ServiceStatus.dwCurrentState != SERVICE_RUNNING) {
            break;
        }

        g_ServiceStatus.dwControlsAccepted = 0;
        g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        g_ServiceStatus.dwWin32ExitCode = 0;
        g_ServiceStatus.dwCheckPoint = 4;

        if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE) {
            OutputDebugString(TEXT("AntivirusService: ServiceCtrlHandler: SetServiceStatus returned error"));
        }

        SetEvent(g_ServiceStopEvent);

        break;
    case SERVICE_CONTROL_SHUTDOWN:
        if (g_ServiceStatus.dwCurrentState != SERVICE_RUNNING) {
            break;
        }

        g_ServiceStatus.dwControlsAccepted = 0;
        g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        g_ServiceStatus.dwWin32ExitCode = 0;
        g_ServiceStatus.dwCheckPoint = 4;

        if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE) {
            OutputDebugString(TEXT("AntivirusService: ServiceCtrlHandler: SetServiceStatus returned error"));
        }

        SetEvent(g_ServiceStopEvent);

        break;
    default:
        break;
    }
}

DWORD WINAPI ServiceWorkerThread(LPVOID lpParam) {
    while (WaitForSingleObject(g_ServiceStopEvent, 0) != WAIT_OBJECT_0) {
        channel.listen([](MessageStruct message) { handleClientMessage(message); });
    }

    return ERROR_SUCCESS;
}

VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv)
{
    DWORD Status = E_FAIL;

    g_StatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceCtrlHandler);

    if (g_StatusHandle == NULL) {
        return;
    }

    ZeroMemory(&g_ServiceStatus, sizeof(g_ServiceStatus));
    g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_ServiceStatus.dwControlsAccepted = 0;
    g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    g_ServiceStatus.dwWin32ExitCode = 0;
    g_ServiceStatus.dwServiceSpecificExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 0;

    if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE) {
        OutputDebugString(TEXT("AntivirusService: ServiceMain: SetServiceStatus returned error"));
    }

    init();

    g_ServiceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (g_ServiceStopEvent == NULL) {
        g_ServiceStatus.dwControlsAccepted = 0;
        g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        g_ServiceStatus.dwWin32ExitCode = GetLastError();
        g_ServiceStatus.dwCheckPoint = 1;

        if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE) {
            OutputDebugString(TEXT("My Sample Service: ServiceMain: SetServiceStatus returned error"));
        }

        return;
    }

    g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    g_ServiceStatus.dwWin32ExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 0;

    if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE) {
        OutputDebugString(TEXT("AntivirusService: ServiceMain: SetServiceStatus returned error"));
    }

    HANDLE hThread = CreateThread(NULL, 0, ServiceWorkerThread, NULL, 0, NULL);
    WaitForSingleObject(hThread, INFINITE);

    CloseHandle(g_ServiceStopEvent);

    g_ServiceStatus.dwControlsAccepted = 0;
    g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    g_ServiceStatus.dwWin32ExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 3;

    if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE) {
        OutputDebugString(TEXT("AntivirusService: ServiceMain: SetServiceStatus returned error"));
    }
}

int main() {
    SERVICE_TABLE_ENTRY ServiceTable[] = {
        { const_cast<LPTSTR>(SERVICE_NAME), ServiceMain },
        { NULL, NULL }
    };

    if (StartServiceCtrlDispatcher(ServiceTable) == FALSE) {
        printf("Service constructor failed with code %d.\n", GetLastError());
        init();
        HANDLE hThread = CreateThread(NULL, 0, ServiceWorkerThread, NULL, 0, NULL);
        WaitForSingleObject(hThread, INFINITE);
    }

    return 0;
}
