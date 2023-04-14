#include "ByteBuffer.h"

using namespace Antivirus;

ByteBuffer::ByteBuffer(uint32_t size) {
	m_buf.resize(size);
}

ByteBuffer::ByteBuffer(int8_t* arr, uint32_t size) {
	if (arr == NULL) {
		m_buf.resize(size);
	} else { 
		m_buf.resize(size);
		put(arr, size);
	}
}

uint32_t ByteBuffer::bytesRemaining() {
	return size() - m_rpos;
}

void ByteBuffer::clear() {
	m_rpos = 0;
	m_wpos = 0;
	m_buf.clear();
}

void ByteBuffer::resize(uint32_t newSize) {
	m_buf.resize(newSize);
}

uint32_t ByteBuffer::size() {
	return m_buf.size();
}

void ByteBuffer::readedBytes(int8_t** bytes, uint32_t* length) {
	int8_t* buffBytes = new int8_t[m_rpos];
	
	for (int i = 0; i < m_rpos; i++) {
		buffBytes[i] = m_buf[i];
	}

	*bytes = buffBytes;
	*length = m_rpos;
}

void ByteBuffer::getInt8(int8_t* buffer, uint32_t length) const {
	for (uint32_t i = 0; i < length; i++) {
		buffer[i] = getInt8();
	}
}

int8_t ByteBuffer::getInt8() const {
	int8_t value = m_buf[m_rpos];

	m_rpos += 1;

	return value;
}

int16_t ByteBuffer::getInt16() const {
	int16_t value =
		((m_buf[m_rpos + 0] & 0xFF) << 8) +
		((m_buf[m_rpos + 1] & 0xFF) << 0);

	m_rpos += 2;

	return value;
}

int32_t ByteBuffer::getInt32() const {
	int32_t value =
		((m_buf[m_rpos + 0] & 0xFF) << 24) +
		((m_buf[m_rpos + 1] & 0xFF) << 16) +
		((m_buf[m_rpos + 2] & 0xFF) << 8) +
		((m_buf[m_rpos + 3] & 0xFF) << 0);

	m_rpos += 4;

	return value;
}

int64_t ByteBuffer::getInt64() const {
	int64_t value = 
		((m_buf[m_rpos + 0] & (int64_t) 0xFF) << 56) +
		((m_buf[m_rpos + 1] & (int64_t) 0xFF) << 48) +
		((m_buf[m_rpos + 2] & (int64_t) 0xFF) << 40) +
		((m_buf[m_rpos + 3] & (int64_t) 0xFF) << 32) +
		((m_buf[m_rpos + 4] & (int64_t) 0xFF) << 24) +
		((m_buf[m_rpos + 5] & (int64_t) 0xFF) << 16) +
		((m_buf[m_rpos + 6] & (int64_t) 0xFF) << 8) +
		((m_buf[m_rpos + 7] & (int64_t) 0xFF) << 0);

	m_rpos += 8;

	return value;
}

char ByteBuffer::getChar() const {
	char value = m_buf[m_rpos];

	m_rpos += 1;

	return value;
}

void ByteBuffer::getChars(char* buffer, uint32_t length) const {
	for (uint32_t i = 0; i < length; i++) {
		buffer[i] = getChar();
	}
}

wchar_t ByteBuffer::getWChar() const {
	wchar_t value = m_buf[m_rpos];

	m_rpos += 1;

	return value;
}

void ByteBuffer::getWChars(wchar_t* buffer, uint32_t length) const {
	for (uint32_t i = 0; i < length; i++) {
		buffer[i] = getWChar();
	}
}

void ByteBuffer::put(int8_t* bytes, uint32_t length) {
	for (uint32_t i = 0; i < length; i++)
		put(bytes[i]);
}

void ByteBuffer::put(int8_t value) {
	if (size() < (m_wpos + 1))
		m_buf.resize(m_wpos + 1);

	m_buf[m_wpos] = value;

	m_wpos += 1;
}

void ByteBuffer::put(int16_t value) {
	if (size() < (m_wpos + 2))
		m_buf.resize(m_wpos + 2);

	m_buf[m_wpos + 0] = (value >> 8) & 0xFF;
	m_buf[m_wpos + 1] = (value >> 0) & 0xFF;

	m_wpos += 2;
}

void ByteBuffer::put(int32_t value) {
	if (size() < (m_wpos + 4))
		m_buf.resize(m_wpos + 4);

	m_buf[m_wpos + 0] = (value >> 24) & 0xFF;
	m_buf[m_wpos + 1] = (value >> 16) & 0xFF;
	m_buf[m_wpos + 2] = (value >> 8) & 0xFF;
	m_buf[m_wpos + 3] = (value >> 0) & 0xFF;

	m_wpos += 4;
}

void ByteBuffer::put(int64_t value) {
	if (size() < (m_wpos + 8))
		m_buf.resize(m_wpos + 8);

	m_buf[m_wpos + 0] = (value >> 56) & 0xFF;
	m_buf[m_wpos + 1] = (value >> 48) & 0xFF;
	m_buf[m_wpos + 2] = (value >> 40) & 0xFF;
	m_buf[m_wpos + 3] = (value >> 32) & 0xFF;
	m_buf[m_wpos + 4] = (value >> 24) & 0xFF;
	m_buf[m_wpos + 5] = (value >> 16) & 0xFF;
	m_buf[m_wpos + 6] = (value >> 8) & 0xFF;
	m_buf[m_wpos + 7] = (value >> 0) & 0xFF;

	m_wpos += 8;
}

void ByteBuffer::put(char value) {
	if (size() < (m_wpos + 1))
		m_buf.resize(m_wpos + 1);

	m_buf[m_wpos] = value;

	m_wpos += 1;
}

void ByteBuffer::put(char* value, uint32_t length) {
	for (uint32_t i = 0; i < length; i++)
		put(value[i]);
}

void ByteBuffer::put(wchar_t value) {
	if (size() < (m_wpos + 1))
		m_buf.resize(m_wpos + 1);

	m_buf[m_wpos] = value;

	m_wpos += 1;
}

void ByteBuffer::put(wchar_t* value, uint32_t length) {
	for (uint32_t i = 0; i < length; i++)
		put(value[i]);
}