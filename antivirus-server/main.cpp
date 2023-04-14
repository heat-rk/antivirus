#include <stdio.h>
#include <Windows.h>
#include <algorithm>

#include "Utils.h"
#include "Channel.h"
#include "StatusNotifier.h"
#include "MessageMethod.h"
#include "MessageStatus.h"
#include "ServiceManager.h"
#include "IncomingMessageBodyScan.h"
#include "LogWriter.h"
#include "ScannerCache.h"
#include "OutgoingMessageBodyScanLast.h"
#include "Message.h"

using namespace Antivirus;

wchar_t const* const SERVICE_NAME = TEXT("AntivirusService");

SERVICE_STATUS          g_ServiceStatus = { 0 };
SERVICE_STATUS_HANDLE   g_StatusHandle = NULL;
HANDLE                  g_ServiceStopEvent = INVALID_HANDLE_VALUE;

Channel                 channel;
StatusNotifier          statusNotifier;

void handleClientMessage(Message message) {
    if (cmpstrs(MessageMethod::E_GET_STATUS, message.method, sizeof(message.method))) {
        statusNotifier.handleIncomingMessage(message);
    } else if (cmpstrs(MessageMethod::E_SCAN_START, message.method, sizeof(message.method))) {
        IncomingMessageBodyScan body =
            incomingMessageBodyScanDeserializer.createFromBytes(message.body, sizeof(message.body));

        TCHAR commandLine[MAX_PATH];
        GetModuleFileName(NULL, commandLine, MAX_PATH);

        TCHAR newCommandLine[MAX_PATH];
        wcscpy_s(newCommandLine, commandLine);
        wcscat_s(newCommandLine, L" --scan ");
        wcscat_s(newCommandLine, body.path);

        STARTUPINFO si = { sizeof(si) };
        PROCESS_INFORMATION pi;

        if (CreateProcess(NULL, newCommandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    } else if (cmpstrs(MessageMethod::E_SCAN_LAST, message.method, sizeof(message.method))) {
        ScannerCache scannerCache;
        std::vector<std::wstring> viruses;
        scannerCache.load(&viruses);

        OutgoingMessageBodyScanLast body;
        body.virusesCount = viruses.size();
        body.pathLengths = new int32_t[body.virusesCount];
        body.paths = new wchar_t*[body.virusesCount];

        for (int i = 0; i < viruses.size(); i++) {
            auto virus = viruses[i];
            body.pathLengths[i] = virus.size();
            body.paths[i] = new wchar_t[body.pathLengths[i]];
            wcscpy_s(body.paths[i], body.pathLengths[i], virus.c_str());
        }

        Message message = generateMessage(
            message.method,
            message.uuid,
            MessageStatus::E_OK,
            &body
        );

        channel.write(message);
    }
}

void init() {
    channel.init();
    statusNotifier.setOutgoingMessagesHandler([](Message message) { channel.write(message); });
}

void destroy() {
    channel.disconnect();
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
        channel.listen([](Message message) { handleClientMessage(message); });
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
            OutputDebugString(TEXT("AntivirusService: ServiceMain: SetServiceStatus returned error"));
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
    destroy();

    CloseHandle(g_ServiceStopEvent);

    g_ServiceStatus.dwControlsAccepted = 0;
    g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    g_ServiceStatus.dwWin32ExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 3;

    if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE) {
        OutputDebugString(TEXT("AntivirusService: ServiceMain: SetServiceStatus returned error"));
    }
}

int wmain(int argc, wchar_t* argv[]) {
    if (argc - 1 == 0) {
        SERVICE_TABLE_ENTRY ServiceTable[] = {
            { const_cast<LPTSTR>(SERVICE_NAME), ServiceMain },
            { NULL, NULL }
        };

        if (StartServiceCtrlDispatcher(ServiceTable) == FALSE) {
            LogWriter::log("Service constructor failed with code %d.\n", GetLastError());
            init();
            HANDLE hThread = CreateThread(NULL, 0, ServiceWorkerThread, NULL, 0, NULL);
            WaitForSingleObject(hThread, INFINITE);
            destroy();
        }
    } else {
        ServiceManager sm(argv[0]);

        if (wcscmp(argv[1], L"--install") == 0) {
            sm.installService();
        }
        else if (wcscmp(argv[1], L"--uninstall") == 0) {
            sm.uninstallService();
        }
        else if (wcscmp(argv[1], L"--start") == 0) {
            sm.runService();
        }
        else if (wcscmp(argv[1], L"--load-base") == 0) {
            sm.loadBaseInput(argv[2]);
        }
        else if (wcscmp(argv[1], L"--scan") == 0) {
            sm.scan(argv[2]);
        }
        else if (wcscmp(argv[1], L"--logs") == 0) {
            sm.logs();
        }
    }

    return 0;
}
