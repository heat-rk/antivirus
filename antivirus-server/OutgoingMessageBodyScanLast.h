#pragma once

#include <cstdint>

#include "Serializable.h"

namespace Antivirus {
	class OutgoingMessageBodyScanLast : public Serializable {
	public:
		class Deserializer : public Serializable::Deserializer<OutgoingMessageBodyScanLast> {
		public:
			virtual OutgoingMessageBodyScanLast create(ByteBuffer* byteBuffer) const override {
				OutgoingMessageBodyScanLast body;
				body.virusesCount = byteBuffer->getInt32();
				body.pathLengths = new int32_t[body.virusesCount];
				body.paths = new wchar_t*[body.virusesCount];

				for (int i = 0; i < body.virusesCount; i++) {
					body.pathLengths[i] = byteBuffer->getInt32();
					body.paths[i] = new wchar_t[body.pathLengths[i]];
					byteBuffer->getWChars(body.paths[i], body.pathLengths[i]);
				}

				return body;
			}
		};

		int32_t virusesCount;
		int32_t* pathLengths;
		wchar_t** paths;

		~OutgoingMessageBodyScanLast() {
			for (int i = 0; i < virusesCount; i++) {
				delete paths[i];
			}

			delete paths;
			delete pathLengths;
		}

		virtual void write(ByteBuffer* byteBuffer) override {
			byteBuffer->put(virusesCount);

			for (int i = 0; i < virusesCount; i++) {
				byteBuffer->put(pathLengths[i]);
				byteBuffer->put(paths[i], pathLengths[i]);
			}
		}
	};

	const static OutgoingMessageBodyScanLast::Deserializer outgoingMessageBodyScanLastDeserializer;
}
