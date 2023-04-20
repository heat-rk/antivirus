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