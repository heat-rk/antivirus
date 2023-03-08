#pragma once

#include <Windows.h>

namespace Antivirus {
	class ServiceManager {
	private:
		const LPCWSTR SERVICE_NAME = L"AntivirusService";
		const LPCWSTR SERVICE_DISPLAY_NAME = L"Antivirus Service";
		char16_t* serviceBinaryPath;
	public:
		ServiceManager(char16_t* serviceBinaryPath)
			: serviceBinaryPath(serviceBinaryPath) 
		{}

		int installService();
		int uninstallService();
		int runService();
		int stopService();
	};
}
