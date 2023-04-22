#pragma once

#include <vector>

#include "ByteBuffer.h"

namespace Antivirus {
	class Serializable {
	public:
		virtual void writeToBytes(int8_t* bytes, uint32_t length) final {
			ByteBuffer byteBuffer(length);
			write(&byteBuffer);
			byteBuffer.getInt8(bytes, length);
		}

		virtual void write(ByteBuffer* byteBuffer) = 0;

		template<class T> class Deserializer {
		public:
			virtual T createFromBytes(
				std::vector<int8_t> bytes
			) const final {
				ByteBuffer byteBuffer(bytes);
				T entity = create(&byteBuffer);
				return entity;
			}

			virtual T createFromBytes(
				int8_t* bytes,
				uint32_t length,
				int8_t** bytesWritten = NULL,
				uint32_t* bytesWrittenLength = NULL
			) const final {
				ByteBuffer byteBuffer(bytes, length);
				T entity = create(&byteBuffer);
				byteBuffer.readedBytes(bytesWritten, bytesWrittenLength);
				return entity;
			}

			virtual T create(ByteBuffer* byteBuffer) const = 0;
		};
	};
}
