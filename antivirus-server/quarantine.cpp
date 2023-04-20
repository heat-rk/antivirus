#include "Quarantine.h"
using namespace Antivirus;
LPCTSTR Quarantine::QuarantineDirectory()
{

    TCHAR routeTopackage[MAX_PATH] = { 0 };
    DWORD lenCharRoute = GetTempPath(MAX_PATH, routeTopackage);

    if (lenCharRoute > MAX_PATH || (lenCharRoute == 0))
    {
        return NULL;
    }

    LPCTSTR lpSubDir = _T("\\quarantine");
    _tcscat_s(routeTopackage, MAX_PATH, lpSubDir);

    DWORD fileAttributes = GetFileAttributes(routeTopackage);
    if (fileAttributes != INVALID_FILE_ATTRIBUTES && (fileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
        return routeTopackage;
    }
    else {
        if (CreateDirectory(routeTopackage, NULL))
        {
            return routeTopackage;
        }
        else
        {
            return NULL;
        }
    }
}

bool Quarantine::Restore(InfectedFile file)
{
    if (file.isQuarantined)
    {
        AllowAccessToFile(file.quarantinePath);
        TCHAR newFilePath[MAX_PATH];
        StringCchCopy(newFilePath, MAX_PATH, file.filePath);
        //StringCchCat(newFilePath, MAX_PATH, _T("\\"));
        //StringCchCat(newFilePath, MAX_PATH, PathFindFileName(file.quarantinePath));

        if (!MoveFile(file.quarantinePath, newFilePath))
        {
            // ��������� ������
            return false;
        }

        if (!AllowAccessToFile(newFilePath))
        {
            // ��������� ������
            return false;
        }

        file.isQuarantined = false;
        return true;
    }
    else
    {
        // ���� �� ��������� � ���������
        return false;
    }
}

void Antivirus::Quarantine::MoveToQuarantine(InfectedFile& file)
{
    LPCTSTR quarantinePath = QuarantineDirectory();
    if (quarantinePath != NULL)
    {
        LPCTSTR fileName = PathFindFileName(file.filePath);
        TCHAR newPath[MAX_PATH];

        PathCombine(newPath, quarantinePath, fileName);
        _tprintf(_T("New path: %s\n"), newPath);

        MoveFile(file.filePath, newPath);
        file.isQuarantined = true;
        _tcscpy_s(file.quarantinePath, MAX_PATH, newPath);
        DenyAccessToFile(newPath);
    }
}

bool Antivirus::Quarantine::AllowAccessToFile(const TCHAR* fileName)
{
    DWORD attributes = GetFileAttributes(fileName);
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        // ��������� ������
        return false;
    }
    attributes &= ~(FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_NOT_CONTENT_INDEXED);
    if (!SetFileAttributes(fileName, attributes | FILE_ATTRIBUTE_NORMAL)) {
        // ��������� ������
        return false;
    }
    return true;
}

bool Antivirus::Quarantine::DenyAccessToFile(const TCHAR* fileName)
{
    TCHAR newFileName[MAX_PATH];

    // �������� ��� ����� ��� ����������

    // ��������� ����� ���������� ".deny"
    StringCchCopy(newFileName, MAX_PATH, fileName);
    // StringCchCat(newFileName, MAX_PATH, _T(".deny"));

     // ��������������� ����
    if (!MoveFile(fileName, newFileName))
    {
        // ��������� ������
        return false;
    }

    // ������ ���� ������� � ����������� ��� ������
    DWORD attributes = GetFileAttributes(newFileName);
    if (attributes == INVALID_FILE_ATTRIBUTES)
    {
        // ��������� ������
        return false;
    }

    attributes |= FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_NOT_CONTENT_INDEXED;
    if (!SetFileAttributes(newFileName, attributes))
    {
        // ��������� ������
        return false;
    }

    return true;
}
