#include <Windows.h>
#include <ShlObj.h>
#include <iostream>

#include "AppDataProvider.h"
#include "LogWriter.h"

#define APP_LOCAL_DIRECTORY L"\\BVT2001 Antivirus\\"

int Antivirus::appdataDirectory(wchar_t** path) {
    HRESULT result = SHGetKnownFolderPath(FOLDERID_ProgramData, 0, NULL, path);

    if (result != S_OK) {
        LogWriter::log("Failed to get local appdata path\n");
        return 1;
    }

    wcscat_s(*path, MAX_PATH, APP_LOCAL_DIRECTORY);

    return 0;
}