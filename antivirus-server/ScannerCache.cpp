#include "ScannerCache.h"
#include "AppDataProvider.h"
#include "ByteBuffer.h"
#include "LogWriter.h"
#include "ScannerConstants.h"

#include <Windows.h>
#include <stdio.h>
#include <filesystem>

#define SCANNER_DATA_FILE_NAME L"scanner-data"

using namespace Antivirus;

ScannerCache::ScannerCache() {
    appdataDirectory(&m_scannerDataFilePath);
    wcscat_s(m_scannerDataFilePath, MAX_PATH, SCANNER_DATA_FILE_NAME);
}

ScannerCache::~ScannerCache() {
    delete[] m_scannerDataFilePath;
}

void ScannerCache::save(
    std::vector<std::wstring> all,
    std::vector<int8_t> statuses,
    int8_t scannerStatus
) {
    std::fstream scannerDataFile;

    scannerDataFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        scannerDataFile.open(m_scannerDataFilePath, std::ios::out | std::ios::binary | std::ios::trunc);
    }
    catch (std::system_error& e) {
        LogWriter::log("%s\n", e.code().message().c_str());
        return;
    }

    ByteBuffer byteBuffer(0);

    byteBuffer.put(scannerStatus);

    for (int i = 0; i < all.size(); i++) {
        int32_t entrySize = (int32_t)(all[i].size() + sizeof(wchar_t));
        byteBuffer.put(statuses[i]);
        byteBuffer.put(entrySize);
        byteBuffer.put((wchar_t*)all[i].c_str(), entrySize);
    }

    char* bytes = new char[byteBuffer.size()];
    byteBuffer.getChars(bytes, byteBuffer.size());

    scannerDataFile.write(bytes, byteBuffer.size());

    delete[] bytes;
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