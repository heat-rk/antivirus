#include <Windows.h>
#include <stdio.h>

#include "ServiceManager.h"

using namespace Antivirus;
using namespace std;

int ServiceManager::installService() {
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

	if (!hSCManager) {
		printf("Can't open Service Control Manager: %d\n", GetLastError());
		return -1;
	}

	LPWSTR servicePath = reinterpret_cast<LPWSTR>(const_cast<char16_t*>(serviceBinaryPath));

	SC_HANDLE hService = CreateService(
		hSCManager,
		SERVICE_NAME,
		SERVICE_DISPLAY_NAME,
		SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_AUTO_START,
		SERVICE_ERROR_NORMAL,
		servicePath,
		NULL, NULL, NULL, NULL, NULL
	);

	if (!hService) {
		printf("Can't create service: %d\n", GetLastError());
		CloseServiceHandle(hSCManager);
		return -1;
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);

	printf("Success install service!\n");

    return 0;
}

int ServiceManager::uninstallService() {
    if (stopService() != 0) {
        return -1;
    }

	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (!hSCManager) {
		printf("Can't open Service Control Manager: %d\n", GetLastError());
		return -1;
	}

	SC_HANDLE hService = OpenServiceW(
		hSCManager,
		SERVICE_NAME,
		SERVICE_STOP | DELETE
	);

	if (!hService) {
		printf("Can't remove service: %d\n", GetLastError());
		CloseServiceHandle(hSCManager);
		return -1;
	}

	DeleteService(hService);
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);

	printf("Success remove service!\n");

    return 0;
}

int ServiceManager::runService() {
    SERVICE_STATUS_PROCESS ssStatus;
    DWORD dwOldCheckPoint;
    DWORD dwStartTickCount;
    DWORD dwWaitTime;
    DWORD dwBytesNeeded;

    // Get a handle to the SCM database. 

    SC_HANDLE schSCManager = OpenSCManager(
        NULL,                    // local computer
        NULL,                    // servicesActive database 
        SC_MANAGER_CONNECT
    );  

    if (NULL == schSCManager) {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return -1;
    }

    // Get a handle to the service.

    SC_HANDLE schService = OpenService(
        schSCManager,         // SCM database 
        SERVICE_NAME,            // name of service 
        SERVICE_START | SERVICE_QUERY_STATUS
    );

    if (schService == NULL) {
        printf("OpenService failed (%d)\n", GetLastError());
        CloseServiceHandle(schSCManager);
        return -1;
    }

    // Check the status in case the service is not stopped. 

    if (!QueryServiceStatusEx(
        schService,                     // handle to service 
        SC_STATUS_PROCESS_INFO,         // information level
        (LPBYTE)&ssStatus,             // address of structure
        sizeof(SERVICE_STATUS_PROCESS), // size of structure
        &dwBytesNeeded)
    ) {
        printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return -1;
    }

    // Check if the service is already running. It would be possible 
    // to stop the service here, but for simplicity this example just returns. 

    if (ssStatus.dwCurrentState != SERVICE_STOPPED && ssStatus.dwCurrentState != SERVICE_STOP_PENDING) {
        printf("Cannot start the service because it is already running\n");
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return 0;
    }

    // Save the tick count and initial checkpoint.

    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = ssStatus.dwCheckPoint;

    // Wait for the service to stop before attempting to start it.

    while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING) {
        // Do not wait longer than the wait hint. A good interval is 
        // one-tenth of the wait hint but not less than 1 second  
        // and not more than 10 seconds. 

        dwWaitTime = ssStatus.dwWaitHint / 10;

        if (dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        Sleep(dwWaitTime);

        // Check the status until the service is no longer stop pending. 

        if (!QueryServiceStatusEx(
            schService,                     // handle to service 
            SC_STATUS_PROCESS_INFO,         // information level
            (LPBYTE)&ssStatus,             // address of structure
            sizeof(SERVICE_STATUS_PROCESS), // size of structure
            &dwBytesNeeded)
        ) {
            printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return -1;
        }

        if (ssStatus.dwCheckPoint > dwOldCheckPoint) {
            // Continue to wait and check.
            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = ssStatus.dwCheckPoint;
        } else {
            if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint) {
                printf("Timeout waiting for service to stop\n");
                CloseServiceHandle(schService);
                CloseServiceHandle(schSCManager);
                return -1;
            }
        }
    }

    // Attempt to start the service.
    if (!StartService(
        schService,  // handle to service 
        0,           // number of arguments 
        NULL)
    ) {
        printf("StartService failed (%d)\n", GetLastError());
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return -1;
    }
    else {
        printf("Service start pending...\n");
    }

    // Check the status until the service is no longer start pending. 

    if (!QueryServiceStatusEx(
        schService,                     // handle to service 
        SC_STATUS_PROCESS_INFO,         // info level
        (LPBYTE)&ssStatus,             // address of structure
        sizeof(SERVICE_STATUS_PROCESS), // size of structure
        &dwBytesNeeded)
    ) {
        printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return -1;
    }

    // Save the tick count and initial checkpoint.

    dwStartTickCount = GetTickCount();
    dwOldCheckPoint = ssStatus.dwCheckPoint;

    while (ssStatus.dwCurrentState == SERVICE_START_PENDING) {
        // Do not wait longer than the wait hint. A good interval is 
        // one-tenth the wait hint, but no less than 1 second and no 
        // more than 10 seconds. 

        dwWaitTime = ssStatus.dwWaitHint / 10;

        if (dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        Sleep(dwWaitTime);

        // Check the status again. 

        if (!QueryServiceStatusEx(
            schService,             // handle to service 
            SC_STATUS_PROCESS_INFO, // info level
            (LPBYTE)&ssStatus,             // address of structure
            sizeof(SERVICE_STATUS_PROCESS), // size of structure
            &dwBytesNeeded)
        ) {
            printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
            break;
        }

        if (ssStatus.dwCheckPoint > dwOldCheckPoint) {
            // Continue to wait and check.
            dwStartTickCount = GetTickCount();
            dwOldCheckPoint = ssStatus.dwCheckPoint;
        } else {
            if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint) {
                // No progress made within the wait hint.
                break;
            }
        }
    }

    // Determine whether the service is running.

    if (ssStatus.dwCurrentState == SERVICE_RUNNING) {
        printf("Service started successfully.\n");
    } else {
        printf("Service not started. \n");
        printf("  Current State: %d\n", ssStatus.dwCurrentState);
        printf("  Exit Code: %d\n", ssStatus.dwWin32ExitCode);
        printf("  Check Point: %d\n", ssStatus.dwCheckPoint);
        printf("  Wait Hint: %d\n", ssStatus.dwWaitHint);
    }

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);

    return 0;
}

