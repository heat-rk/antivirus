#pragma once

#include <cstdlib>
#include <cstdint>
#include <vector>
#include <memory>

#define BB_DEFAULT_SIZE 4096

namespace Antivirus {
	class ByteBuffer {
	private:
		uint32_t wpos;
		mutable uint32_t rpos;
		std::vector<int8_t> buf;
	public:
		ByteBuffer(uint32_t size = BB_DEFAULT_SIZE);
		ByteBuffer(int8_t* arr, uint32_t size);
		~ByteBuffer() = default;

		uint32_t bytesRemaining(); 
		void clear(); 
		void resize(uint32_t newSize);
		uint32_t size();

		int8_t getInt8() const;
		void getInt8(int8_t* buffer, uint32_t length) const;
		int16_t getInt16() const;
		int32_t getInt32() const;
		int64_t getInt64() const;
		char getChar() const;
		void getChars(char* buffer, uint32_t length) const;

		void put(int8_t* bytes, uint32_t length);
		void put(int8_t b);
		void put(int16_t value);
		void put(int32_t value);
		void put(int64_t value);
		void put(char value);
		void put(char* value, uint32_t length);
	};
}
