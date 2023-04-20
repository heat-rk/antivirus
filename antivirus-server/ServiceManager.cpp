#include <Windows.h>
#include <stdio.h>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>

#include "SignatureBaseFile.h"
#include "Sha256.h"
#include "ServiceManager.h"
#include "Utils.h"
#include "ByteBuffer.h"
#include "Scanner.h"
#include "AppDataProvider.h"
#include "VirusRecord.h"
#include "LogReader.h"
#include "LogWriter.h"

using namespace Antivirus;

#define BASE_FILE_NAME L"antivirus-base"

int ServiceManager::installService() {
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);

	if (!hSCManager) {
        LogWriter::log("ServiceManager: Can't open Service Control Manager: %d\n", GetLastError());
		return -1;
	}

	SC_HANDLE hService = CreateService(
		hSCManager,
		m_SERVICE_NAME,
		m_SERVICE_DISPLAY_NAME,
		SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_AUTO_START,
		SERVICE_ERROR_NORMAL,
        m_serviceBinaryPath,
		NULL, NULL, NULL, NULL, NULL
	);

	if (!hService) {
        LogWriter::log("ServiceManager: Can't create service: %d\n", GetLastError());
		CloseServiceHandle(hSCManager);
		return -1;
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);

    LogWriter::log("ServiceManager: Success install service!\n");

    return 0;
}

int ServiceManager::uninstallService() {
    if (stopService() != 0) {
        return -1;
    }

	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (!hSCManager) {
        LogWriter::log("ServiceManager: Can't open Service Control Manager: %d\n", GetLastError());
		return -1;
	}

	SC_HANDLE hService = OpenServiceW(
		hSCManager,
		m_SERVICE_NAME,
		SERVICE_STOP | DELETE
	);

	if (!hService) {
        LogWriter::log("ServiceManager: Can't remove service: %d\n", GetLastError());
		CloseServiceHandle(hSCManager);
		return -1;
	}

	DeleteService(hService);
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);

    LogWriter::log("ServiceManager: Success remove service!\n");

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
        LogWriter::log("ServiceManager: OpenSCManager failed (%d)\n", GetLastError());
        return -1;
    }

    // Get a handle to the service.

    SC_HANDLE schService = OpenService(
        schSCManager,         // SCM database 
        m_SERVICE_NAME,            // name of service 
        SERVICE_START | SERVICE_QUERY_STATUS
    );

    if (schService == NULL) {
        LogWriter::log("ServiceManager: OpenService failed (%d)\n", GetLastError());
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
        LogWriter::log("ServiceManager: QueryServiceStatusEx failed (%d)\n", GetLastError());
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return -1;
    }

    // Check if the service is already running. It would be possible 
    // to stop the service here, but for simplicity this example just returns. 

    if (ssStatus.dwCurrentState != SERVICE_STOPPED && ssStatus.dwCurrentState != SERVICE_STOP_PENDING) {
        LogWriter::log("ServiceManager: Cannot start the service because it is already running\n");
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
            LogWriter::log("ServiceManager: QueryServiceStatusEx failed (%d)\n", GetLastError());
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
                LogWriter::log("ServiceManager: Timeout waiting for service to stop\n");
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
        LogWriter::log("ServiceManager: StartService failed (%d)\n", GetLastError());
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return -1;
    }
    else {
        LogWriter::log("ServiceManager: Service start pending...\n");
    }

    // Check the status until the service is no longer start pending. 

    if (!QueryServiceStatusEx(
        schService,                     // handle to service 
        SC_STATUS_PROCESS_INFO,         // info level
        (LPBYTE)&ssStatus,             // address of structure
        sizeof(SERVICE_STATUS_PROCESS), // size of structure
        &dwBytesNeeded)
    ) {
        LogWriter::log("ServiceManager: QueryServiceStatusEx failed (%d)\n", GetLastError());
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
            LogWriter::log("ServiceManager: QueryServiceStatusEx failed (%d)\n", GetLastError());
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
        LogWriter::log("ServiceManager: Service started successfully.\n");
    } else {
        LogWriter::log("ServiceManager: Service not started. \n");
        LogWriter::log("  Current State: %d\n", ssStatus.dwCurrentState);
        LogWriter::log("  Exit Code: %d\n", ssStatus.dwWin32ExitCode);
        LogWriter::log("  Check Point: %d\n", ssStatus.dwCheckPoint);
        LogWriter::log("  Wait Hint: %d\n", ssStatus.dwWaitHint);
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
        LogWriter::log("ServiceManager: OpenSCManager failed (%d)\n", GetLastError());
        return -1;
    }

    // Get a handle to the service.

    SC_HANDLE schService = OpenService(
        schSCManager,         // SCM database 
        m_SERVICE_NAME,            // name of service 
        SERVICE_STOP |
        SERVICE_QUERY_STATUS
    );

    if (schService == NULL) {
        LogWriter::log("ServiceManager: OpenService failed (%d)\n", GetLastError());
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
        LogWriter::log("ServiceManager: QueryServiceStatusEx failed (%d)\n", GetLastError());
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return -1;
    }

    if (ssp.dwCurrentState == SERVICE_STOPPED) {
        LogWriter::log("ServiceManager: Service is already stopped.\n");
        CloseServiceHandle(schService);
        CloseServiceHandle(schSCManager);
        return 0;
    }

    // If a stop is pending, wait for it.

    while (ssp.dwCurrentState == SERVICE_STOP_PENDING) {
        LogWriter::log("ServiceManager: Service stop pending...\n");

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
            LogWriter::log("ServiceManager: QueryServiceStatusEx failed (%d)\n", GetLastError());
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return -1;
        }

        if (ssp.dwCurrentState == SERVICE_STOPPED) {
            LogWriter::log("ServiceManager: Service stopped successfully.\n");
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return -1;
        }

        if (GetTickCount() - dwStartTime > dwTimeout) {
            LogWriter::log("ServiceManager: Service stop timed out.\n");
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
        LogWriter::log("ServiceManager: ControlService failed (%d)\n", GetLastError());
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
            LogWriter::log("ServiceManager: QueryServiceStatusEx failed (%d)\n", GetLastError());
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return -1;
        }

        if (ssp.dwCurrentState == SERVICE_STOPPED)
            break;

        if (GetTickCount() - dwStartTime > dwTimeout) {
            LogWriter::log("ServiceManager: Wait timed out\n");
            CloseServiceHandle(schService);
            CloseServiceHandle(schSCManager);
            return -1;
        }
    }

    LogWriter::log("ServiceManager: Service stopped successfully\n");

    return 0;
}