int ServiceManager::stopService() {
    SERVICE_STATUS_PROCESS ssp;
    DWORD dwStartTime = GetTickCount();
    DWORD dwBytesNeeded;
    DWORD dwTimeout = 30000; // 30-second time-out
    DWORD dwWaitTime;

    // Get a handle to the SCM database. 

    SC_HANDLE schSCManager = OpenSCManager(
        NULL,                    // local computer
        NULL,                    // ServicesActive database 
        SC_MANAGER_CONNECT
    );

    if (NULL == schSCManager) {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return -1;
    }

    // Get a handle to the service.

    SC_HANDLE schService = OpenService(
        schSCManager,         // SCM database 
        SERVICE_NAME,            // name of service 
        SERVICE_STOP |
        SERVICE_QUERY_STATUS
    );

    if (schService == NULL) {
        printf("OpenService failed (%d)\n", GetLastError());
        CloseServiceHandle(schSCManager);
        return -1;
    }

    // Make sure the service is not already stopped.

    if (!QueryServiceStatusEx(
        schService,
        SC_STATUS_PROCESS_INFO,
        (LPBYTE)&ssp,
        sizeof(SERVICE_STATUS_PROCESS),
        &dwBytesNeeded)
    ) {
        printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return -1;
    }

    if (ssp.dwCurrentState == SERVICE_STOPPED) {
        printf("Service is already stopped.\n");
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return -1;
    }

    // If a stop is pending, wait for it.

    while (ssp.dwCurrentState == SERVICE_STOP_PENDING) {
        printf("Service stop pending...\n");

        // Do not wait longer than the wait hint. A good interval is 
        // one-tenth of the wait hint but not less than 1 second  
        // and not more than 10 seconds. 

        dwWaitTime = ssp.dwWaitHint / 10;

        if (dwWaitTime < 1000)
            dwWaitTime = 1000;
        else if (dwWaitTime > 10000)
            dwWaitTime = 10000;

        Sleep(dwWaitTime);

        if (!QueryServiceStatusEx(
            schService,
            SC_STATUS_PROCESS_INFO,
            (LPBYTE)&ssp,
            sizeof(SERVICE_STATUS_PROCESS),
            &dwBytesNeeded)
        ) {
            printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return -1;
        }

        if (ssp.dwCurrentState == SERVICE_STOPPED) {
            printf("Service stopped successfully.\n");
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return -1;
        }

        if (GetTickCount() - dwStartTime > dwTimeout) {
            printf("Service stop timed out.\n");
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return -1;
        }
    }

    // Send a stop code to the service.

    if (!ControlService(
        schService,
        SERVICE_CONTROL_STOP,
        (LPSERVICE_STATUS)&ssp)
    ) {
        printf("ControlService failed (%d)\n", GetLastError());
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return -1;
    }

    // Wait for the service to stop.

    while (ssp.dwCurrentState != SERVICE_STOPPED) {
        Sleep(ssp.dwWaitHint);

        if (!QueryServiceStatusEx(
            schService,
            SC_STATUS_PROCESS_INFO,
            (LPBYTE)&ssp,
            sizeof(SERVICE_STATUS_PROCESS),
            &dwBytesNeeded)
        ) {
            printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return -1;
        }

        if (ssp.dwCurrentState == SERVICE_STOPPED)
            break;

        if (GetTickCount() - dwStartTime > dwTimeout) {
            printf("Wait timed out\n");
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return -1;
        }
    }

    printf("Service stopped successfully\n");

    return 0;
}