#pragma once

#include "Serializable.h"

#include <cstdint>

#define HASH_SIZE 32

namespace Antivirus {
	class VirusSignature : public Serializable {
	public:
		class Deserializer : public Serializable::Deserializer<VirusSignature> {
		public:
			virtual VirusSignature create(ByteBuffer* byteBuffer) const override {
				VirusSignature signature;
				signature.offset = byteBuffer->getInt32();
				signature.length = byteBuffer->getInt32();
				signature.first = byteBuffer->getInt64();
				byteBuffer->getInt8(signature.hash, sizeof(signature.hash));
				return signature;
			}
		};

		int32_t offset;
		int32_t length;
		int64_t first;
		int8_t hash[HASH_SIZE];

		virtual void write(ByteBuffer* byteBuffer) override {
			byteBuffer->put(offset);
			byteBuffer->put(length);
			byteBuffer->put(first);
			byteBuffer->put(hash, sizeof(hash));
		}
	};

	const static VirusSignature::Deserializer virusSignatureDeserializer;
	const static uint32_t virusSignatureSize = 48;
}