int ServiceManager::loadBaseInput(wchar_t* path) {
    SignatureBaseFileWriter baseWriter;

    wchar_t* appdataPath = NULL;
    appdataDirectory(&appdataPath);
    
    if (!CreateDirectory(appdataPath, NULL) &&
        ERROR_ALREADY_EXISTS != GetLastError()) {
        LogWriter::log("ServiceManager: Error creating app data directory (GLE = %d)\n", GetLastError());
        return 1;
    }

    wcscat_s(appdataPath, MAX_PATH, BASE_FILE_NAME);
    baseWriter.open(appdataPath, true);

    std::ifstream file;

    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        file.open(path);
    }
    catch (std::system_error& e) {
        LogWriter::log("ServiceManager: %s\n", e.code().message().c_str());
        return 1;
    }
    
    VirusSignature signature;
    VirusRecord record;
    Sha256 sha256;
    uint8_t* hash;
    std::string line;
    int8_t* signatureBytes;
    uint32_t i;
    ByteBuffer firstBytesBuffer(sizeof(signature.first));

    while (!file.eof()) {
        getline(file, line);
        signature.offset = toInt32(line);

        getline(file, line);
        signature.length = toInt32(line);

        getline(file, line);
        signatureBytes = new int8_t[signature.length];
        std::stringstream signatureBytesStream(line);

        i = 0;

        while (getline(signatureBytesStream, line, ' ')) {
            signatureBytes[i++] = toInt8(line);
        }

        sha256.update(signatureBytes, signature.length);
        hash = sha256.digest();
        std::copy(hash, hash + HASH_SIZE, signature.hash);

        firstBytesBuffer.put(signatureBytes, firstBytesBuffer.size());
        signature.first = firstBytesBuffer.getInt64();

        getline(file, line);
        record.name = line;

        record.signature = signature;

        delete signatureBytes;

        getline(file, line);
        record.type = toInt8(line);

        baseWriter.addRecord(record);

        getline(file, line);

        if (line.compare(";") != 0) {
            LogWriter::log(L"ServiceManager: Viruses base file not ending with \";\"!\n");
            break;
        }
    }

    baseWriter.close();
    delete[] appdataPath;

    return 0;
}

int ServiceManager::scan(wchar_t* path) {
    Scanner scanner;
    SignatureBaseFileReader baseReader;

    wchar_t* appdataPath = NULL;
    appdataDirectory(&appdataPath);
    wcscat_s(appdataPath, MAX_PATH, BASE_FILE_NAME);

    baseReader.open(appdataPath);

    delete[] appdataPath;

    for (int i = 0; i < baseReader.getRecordsCount(); i++) {
        VirusRecord record;
        baseReader.readRecord(&record);
        scanner.addRecord(record);
    }

    baseReader.close();

    scanner.start(path);

    return 0;
}

int ServiceManager::logs() {
    LogReader::connect();
    return 0;
}