#pragma once

#include "VirusRecord.h"

#include <cstdint>
#include <fstream>
#include <iostream>

using namespace std;

namespace Antivirus {
	class SignatureBaseFile {
	protected:
		fstream file;
		uint32_t recordsCount;
	public:
		SignatureBaseFile();
		virtual void close();
		virtual bool isOpen();
		virtual uint32_t getRecordsCount();
	};

	class SignatureBaseFileWriter : public SignatureBaseFile {
	public:
		SignatureBaseFileWriter() : SignatureBaseFile() {}
		bool open(wchar_t* filename, bool trunc);
		bool addRecord(VirusRecord record);
	};

	class SignatureBaseFileReader : public SignatureBaseFile {
	public:
		SignatureBaseFileReader() : SignatureBaseFile() {}
		bool open(wchar_t* filename);
		bool readRecord(VirusRecord* record);
	};
}
