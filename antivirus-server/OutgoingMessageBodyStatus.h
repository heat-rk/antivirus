#pragma once

#include <cstdint>

#include "Serializable.h"

namespace Antivirus {
	namespace StatusValue {
		const int8_t E_STATUS_OK = 0;
		const int8_t E_STATUS_ERROR = 1;
	}

	class OutgoingMessageBodyStatus : public Serializable {
	public:
		class Deserializer : public Serializable::Deserializer<OutgoingMessageBodyStatus> {
		public:
			virtual OutgoingMessageBodyStatus create(ByteBuffer byteBuffer) const override {
				OutgoingMessageBodyStatus body;
				body.status = byteBuffer.getInt8();
				return body;
			}
		};

		int8_t status;

		virtual void write(ByteBuffer* byteBuffer) override {
			byteBuffer->put(status);
		}
	};

	const static OutgoingMessageBodyStatus::Deserializer outgoingMessageBodyStatusDeserializer;
}