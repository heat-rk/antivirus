package ru.heatrk.antivirus.data.models.structs

import java.nio.ByteBuffer

class MessageBodyErrorStruct : SerializableStruct() {
    val message = ByteArray(64)

    override fun write(byteBuffer: ByteBuffer) {
        byteBuffer.put(message)
    }

    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (javaClass != other?.javaClass) return false

        other as MessageBodyErrorStruct

        if (!message.contentEquals(other.message)) return false

        return true
    }

    override fun hashCode(): Int {
        return message.contentHashCode()
    }

    override fun toString(): String {
        return "MessageBodyErrorStruct(message=${message.contentToString()})"
    }

    companion object : Deserializer<MessageBodyErrorStruct>() {
        override fun create(byteBuffer: ByteBuffer) = MessageBodyErrorStruct().apply {
            byteBuffer.get(message)
        }
    }
}