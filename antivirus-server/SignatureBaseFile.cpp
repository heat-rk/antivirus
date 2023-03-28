#include "SignatureBaseFile.h"
#include "ByteBuffer.h"
#include "Utils.h"

using namespace Antivirus;

#define SIGN "AvtomatPls"

SignatureBaseFile::SignatureBaseFile() {
	this->recordsCount = 0;
}

void SignatureBaseFile::close() {
	if (file.is_open()) file.close();
}

bool SignatureBaseFile::isOpen() {
	return file.is_open();
}

uint32_t SignatureBaseFile::getRecordsCount() {
	return this->recordsCount;
}




bool SignatureBaseFileWriter::open(wchar_t* filename, bool trunc) {
	if (filename == NULL) return false;

	if (!isFileExist(filename) || trunc) {
		file.open(filename, ios::out | ios::binary);

		if (!file.is_open()) return false;

		ByteBuffer byteBuffer(sizeof(SIGN) + sizeof(uint32_t));

		byteBuffer.put((char*)SIGN, sizeof(SIGN));
		byteBuffer.put(this->recordsCount);

		char* bytes = new char[byteBuffer.size()];
		byteBuffer.getChars(bytes, byteBuffer.size());

		file.write(bytes, byteBuffer.size());

		delete bytes;
	} else {
		file.open(filename, ios::in | ios::out | ios::binary);

		if (!file.is_open()) return false;

		ByteBuffer byteBuffer(sizeof(SIGN) + sizeof(uint32_t));

		char* bytes = new char[byteBuffer.size()];
		file.read(bytes, byteBuffer.size());
		byteBuffer.put(bytes, byteBuffer.size());

		char sign[sizeof(SIGN)];
		byteBuffer.getChars(sign, sizeof(SIGN));

		if (memcmp(sign, SIGN, sizeof(SIGN))) {
			file.close();
			return false;
		}

		this->recordsCount = byteBuffer.getUInt32();

		delete bytes;
	}

	return true;

}

bool SignatureBaseFileWriter::addRecord(VirusRecord record) {
	if (!file.is_open()) return false;

	file.seekp(0, ios::end);

	ByteBuffer byteBuffer(virusSignatureSize + record.nameLength + sizeof(uint8_t));
	record.write(&byteBuffer);

	char* bytes = new char[byteBuffer.size()];
	byteBuffer.getChars(bytes, byteBuffer.size());

	file.write(bytes, byteBuffer.size());

	delete bytes;

	file.seekp(sizeof(SIGN), ios::beg);

	this->recordsCount++;

	byteBuffer.clear();
	byteBuffer.resize(sizeof(uint32_t));
	byteBuffer.put(this->recordsCount);

	char* recordsCountBytes = new char[byteBuffer.size()];
	byteBuffer.getChars(recordsCountBytes, byteBuffer.size());

	file.write(recordsCountBytes, byteBuffer.size());

	delete recordsCountBytes;

	return true;
}



bool SignatureBaseFileReader::open(wchar_t* filename) {
	if (filename == NULL) return false;

	if (isFileExist(filename)) {
		file.open(filename, ios::in | ios::out | ios::binary);

		if (!file.is_open()) return false;

		ByteBuffer byteBuffer(sizeof(SIGN) + sizeof(uint32_t));

		char* bytes = new char[byteBuffer.size()];
		file.read(bytes, byteBuffer.size());
		byteBuffer.put(bytes, byteBuffer.size());

		char sign[sizeof(SIGN)];
		byteBuffer.getChars(sign, sizeof(SIGN));

		if (memcmp(sign, SIGN, sizeof(SIGN))) {
			file.close();
			return false;
		}

		this->recordsCount = byteBuffer.getUInt32();

		delete bytes;
	}
	else { 
		return false;
	}

	return true;

}

bool SignatureBaseFileReader::readRecord(VirusRecord* record) {
	if (record == NULL || !file.is_open()) return false;

	ByteBuffer byteBuffer(sizeof(uint8_t));
	char nameLength[sizeof(uint8_t)];
	file.read(nameLength, sizeof(nameLength));
	byteBuffer.put(nameLength, sizeof(uint8_t));
	record->nameLength = byteBuffer.getUInt8();

	byteBuffer.clear();
	byteBuffer.resize(virusSignatureSize + record->nameLength);

	char* bytes = new char[byteBuffer.size()];
	file.read(bytes, byteBuffer.size());
	byteBuffer.put(bytes, byteBuffer.size());

	record->name = new char[record->nameLength];
	byteBuffer.getChars(record->name, record->nameLength);
	record->signature = virusSignatureDeserializer.create(byteBuffer);

	delete bytes;

	return true;
}
