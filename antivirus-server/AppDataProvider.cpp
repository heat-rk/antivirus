#include <Windows.h>
#include <ShlObj.h>
#include <iostream>

#include "AppDataProvider.h"
#include "LogWriter.h"

#define APP_DIRECTORY_NAME L"\\BVT2001 Antivirus\\"

static wchar_t* cachedPath;

int Antivirus::appdataDirectory(wchar_t** path) {
    if (cachedPath == NULL) {
        HRESULT result = SHGetKnownFolderPath(FOLDERID_ProgramData, 0, NULL, &cachedPath);

        if (result != S_OK) {
            LogWriter::log("Failed to get local appdata path\n");
            return 1;
        }

        wcscat_s(cachedPath, MAX_PATH, APP_DIRECTORY_NAME);
    }

    *path = new wchar_t[MAX_PATH];
    wcscpy_s(*path, MAX_PATH, cachedPath);

    return 0;
}