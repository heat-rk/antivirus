package ru.heatrk.antivirus.data.models.structs

import java.nio.ByteBuffer

class MessageBodyStatusStruct : SerializableStruct() {
    var status: Byte = 0

    override fun write(byteBuffer: ByteBuffer) {
        byteBuffer.put(status)
    }

    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (javaClass != other?.javaClass) return false

        other as MessageBodyStatusStruct

        if (status != other.status) return false

        return true
    }

    override fun hashCode(): Int {
        return status.hashCode()
    }

    override fun toString(): String {
        return "MessageBodyStatusStruct(status=$status)"
    }

    companion object : Deserializer<MessageBodyStatusStruct>() {
        override val size = 1

        override fun create(byteBuffer: ByteBuffer) = MessageBodyStatusStruct().apply {
            status = byteBuffer.get()
        }
    }
}