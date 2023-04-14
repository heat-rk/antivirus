#include "SignatureBaseFile.h"
#include "ByteBuffer.h"
#include "Utils.h"

using namespace Antivirus;

#define SIGN "AvtomatPls"

SignatureBaseFile::SignatureBaseFile() {
	this->m_recordsCount = 0;
}

void SignatureBaseFile::close() {
	if (m_file.is_open()) m_file.close();
}

bool SignatureBaseFile::isOpen() {
	return m_file.is_open();
}

uint32_t SignatureBaseFile::getRecordsCount() {
	return this->m_recordsCount;
}




bool SignatureBaseFileWriter::open(wchar_t* filename, bool trunc) {
	if (filename == NULL) return false;

	if (!isFileExist(filename) || trunc) {
		m_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {
			m_file.open(filename, std::ios::out | std::ios::binary);
		} catch (std::system_error& e) {
			printf("%s\n", e.code().message().c_str());
		}

		if (!m_file.is_open()) {
			return false;
		}

		ByteBuffer byteBuffer(sizeof(SIGN) + sizeof(int32_t));

		byteBuffer.put((char*)SIGN, sizeof(SIGN));
		byteBuffer.put((int32_t) this->m_recordsCount);

		char* bytes = new char[byteBuffer.size()];
		byteBuffer.getChars(bytes, byteBuffer.size());

		m_file.write(bytes, byteBuffer.size());

		delete bytes;
	} else {
		m_file.open(filename, std::ios::in | std::ios::out | std::ios::binary);

		if (!m_file.is_open()) return false;

		ByteBuffer byteBuffer(sizeof(SIGN) + sizeof(uint32_t));

		char* bytes = new char[byteBuffer.size()];
		m_file.read(bytes, byteBuffer.size());
		byteBuffer.put(bytes, byteBuffer.size());

		char sign[sizeof(SIGN)];
		byteBuffer.getChars(sign, sizeof(SIGN));

		if (memcmp(sign, SIGN, sizeof(SIGN))) {
			m_file.close();
			return false;
		}

		this->m_recordsCount = byteBuffer.getInt32();

		delete bytes;
	}

	return true;

}

bool SignatureBaseFileWriter::addRecord(VirusRecord record) {
	if (!m_file.is_open()) return false;

	m_file.seekp(0, std::ios::end);

	ByteBuffer byteBuffer(virusSignatureSize + record.nameLength + sizeof(uint8_t));
	record.write(&byteBuffer);

	char* bytes = new char[byteBuffer.size()];
	byteBuffer.getChars(bytes, byteBuffer.size());

	m_file.write(bytes, byteBuffer.size());

	delete bytes;

	m_file.seekp(sizeof(SIGN), std::ios::beg);

	this->m_recordsCount++;

	byteBuffer.clear();
	byteBuffer.resize(sizeof(int32_t));
	byteBuffer.put((int32_t) this->m_recordsCount);

	char* recordsCountBytes = new char[byteBuffer.size()];
	byteBuffer.getChars(recordsCountBytes, byteBuffer.size());

	m_file.write(recordsCountBytes, byteBuffer.size());

	delete recordsCountBytes;

	return true;
}



bool SignatureBaseFileReader::open(wchar_t* filename) {
	if (filename == NULL) return false;

	if (isFileExist(filename)) {
		m_file.open(filename, std::ios::in | std::ios::out | std::ios::binary);

		if (!m_file.is_open()) return false;

		ByteBuffer byteBuffer(sizeof(SIGN) + sizeof(int32_t));

		char* bytes = new char[byteBuffer.size()];
		m_file.read(bytes, byteBuffer.size());
		byteBuffer.put(bytes, byteBuffer.size());

		char sign[sizeof(SIGN)];
		byteBuffer.getChars(sign, sizeof(SIGN));

		if (memcmp(sign, SIGN, sizeof(SIGN))) {
			m_file.close();
			return false;
		}

		this->m_recordsCount = byteBuffer.getInt32();

		delete bytes;
	}
	else { 
		return false;
	}

	return true;

}

bool SignatureBaseFileReader::readRecord(VirusRecord* record) {
	if (record == NULL || !m_file.is_open()) return false;

	ByteBuffer byteBuffer(sizeof(int8_t));
	char nameLength[sizeof(int8_t)];
	m_file.read(nameLength, sizeof(nameLength));
	byteBuffer.put(nameLength, sizeof(int8_t));
	record->nameLength = byteBuffer.getInt8();

	byteBuffer.clear();
	byteBuffer.resize(virusSignatureSize + record->nameLength + sizeof(int8_t));

	char* bytes = new char[byteBuffer.size()];
	m_file.read(bytes, byteBuffer.size());
	byteBuffer.put(bytes, byteBuffer.size());

	record->name = new char[record->nameLength];
	byteBuffer.getChars(record->name, record->nameLength);
	record->type = byteBuffer.getInt8();
	record->signature = virusSignatureDeserializer.create(&byteBuffer);

	delete bytes;

	return true;
}
