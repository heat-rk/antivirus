#pragma once

#include <cstdint>

#include "Serializable.h"

#define MESSAGE_BYTES_LENGTH 334

namespace Antivirus {

	class Message : public Serializable {
	public:
		class Deserializer : public Serializable::Deserializer<Message> {
		public:
			virtual Message create(ByteBuffer byteBuffer) const override {
				Message message;
				byteBuffer.getChars(message.method, sizeof(message.method));
				byteBuffer.getChars(message.uuid, sizeof(message.uuid));
				message.timestamp = byteBuffer.getInt64();
				message.status = byteBuffer.getInt8();
				byteBuffer.getInt8(message.body, sizeof(message.body));
				return message;
			}
		};

		char method[32] = { 0 };
		char uuid[37] = { 0 };
		int64_t timestamp;
		int8_t status;
		int8_t body[256] = { 0 };

		virtual void write(ByteBuffer* byteBuffer) override {
			byteBuffer->put(method, sizeof(method));
			byteBuffer->put(uuid, sizeof(uuid));
			byteBuffer->put(timestamp);
			byteBuffer->put(status);
			byteBuffer->put(body, sizeof(body));
		}
	};

	const static Message::Deserializer messageDeserializer;
}
