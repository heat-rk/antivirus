#pragma once

#include <Windows.h>
#include <cstdint>

namespace Antivirus {
	class LogWriter {
	private:
		static HANDLE m_pipe;

		static bool openPipe();
	public:
		static void log(const wchar_t* format, ...);
		static void log(const char* format, ...);
		static void log(int8_t* bytes, int32_t size);
	};
}
