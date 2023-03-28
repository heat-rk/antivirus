#pragma once

#include <Windows.h>

namespace Antivirus {
	class Scanner {
	private:
		bool m_isScanning;
		HANDLE m_scannerThread;
	public:
		Scanner();

		void start(char* path);
		void pause();
		void stop();

		bool isScanning();
	};
}