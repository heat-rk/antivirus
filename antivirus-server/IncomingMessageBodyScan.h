#pragma once

#include <cstdint>
#include <string>

#include "Serializable.h"

namespace Antivirus {
	class IncomingMessageBodyScan : public Serializable {
	public:
		class Deserializer : public Serializable::Deserializer<IncomingMessageBodyScan> {
		public:
			virtual IncomingMessageBodyScan create(ByteBuffer* byteBuffer) const override {
				IncomingMessageBodyScan body;
				int32_t pathLength = byteBuffer->getInt32();
				wchar_t* path = new wchar_t[pathLength];
				byteBuffer->getWChars(path, pathLength);
				body.path = std::wstring(path);
				delete[] path;
				return body;
			}
		};

		std::wstring path;

		virtual void write(ByteBuffer* byteBuffer) override {
			byteBuffer->put((int32_t) path.size());
			byteBuffer->put((wchar_t*) path.c_str(), (int32_t) path.size());
		}
	};

	const static IncomingMessageBodyScan::Deserializer incomingMessageBodyScanDeserializer;
}