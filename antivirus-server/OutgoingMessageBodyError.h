#pragma once

#include <cstdint>

#include "Serializable.h"

namespace Antivirus {
	class OutgoingMessageBodyError : public Serializable {
	public:
		class Deserializer : public Serializable::Deserializer<OutgoingMessageBodyError> {
		public:
			virtual OutgoingMessageBodyError create(ByteBuffer* byteBuffer) const override {
				OutgoingMessageBodyError body;
				byteBuffer->getChars(body.message, sizeof(body.message));
				return body;
			}
		};

		char message[64];

		virtual void write(ByteBuffer* byteBuffer) override {
			byteBuffer->put(message, sizeof(message));
		}
	};

	const static OutgoingMessageBodyError::Deserializer outgoingMessageBodyErrorDeserializer;
}