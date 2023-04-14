#pragma once

#include "Message.h"
#include "VirusRecord.h"
#include "Sha256.h"
#include "Channel.h"
#include "ScannerCache.h"

#include <Windows.h>
#include <fstream>
#include <functional>
#include <map>
#include <vector>

using namespace std;

namespace Antivirus {
	class Scanner {
	public:
		struct ThreadParams {
			Channel* channel;
			Scanner* scanner;
		};

	private:
		bool m_isActive;
		bool m_isScanning;
		HANDLE m_scannerThread;
		ThreadParams* m_threadParams;
		map<int64_t, vector<VirusRecord>> m_records;
		Sha256 m_sha256;
		Channel m_channel;
		ScannerCache m_cache;

		void findEntries(wstring path, vector<wstring>* entries);

		bool scan(ifstream* file);
		bool scan(int8_t* bytes, uint64_t length);
		bool scan(int8_t* bytes, uint64_t length, uint64_t offset);

	public:
		Scanner();
		~Scanner();

		void addRecord(VirusRecord record);

		void start(wchar_t* path);
		void pause();
		void resume();
		void stop();

		bool isScanning();
	};
}