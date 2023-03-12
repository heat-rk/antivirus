#include "ByteBuffer.h"

using namespace Antivirus;

ByteBuffer::ByteBuffer(uint32_t size) {
	buf.resize(size);
}

ByteBuffer::ByteBuffer(int8_t* arr, uint32_t size) {
	if (arr == NULL) {
		buf.resize(size);
	} else { 
		buf.resize(size);
		put(arr, size);
	}
}

uint32_t ByteBuffer::bytesRemaining() {
	return size() - rpos;
}

void ByteBuffer::clear() {
	rpos = 0;
	wpos = 0;
	buf.clear();
}

void ByteBuffer::resize(uint32_t newSize) {
	buf.resize(newSize);
	rpos = 0;
	wpos = 0;
}

uint32_t ByteBuffer::size() {
	return buf.size();
}

void ByteBuffer::getInt8(int8_t* buffer, uint32_t length) const {
	for (uint32_t i = 0; i < length; i++) {
		buffer[i] = getInt8();
	}
}

int8_t ByteBuffer::getInt8() const {
	int8_t value = buf[rpos];

	rpos += 1;

	return value;
}

int16_t ByteBuffer::getInt16() const {
	int16_t value =
		((buf[rpos + 0] & 0xFF) << 8) +
		((buf[rpos + 1] & 0xFF) << 0);

	rpos += 2;

	return value;
}

int32_t ByteBuffer::getInt32() const {
	int32_t value =
		((buf[rpos + 0] & 0xFF) << 24) +
		((buf[rpos + 1] & 0xFF) << 16) +
		((buf[rpos + 2] & 0xFF) << 8) +
		((buf[rpos + 3] & 0xFF) << 0);

	rpos += 4;

	return value;
}

int64_t ByteBuffer::getInt64() const {
	int64_t value = 
		((buf[rpos + 0] & (int64_t) 0xFF) << 56) +
		((buf[rpos + 1] & (int64_t) 0xFF) << 48) +
		((buf[rpos + 2] & (int64_t) 0xFF) << 40) +
		((buf[rpos + 3] & (int64_t) 0xFF) << 32) +
		((buf[rpos + 4] & 0xFF) << 24) +
		((buf[rpos + 5] & 0xFF) << 16) +
		((buf[rpos + 6] & 0xFF) << 8) +
		((buf[rpos + 7] & 0xFF) << 0);

	rpos += 8;

	return value;
}

char ByteBuffer::getChar() const {
	char value = buf[rpos];

	rpos += 1;

	return value;
}

void ByteBuffer::getChars(char* buffer, uint32_t length) const {
	for (uint32_t i = 0; i < length; i++) {
		buffer[i] = getChar();
	}
}

void ByteBuffer::put(int8_t* bytes, uint32_t length) {
	for (uint32_t i = 0; i < length; i++)
		put(bytes[i]);
}

void ByteBuffer::put(int8_t value) {
	if (size() < (wpos + 1))
		buf.resize(wpos + 1);

	buf[wpos] = value;

	wpos += 1;
}

void ByteBuffer::put(int16_t value) {
	if (size() < (wpos + 2))
		buf.resize(wpos + 2);

	buf[wpos + 0] = (value >> 8) & 0xFF;
	buf[wpos + 1] = (value >> 0) & 0xFF;

	wpos += 2;
}

void ByteBuffer::put(int32_t value) {
	if (size() < (wpos + 4))
		buf.resize(wpos + 4);

	buf[wpos + 0] = (value >> 24) & 0xFF;
	buf[wpos + 1] = (value >> 16) & 0xFF;
	buf[wpos + 2] = (value >> 8) & 0xFF;
	buf[wpos + 3] = (value >> 0) & 0xFF;

	wpos += 4;
}

void ByteBuffer::put(int64_t value) {
	if (size() < (wpos + 8))
		buf.resize(wpos + 8);

	buf[wpos + 0] = (value >> 56) & 0xFF;
	buf[wpos + 1] = (value >> 48) & 0xFF;
	buf[wpos + 2] = (value >> 40) & 0xFF;
	buf[wpos + 3] = (value >> 32) & 0xFF;
	buf[wpos + 4] = (value >> 24) & 0xFF;
	buf[wpos + 5] = (value >> 16) & 0xFF;
	buf[wpos + 6] = (value >> 8) & 0xFF;
	buf[wpos + 7] = (value >> 0) & 0xFF;

	wpos += 8;
}

void ByteBuffer::put(char value) {
	if (size() < (wpos + 1))
		buf.resize(wpos + 1);

	buf[wpos] = value;

	wpos += 1;
}

void ByteBuffer::put(char* value, uint32_t length) {
	for (uint32_t i = 0; i < length; i++)
		put(value[i]);
}