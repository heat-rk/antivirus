#pragma once

#include <Windows.h>

namespace Antivirus {
	class LogWriter {
	private:
		static HANDLE m_pipe;

		static void openPipe();
	public:
		static void log(const wchar_t* format, ...);
		static void log(int8_t* bytes, int32_t size);
	};
}
