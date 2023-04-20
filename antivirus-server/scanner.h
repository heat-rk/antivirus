#pragma once

#include "Message.h"
#include "VirusRecord.h"
#include "Sha256.h"
#include "ChannelClient.h"
#include "ScannerCache.h"
#include "ByteBuffer.h"

#include <Windows.h>
#include <fstream>
#include <functional>
#include <map>
#include <vector>
#include <queue>
#include <mutex>

namespace Antivirus {
	class Scanner {
	public:
		struct ChannelThreadParams {
			ChannelClient* channel;
			Scanner* scanner;
		};
	private:
		HANDLE m_scannerChannelThread;
		HANDLE m_stopEvent;
		HANDLE m_resumeEvent;
		ChannelThreadParams* m_channelThreadParams;
		std::map<int64_t, std::vector<VirusRecord>> m_records;
		Sha256 m_sha256;
		ChannelClient m_channel;
		ScannerCache m_cache;
		std::vector<std::wstring> m_entries;
		std::vector<std::int8_t> m_statuses;
		int8_t m_scanStatus;
		ByteBuffer m_firstBytesBuffer;

		void findEntries(std::wstring path);
		void updateStatus(int8_t status, bool force = false);
		bool scan(int8_t* bytes, uint64_t length, uint64_t offset);

	public:
		Scanner();
		~Scanner();

		void addRecord(VirusRecord record);

		void start(wchar_t* path);
		void pause();
		void resume();
		void stop();
	};
}