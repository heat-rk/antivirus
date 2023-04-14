#pragma once

#include <cstdint>

#include "Serializable.h"

namespace Antivirus {

	class Message : public Serializable {
	public:
		class Deserializer : public Serializable::Deserializer<Message> {
		public:
			virtual Message create(ByteBuffer* byteBuffer) const override {
				Message message;
				byteBuffer->getChars(message.method, sizeof(message.method));
				byteBuffer->getChars(message.uuid, sizeof(message.uuid));
				message.timestamp = byteBuffer->getInt64();
				message.status = byteBuffer->getInt8();
				message.bodySize = byteBuffer->getInt32();
				message.body = new int8_t[message.bodySize];
				byteBuffer->getInt8(message.body, message.bodySize);
				return message;
			}
		};

		char method[32] = { 0 };
		char uuid[37] = { 0 };
		int64_t timestamp;
		int8_t status;
		int32_t bodySize;
		int8_t* body;

		virtual void write(ByteBuffer* byteBuffer) override {
			byteBuffer->put(method, sizeof(method));
			byteBuffer->put(uuid, sizeof(uuid));
			byteBuffer->put(timestamp);
			byteBuffer->put(status);
			byteBuffer->put(bodySize);
			byteBuffer->put(body, bodySize);
		}
	};

	const static Message::Deserializer messageDeserializer;
}
