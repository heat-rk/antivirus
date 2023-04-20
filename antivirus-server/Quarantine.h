
#pragma once

#include <Windows.h>
#include <strsafe.h>
#include <tchar.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
namespace Antivirus {
	class Quarantine {
	public:
		struct InfectedFile
		{
			TCHAR filePath[MAX_PATH];
			TCHAR quarantinePath[MAX_PATH];
			bool isQuarantined;
			InfectedFile(const TCHAR* path)
			{
				_tcscpy_s(filePath, MAX_PATH, path);
				_tcscpy_s(quarantinePath, MAX_PATH, _T("C:\\quarantine"));
				isQuarantined = false;
			}
		};
	public:
		LPCTSTR QuarantineDirectory();
		bool Restore(InfectedFile file);
		void MoveToQuarantine(InfectedFile& file);
		bool AllowAccessToFile(const TCHAR* fileName);
		bool DenyAccessToFile(const TCHAR* fileName);

	};
}