#include <stdio.h>
#include <Windows.h>
#include <algorithm>

#include "Utils.h"
#include "Channel.h"
#include "MessageMethod.h"
#include "MessageStatus.h"
#include "ServiceManager.h"
#include "IncomingMessageBodyScan.h"
#include "LogWriter.h"
#include "ScannerCache.h"

using namespace Antivirus;

wchar_t const* const SERVICE_NAME = TEXT("AntivirusService");

SERVICE_STATUS          g_ServiceStatus = { 0 };
SERVICE_STATUS_HANDLE   g_StatusHandle = NULL;
HANDLE                  g_ServiceStopEvent = INVALID_HANDLE_VALUE;

Channel                 channel;
Channel                 internalChannel;

void handleClientMessage(Message message) {
    if (cmpstrs(MessageMethod::E_SCAN_START, message.method, sizeof(message.method))) {
        IncomingMessageBodyScan body =
            incomingMessageBodyScanDeserializer.createFromBytes(message.body);

        TCHAR commandLine[MAX_PATH];
        GetModuleFileName(NULL, commandLine, MAX_PATH);

        TCHAR newCommandLine[MAX_PATH];
        wcscpy_s(newCommandLine, commandLine);
        wcscat_s(newCommandLine, L" --scan ");
        wcscat_s(newCommandLine, body.path.c_str());

        STARTUPINFO si = { sizeof(si) };
        PROCESS_INFORMATION pi;

        if (CreateProcess(NULL, newCommandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    }
}

void init() {
    ScannerCache scannerCache;
    scannerCache.validate();

    channel.init(CHANNEL_TYPE_EXTERNAL);
    internalChannel.init(CHANNEL_TYPE_INTERNAL);
}

void handleServiceStop() {
    Message stopMessage = generateMessage(
        (char*) MessageMethod::E_SCAN_STOP,
        MessageStatus::E_REQUEST
    );

    internalChannel.write(stopMessage);

    if (g_ServiceStatus.dwCurrentState == SERVICE_STOPPED ||
        g_ServiceStatus.dwCurrentState == SERVICE_STOP_PENDING) {
        return;
    }

    g_ServiceStatus.dwControlsAccepted = 0;
    g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    g_ServiceStatus.dwWin32ExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 4;

    if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE) {
        LogWriter::log(L"AntivirusService: ServiceCtrlHandler: SetServiceStatus returned error\n");
    }

    SetEvent(g_ServiceStopEvent);
}

VOID WINAPI ServiceCtrlHandler(DWORD dwControl) {
    switch (dwControl) {
    case SERVICE_CONTROL_STOP:
        handleServiceStop();
        break;
    case SERVICE_CONTROL_SHUTDOWN:
        handleServiceStop();
        break;
    default:
        break;
    }
}

DWORD WINAPI ExternalChannelInputThread(LPVOID lpParam) {
    for (;;) {
        channel.handleInputClient([](Message message) { 
            handleClientMessage(message);
            internalChannel.write(message);
        });
    }

    return ERROR_SUCCESS;
}

DWORD WINAPI ExternalChannelOutputThread(LPVOID lpParam) {
    for (;;) {
        channel.handleOutputClient();
    }

    return ERROR_SUCCESS;
}

DWORD WINAPI InternalChannelInputThread(LPVOID lpParam) {
    for (;;) {
        internalChannel.handleInputClient([](Message message) {
            channel.write(message);
        });
    }

    return ERROR_SUCCESS;
}

DWORD WINAPI InternalChannelOutputThread(LPVOID lpParam) {
    for (;;) {
        internalChannel.handleOutputClient();
    }

    return ERROR_SUCCESS;
}

void startServiceThreads() {
    HANDLE internalInputThread = CreateThread(NULL, 0, InternalChannelInputThread, NULL, 0, NULL);

    if (internalInputThread == NULL) {
        LogWriter::log(L"AntivirusService: ServiceMain: Internal input thread creation failed, GLE=%d\n", GetLastError());
    }

    HANDLE internalOutputThread = CreateThread(NULL, 0, InternalChannelOutputThread, NULL, 0, NULL);

    if (internalOutputThread == NULL) {
        LogWriter::log(L"AntivirusService: ServiceMain: Internal output thread creation failed, GLE=%d\n", GetLastError());
    }

    HANDLE externalInputThread = CreateThread(NULL, 0, ExternalChannelInputThread, NULL, 0, NULL);

    if (externalInputThread == NULL) {
        LogWriter::log(L"AntivirusService: ServiceMain: External input thread creation failed, GLE=%d\n", GetLastError());
    }

    HANDLE externalOutputThread = CreateThread(NULL, 0, ExternalChannelOutputThread, NULL, 0, NULL);

    if (externalOutputThread == NULL) {
        LogWriter::log(L"AntivirusService: ServiceMain: External output thread creation failed, GLE=%d\n", GetLastError());
    }

    if (WaitForSingleObject(g_ServiceStopEvent, INFINITE) == WAIT_FAILED) {
        LogWriter::log(L"AntivirusService: ServiceMain: Waiting for threads failed, GLE=%d\n", GetLastError());
    }

    LogWriter::log(L"ldgldkfgd\n");
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
        LogWriter::log(L"AntivirusService: ServiceMain: SetServiceStatus returned error\n");
    }

    init();

    g_ServiceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (g_ServiceStopEvent == NULL) {
        g_ServiceStatus.dwControlsAccepted = 0;
        g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        g_ServiceStatus.dwWin32ExitCode = GetLastError();
        g_ServiceStatus.dwCheckPoint = 1;

        if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE) {
            LogWriter::log(L"AntivirusService: ServiceMain: SetServiceStatus returned error\n");
        }

        return;
    }

    g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    g_ServiceStatus.dwWin32ExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 0;

    if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE) {
        LogWriter::log(L"AntivirusService: ServiceMain: SetServiceStatus returned error\n");
    }

    startServiceThreads();

    CloseHandle(g_ServiceStopEvent);

    g_ServiceStatus.dwControlsAccepted = 0;
    g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    g_ServiceStatus.dwWin32ExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 3;

    if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE) {
        LogWriter::log(L"AntivirusService: ServiceMain: SetServiceStatus returned error\n");
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
            startServiceThreads();
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
