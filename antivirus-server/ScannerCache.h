#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>

namespace Antivirus {
	class ScannerCache {
	private:
		wchar_t* m_scannerDataFilePath;
	public:
		ScannerCache();
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
