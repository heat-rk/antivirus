#pragma once

#include "VirusSignature.h"
#include "Serializable.h"

#include <cstdint>
#include <string>

namespace Antivirus {
	class VirusRecord : public Serializable {
	public:
		class Deserializer : public Serializable::Deserializer<VirusRecord> {
		public:
			virtual VirusRecord create(ByteBuffer* byteBuffer) const override {
				VirusRecord record;
				int8_t nameLength = byteBuffer->getInt8();
				char* name = new char[nameLength];
				byteBuffer->getChars(name, nameLength);
				record.name = std::string(name);
				delete[] name;
				record.type = byteBuffer->getInt8();
				record.signature = virusSignatureDeserializer.create(byteBuffer);
				return record;
			}
		};

		std::string name;
		int8_t type;
		VirusSignature signature;

		virtual void write(ByteBuffer* byteBuffer) override {
			byteBuffer->put((int8_t) name.size());
			byteBuffer->put((char*) name.c_str(), (int8_t) name.size());
			byteBuffer->put(type);
			signature.write(byteBuffer);
		}
	};

	const static VirusRecord::Deserializer virusRecordDeserializer;
}
