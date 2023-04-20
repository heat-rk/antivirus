#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <Windows.h>

namespace Antivirus {
	class ScannerCache {
	public:
		struct DelayedSavingItem {
			std::vector<std::wstring> all;
			std::vector<int8_t> statuses;
			int8_t scannerStatus;
		};

		struct SavingThreadParams {
			int64_t* lastSaveRequestMillis;
			std::vector<DelayedSavingItem>* saveRequests;
			wchar_t* scannerDataFilePath;
			bool* saverEnabled;
		};
	private:
		HANDLE m_savingThread;
		SavingThreadParams* m_savingThreadParams;
		std::vector<DelayedSavingItem> m_saveRequests;
		int64_t m_lastSaveRequestMillis;
		wchar_t* m_scannerDataFilePath;
		bool m_saverEnabled;
	public:
		ScannerCache(bool saverEnabled = false);
		~ScannerCache();

		void save(
			std::vector<std::wstring> all,
			std::vector<int8_t> statuses,
			int8_t scannerStatus
		);

		bool load(
			std::vector<std::wstring>* all,
			std::vector<int8_t>* statuses,
			int8_t* scannerStatus
		);

		void validate();
	};
}
