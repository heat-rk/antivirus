#include "ScannerCache.h"
#include "AppDataProvider.h"
#include "ByteBuffer.h"
#include "LogWriter.h"

#include <fstream>
#include <Windows.h>
#include <stdio.h>
#include <filesystem>

#define SCANNER_DATA_FILE_NAME L"scanner-data"

using namespace Antivirus;

void ScannerCache::save(std::vector<std::wstring> viruses) {
    wchar_t* appdataPath;
    appdataDirectory(&appdataPath);
    wcscat_s(appdataPath, MAX_PATH, SCANNER_DATA_FILE_NAME);

    std::fstream scannerDataFile;
    scannerDataFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        scannerDataFile.open(appdataPath, std::ios::out | std::ios::binary);
    } catch (std::system_error& e) {
        LogWriter::log("%s\n", e.code().message().c_str());
    }

    if (!scannerDataFile.is_open()) {
        char message[] = "Scanner:Save: Scanner cache file can't be opened\n";
        LogWriter::log(message);
        return;
    }

    int32_t size = 0;

    for (int i = 0; i < viruses.size(); i++) {
        size += sizeof(int32_t) + viruses[i].size() + 1;
    }

    ByteBuffer byteBuffer(size);

    for (int i = 0; i < viruses.size(); i++) {
        byteBuffer.put((int32_t)viruses[i].size() + 1);
        byteBuffer.put((wchar_t*)viruses[i].c_str(), viruses[i].size() + 1);
    }

    char* bytes = new char[byteBuffer.size()];
    byteBuffer.getChars(bytes, byteBuffer.size());

    scannerDataFile.write(bytes, byteBuffer.size());

    CoTaskMemFree(appdataPath);
    scannerDataFile.close();
    delete bytes;
}

void ScannerCache::load(std::vector<std::wstring>* dest) {
    wchar_t* appdataPath;
    appdataDirectory(&appdataPath);
    wcscat_s(appdataPath, MAX_PATH, SCANNER_DATA_FILE_NAME);

    std::fstream scannerDataFile;
    scannerDataFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        scannerDataFile.open(appdataPath, std::ios::in | std::ios::binary);
    }
    catch (std::system_error& e) {
        LogWriter::log("%s\n", e.code().message().c_str());
    }

    if (!scannerDataFile.is_open()) {
        char message[] = "Scanner:Load: Scanner cache file can't be opened\n";
        LogWriter::log(message);
        return;
    }

    ByteBuffer byteBuffer;
    char pathLengthBytesBuffer[sizeof(int32_t)];
    int32_t pathLength;
    char* pathBytesBuffer;
    wchar_t* pathBuffer;

    while (scannerDataFile.peek() != EOF) {
        scannerDataFile.read(pathLengthBytesBuffer, sizeof(pathLengthBytesBuffer));
        byteBuffer.put(pathLengthBytesBuffer, sizeof(pathLengthBytesBuffer));
        pathLength = byteBuffer.getInt32();
        pathBytesBuffer = new char[pathLength];
        scannerDataFile.read(pathBytesBuffer, pathLength);
        byteBuffer.put(pathBytesBuffer, pathLength);
        pathBuffer = new wchar_t[pathLength];
        byteBuffer.getWChars(pathBuffer, pathLength);
        dest->push_back(std::wstring(pathBuffer));
        delete pathBytesBuffer;
        delete pathBuffer;
    }

    CoTaskMemFree(appdataPath);
    scannerDataFile.close();
}