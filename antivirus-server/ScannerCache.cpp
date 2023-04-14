#include "ScannerCache.h"
#include "AppDataProvider.h"
#include "ByteBuffer.h"

#include <fstream>
#include <Windows.h>
#include <stdio.h>
#include <filesystem>

#define SCANNER_DATA_FILE_NAME L"scanner-data"

using namespace Antivirus;

void ScannerCache::save(std::vector<std::wstring> viruses) {
    /*wchar_t* appdataPath;
    appdataDirectory(&appdataPath);
    wcscat_s(appdataPath, MAX_PATH, SCANNER_DATA_FILE_NAME);

    fstream scannerDataFile;
    scannerDataFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        scannerDataFile.open(appdataPath, ios::out | ios::binary);
    } catch (std::system_error& e) {
        printf("%s\n", e.code().message().c_str());
    }

    if (!scannerDataFile.is_open()) {
        char message[] = "Scanner: Scanner cache file can't be opened\n";
        printf(message);
        return;
    }

    int32_t size = 0;

    for (int i = 0; i < viruses.size(); i++) {
        size += sizeof(int32_t) + viruses[i].size();
    }

    ByteBuffer byteBuffer(size);

    for (int i = 0; i < viruses.size(); i++) {
        byteBuffer.put((int32_t)viruses[i].size());
        byteBuffer.put((wchar_t*)viruses[i].c_str(), viruses[i].size());
    }

    char* bytes = new char[byteBuffer.size()];
    byteBuffer.getChars(bytes, byteBuffer.size());

    scannerDataFile.write(bytes, byteBuffer.size());

    CoTaskMemFree(appdataPath);
    scannerDataFile.close();
    delete bytes;*/
}

void ScannerCache::load(std::vector<std::wstring>* dest) {

}