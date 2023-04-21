#pragma once

#include <cstdint>

#include "Serializable.h"

namespace Antivirus {
	namespace ProtectionStatus {
		const int8_t E_ENABLED = 0;
		const int8_t E_DISABLED = 1;
	}

	class OutgoingMessageBodyProtection : public Serializable {
	public:
		class Deserializer : public Serializable::Deserializer<OutgoingMessageBodyProtection> {
		public:
			virtual OutgoingMessageBodyProtection create(ByteBuffer* byteBuffer) const override {
				OutgoingMessageBodyProtection body;
				body.status = byteBuffer->getInt8();
				return body;
			}
		};

		int8_t status;

		virtual void write(ByteBuffer* byteBuffer) override {
			byteBuffer->put(status);
		}
	};

	const static OutgoingMessageBodyProtection::Deserializer outgoingMessageBodyProtectionDeserializer;
}