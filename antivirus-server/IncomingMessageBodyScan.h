#pragma once

#include <cstdint>

#include "Serializable.h"

namespace Antivirus {
	class IncomingMessageBodyScan : public Serializable {
	public:
		class Deserializer : public Serializable::Deserializer<IncomingMessageBodyScan> {
		public:
			virtual IncomingMessageBodyScan create(ByteBuffer* byteBuffer) const override {
				IncomingMessageBodyScan body;
				body.pathLength = byteBuffer->getInt32();
				byteBuffer->getWChars(body.path, body.pathLength);
				return body;
			}
		};

		int32_t pathLength;
		wchar_t* path;

		virtual void write(ByteBuffer* byteBuffer) override {
			byteBuffer->put(pathLength);
			byteBuffer->put(path, pathLength);
		}
	};

	const static IncomingMessageBodyScan::Deserializer incomingMessageBodyScanDeserializer;
}