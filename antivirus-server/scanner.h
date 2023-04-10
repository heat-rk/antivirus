#pragma once

#include "Message.h"
#include "VirusRecord.h"
#include "Sha256.h"

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
			function<bool()> isScanning;
			vector<function<void()>>* queue;
		};

	private:
		bool m_isScanning;
		HANDLE m_scannerThread;
		vector<function<void()>> m_scanQueue;
		function<void(Message)> m_outgoingMessagesHandler;
		map<int64_t, vector<VirusRecord>> m_records;
		Sha256 m_sha256;
		ThreadParams* m_threadParams;

		void findEntries(wstring path, vector<wstring>* entries);
		void addToScanQueue(function<void()> task);

	public:
		Scanner();
		~Scanner();

		void addRecord(VirusRecord record);

		void handleIncomingMessage(Message message);
		void setOutgoingMessagesHandler(function<void(Message)> onMessage);

		void start(wchar_t* path);
		void pause();
		void resume();
		void stop();

		void waitForScannerThread();

		bool scan(ifstream* file);
		bool scan(int8_t* bytes, uint64_t length);
		bool scan(int8_t* bytes, uint64_t length, uint64_t offset);

		bool isScanning();
	};
}