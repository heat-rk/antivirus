#pragma once

#include <Windows.h>

namespace Antivirus {
	class ServiceManager {
	private:
		const LPCWSTR m_SERVICE_NAME = L"AntivirusService";
		const LPCWSTR m_SERVICE_DISPLAY_NAME = L"Antivirus Service";
		wchar_t* m_serviceBinaryPath;
	public:
		ServiceManager(wchar_t* serviceBinaryPath)
			: m_serviceBinaryPath(serviceBinaryPath) 
		{}

		int installService();
		int uninstallService();
		int runService();
		int stopService();
		int loadBaseInput(wchar_t* path);
		int scan(wchar_t* path);
		int logs();
	};
}
