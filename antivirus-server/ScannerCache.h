#pragma once

#include <vector>
#include <Windows.h>
#include <stdio.h>
#include <filesystem>

using namespace std;

namespace Antivirus {
	class ScannerCache {
	public:
		void save(vector<wstring> viruses);
		void load(vector<wstring>* dest);
	};
}
