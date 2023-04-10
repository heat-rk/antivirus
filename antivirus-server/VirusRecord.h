#pragma once

#include "VirusSignature.h"
#include "Serializable.h"

#include <cstdint>

namespace Antivirus {
	class VirusRecord : public Serializable {
	public:
		class Deserializer : public Serializable::Deserializer<VirusRecord> {
		public:
			virtual VirusRecord create(ByteBuffer byteBuffer) const override {
				VirusRecord record;
				record.nameLength = byteBuffer.getInt8();
				byteBuffer.getChars(record.name, record.nameLength);
				record.type = byteBuffer.getInt8();
				record.signature = virusSignatureDeserializer.create(byteBuffer);
				return record;
			}
		};

		int8_t nameLength;
		char* name;
		int8_t type;
		VirusSignature signature;

		virtual void write(ByteBuffer* byteBuffer) override {
			byteBuffer->put(nameLength);
			byteBuffer->put(name, nameLength);
			byteBuffer->put(type);
			signature.write(byteBuffer);
		}
	};

	const static VirusRecord::Deserializer virusRecordDeserializer;
}
