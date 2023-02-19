#pragma once

#include <Windows.h>

namespace Antivirus {
	class Scanner {
	private:
		HANDLE scannerThread;
	public:
		Scanner();
		void start();
	};
}