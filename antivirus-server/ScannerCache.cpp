#include "ScannerCache.h"
#include "AppDataProvider.h"
#include "ByteBuffer.h"
#include "LogWriter.h"
#include "ScannerConstants.h"
#include "Utils.h"

#include <Windows.h>
#include <stdio.h>
#include <filesystem>
#include <mutex>

#define SCANNER_DATA_FILE_NAME L"scanner-data"
#define SAVE_DEBOUNCE_MILLIS 500

using namespace Antivirus;

DWORD WINAPI savingThreadHandler(LPVOID lpvParam) {
    ScannerCache::SavingThreadParams* params =
        reinterpret_cast<ScannerCache::SavingThreadParams*>(lpvParam);

    int64_t currentMillis;
    ScannerCache::DelayedSavingItem request;

    while (*params->saverEnabled || !params->saveRequests->empty()) {
        currentMillis = timeSinceEpochMillis();

        if (
            currentMillis - *params->lastSaveRequestMillis > SAVE_DEBOUNCE_MILLIS &&
            !params->saveRequests->empty()
        ) {
            *params->lastSaveRequestMillis = currentMillis;
            request = params->saveRequests->back();
            params->saveRequests->clear();

            std::fstream scannerDataFile;

            scannerDataFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

            try {
                scannerDataFile.open(
                    params->scannerDataFilePath,
                    std::ios::out | std::ios::binary | std::ios::trunc
                );
            }
            catch (std::system_error& e) {
                LogWriter::log("%s\n", e.code().message().c_str());
                continue;
            }

            ByteBuffer byteBuffer(0);

            byteBuffer.put(request.scannerStatus);

            for (int i = 0; i < request.all.size(); i++) {
                int32_t entrySize = 
                    (int32_t)(request.all[i].size() + sizeof(wchar_t));

                byteBuffer.put(request.statuses[i]);
                byteBuffer.put(entrySize);
                byteBuffer.put((wchar_t*)request.all[i].c_str(), entrySize);
            }

            char* bytes = new char[byteBuffer.size()];
            byteBuffer.getChars(bytes, byteBuffer.size());

            scannerDataFile.write(bytes, byteBuffer.size());

            delete[] bytes;
        }
    }

    return ERROR_SUCCESS;
}

ScannerCache::ScannerCache(bool saverEnabled) {
    m_saverEnabled = saverEnabled;

    appdataDirectory(&m_scannerDataFilePath);
    wcscat_s(m_scannerDataFilePath, MAX_PATH, SCANNER_DATA_FILE_NAME);

    m_savingThreadParams = new SavingThreadParams;
    m_savingThreadParams->saveRequests = &m_saveRequests;
    m_savingThreadParams->lastSaveRequestMillis = &m_lastSaveRequestMillis;
    m_savingThreadParams->scannerDataFilePath = m_scannerDataFilePath;
    m_savingThreadParams->saverEnabled = &m_saverEnabled;

    m_savingThread = CreateThread(
        NULL,
        0,
        savingThreadHandler,
        m_savingThreadParams,
        0,
        NULL
    );

    if (m_savingThread == NULL) {
        LogWriter::log("ScannerCache: Saving thread creation failed, GLE=%d.\n", GetLastError());
        return;
    }
}

ScannerCache::~ScannerCache() {
    m_saverEnabled = false;
    WaitForSingleObject(m_savingThread, INFINITE);
    delete m_savingThreadParams;
    CloseHandle(m_savingThread);
    delete[] m_scannerDataFilePath;
}

void ScannerCache::save(
    std::vector<std::wstring> all,
    std::vector<int8_t> statuses,
    int8_t scannerStatus
) {
    m_saveRequests.push_back(DelayedSavingItem{ all, statuses, scannerStatus });
    m_lastSaveRequestMillis = timeSinceEpochMillis();
}

bool ScannerCache::load(
    std::vector<std::wstring>* all,
    std::vector<int8_t>* statuses,
    int8_t* scannerStatus
) {
    std::ifstream scannerDataFile;

    scannerDataFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        scannerDataFile.open(m_scannerDataFilePath, std::ios::in | std::ios::binary);
    }
    catch (std::system_error& e) {
        LogWriter::log("%s\n", e.code().message().c_str());
        return false;
    }

    scannerDataFile.seekg(0, scannerDataFile.end);
    uint64_t length = scannerDataFile.tellg();
    scannerDataFile.seekg(0, scannerDataFile.beg);

    ByteBuffer byteBuffer(length);

    char* bytes = new char[length];
    scannerDataFile.read(bytes, length);
    scannerDataFile.close();

    byteBuffer.put(bytes, length);

    *scannerStatus = byteBuffer.getInt8();

    int32_t pathLength;
    wchar_t* path;

    while (byteBuffer.bytesRemaining() > 0) {
        statuses->push_back(byteBuffer.getInt8());
        pathLength = byteBuffer.getInt32();
        path = new wchar_t[pathLength];
        byteBuffer.getWChars(path, pathLength);
        all->push_back(std::wstring(path));
        delete[] path;
    }

    delete[] bytes;
    return true;
}

void ScannerCache::validate() {
    std::vector<std::wstring> all;
    std::vector<int8_t> statuses;
    int8_t scannerStatus;

    if (load(&all, &statuses, &scannerStatus)) {
        save(all, statuses, SCANNED);
        LogWriter::log(L"ScannerCache:Validate: Scanner status updated to SCANNED\n");
    } else {
        if (DeleteFile(m_scannerDataFilePath)) {
            LogWriter::log(L"ScannerCache:Validate: Scanner cached data deleted\n");
        }
        else {
            LogWriter::log(L"ScannerCache:Validate: Scanner cached data deleting failed!\n");
        }
    }
}