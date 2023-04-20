#include <Windows.h>
#include <iostream>
#include <tchar.h>
#include <Shlwapi.h>
#include <strsafe.h>
#include "Quarantine.h"

#include <shlwapi.h> // include Shlwapi library for PathCombine
using namespace Antivirus;




int main()
{
    Quarantine qrnt;
    // Quarantine::InfectedFile file(_T("C:\\Users\\ggser.LAPTOP-IDL1RS3U\\fake1.exe"));
    Quarantine::InfectedFile file(_T("C:\\Users\\ggser.LAPTOP-IDL1RS3U\\fake2.docx"));
    qrnt.MoveToQuarantine(file);
    // qrnt.DenyAccessToFile(file.quarantinePath);s
     //qrnt.AllowAccessToFile(file.filePath);
    qrnt.Restore(file);



    // Теперь можно использовать информацию о файле, хранящуюся в структуре file.

    //
//struct InfectedFile
//{
//    TCHAR filePath[MAX_PATH];
//    TCHAR quarantinePath[MAX_PATH];
//    bool isQuarantined;
//    InfectedFile(const TCHAR* path)
//    {
//        _tcscpy_s(filePath, MAX_PATH, path);
//        _tcscpy_s(quarantinePath, MAX_PATH, _T("C:\\quarantine"));
//        isQuarantined = false;
//    }
//};
//
//LPCTSTR QuarantineDirectory()
//{
//    TCHAR routeTopackage[MAX_PATH] = { 0 };
//    DWORD lenCharRoute = GetTempPath(MAX_PATH, routeTopackage);
//
//    if (lenCharRoute > MAX_PATH || (lenCharRoute == 0))
//    {
//        return NULL;
//    }
//
//    LPCTSTR lpSubDir = _T("\\quarantine");
//    _tcscat_s(routeTopackage, MAX_PATH, lpSubDir);
//
//    DWORD fileAttributes = GetFileAttributes(routeTopackage);
//    if (fileAttributes != INVALID_FILE_ATTRIBUTES && (fileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
//        return routeTopackage;
//    }
//    else {
//        if (CreateDirectory(routeTopackage, NULL))
//        {
//            return routeTopackage;
//        }
//        else
//        {
//            return NULL;
//        }
//    }
//}
//
//bool DenyAccessToFile(const TCHAR* fileName)
//{
//    TCHAR newFileName[MAX_PATH];
//
//    // Получаем имя файла без расширения
//
//    // Добавляем новое расширение ".deny"
//    StringCchCopy(newFileName, MAX_PATH, fileName);
//    // StringCchCat(newFileName, MAX_PATH, _T(".deny"));
//
//     // Переименовываем файл
//    if (!MoveFile(fileName, newFileName))
//    {
//        // Обработка ошибки
//        return false;
//    }
//
//    // Делаем файл скрытым и недоступным для записи
//    DWORD attributes = GetFileAttributes(newFileName);
//    if (attributes == INVALID_FILE_ATTRIBUTES)
//    {
//        // Обработка ошибки
//        return false;
//    }
//
//    attributes |= FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_NOT_CONTENT_INDEXED;
//    if (!SetFileAttributes(newFileName, attributes))
//    {
//        // Обработка ошибки
//        return false;
//    }
//
//    return true;
//}
//
//
//bool AllowAccessToFile(const TCHAR* fileName) {
//    DWORD attributes = GetFileAttributes(fileName);
//    if (attributes == INVALID_FILE_ATTRIBUTES) {
//        // Обработка ошибки
//        return false;
//    }
//    attributes &= ~(FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_NOT_CONTENT_INDEXED);
//    if (!SetFileAttributes(fileName, attributes | FILE_ATTRIBUTE_NORMAL)) {
//        // Обработка ошибки
//        return false;
//    }
//    return true;
//}
//
//
//
//
//
//void moveToQuarantine(InfectedFile& file)
//{
//    LPCTSTR quarantinePath = QuarantineDirectory();
//    if (quarantinePath != NULL)
//    {
//        LPCTSTR fileName = PathFindFileName(file.filePath);
//        TCHAR newPath[MAX_PATH];
//
//        PathCombine(newPath, quarantinePath, fileName);
//        _tprintf(_T("New path: %s\n"), newPath);
//
//        MoveFile(file.filePath, newPath);
//        file.isQuarantined = true;
//        _tcscpy_s(file.quarantinePath, MAX_PATH, newPath);
//        DenyAccessToFile(newPath);
//    }
//}
//
//
//
//bool Restore(InfectedFile file)
//{
//    if (file.isQuarantined)
//    {
//        std::cout << file.quarantinePath;
//        std::cout << file.isQuarantined;
//        AllowAccessToFile(file.quarantinePath);
//        TCHAR newFilePath[MAX_PATH];
//        StringCchCopy(newFilePath, MAX_PATH, file.filePath);
//        //StringCchCat(newFilePath, MAX_PATH, _T("\\"));
//        //StringCchCat(newFilePath, MAX_PATH, PathFindFileName(file.quarantinePath));
//
//        if (!MoveFile(file.quarantinePath, newFilePath))
//        {
//            // Обработка ошибки
//            return false;
//        }
//
//        if (!AllowAccessToFile(newFilePath))
//        {
//            // Обработка ошибки
//            return false;
//        }
//
//        file.isQuarantined = false;
//        return true;
//    }
//    else
//    {
//        // Файл не находится в карантине
//        return false;
//    }
//}
//
//
//


}






