#pragma once

#include <vector>
#include <string>

namespace Antivirus {
	class ScannerCache {
	public:
		void save(std::vector<std::wstring> viruses);
		void load(std::vector<std::wstring>* dest);
	};
}
