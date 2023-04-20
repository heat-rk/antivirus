#pragma once

#include <cstdint>
#include <vector>

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
				int32_t bodySize = byteBuffer->getInt32();
				byteBuffer->getInt8(&message.body, bodySize);
				return message;
			}
		};

		char method[32] = { 0 };
		char uuid[37] = { 0 };
		int64_t timestamp;
		int8_t status;
		std::vector<int8_t> body;

		virtual void write(ByteBuffer* byteBuffer) override {
			byteBuffer->put(method, sizeof(method));
			byteBuffer->put(uuid, sizeof(uuid));
			byteBuffer->put(timestamp);
			byteBuffer->put(status);
			byteBuffer->put(body);
		}
	};

	const static Message::Deserializer messageDeserializer;
}
