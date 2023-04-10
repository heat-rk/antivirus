#pragma once

#include <cstdint>

#include "Serializable.h"

namespace Antivirus {
	class OutgoingMessageBodyScan : public Serializable {
	public:
		class Deserializer : public Serializable::Deserializer<OutgoingMessageBodyScan> {
		public:
			virtual OutgoingMessageBodyScan create(ByteBuffer byteBuffer) const override {
				OutgoingMessageBodyScan body;
				body.progress = byteBuffer.getInt32();
				body.total = byteBuffer.getInt32();
				body.pathLength = byteBuffer.getInt16();
				byteBuffer.getWChars(body.path, body.pathLength);
				body.infected = byteBuffer.getInt8();
				return body;
			}
		};

		int32_t progress;
		int32_t total;
		int16_t pathLength;
		wchar_t* path;
		int8_t infected;

		~OutgoingMessageBodyScan() {
			delete path;
		}

		virtual void write(ByteBuffer* byteBuffer) override {
			byteBuffer->put(progress);
			byteBuffer->put(total);
			byteBuffer->put(pathLength);
			byteBuffer->put(path, pathLength);
			byteBuffer->put(infected);
		}
	};

	const static OutgoingMessageBodyScan::Deserializer outgoingMessageBodyScanDeserializer;
}