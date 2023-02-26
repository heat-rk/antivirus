package ru.heatrk.antivirus.data.models.structs

import java.nio.ByteBuffer

class MessageStruct : SerializableStruct() {
    val source = ByteArray(32)
    val target = ByteArray(32)
    var timestamp: Long = System.currentTimeMillis()
    var type: Byte = 0
    val body = ByteArray(256)

    override fun write(byteBuffer: ByteBuffer) {
        byteBuffer.put(source)
        byteBuffer.put(target)
        byteBuffer.putLong(timestamp)
        byteBuffer.put(type)
        byteBuffer.put(body)
    }

    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (javaClass != other?.javaClass) return false

        other as MessageStruct

        if (!source.contentEquals(other.source)) return false
        if (!target.contentEquals(other.target)) return false
        if (timestamp != other.timestamp) return false
        if (type != other.type) return false
        if (!body.contentEquals(other.body)) return false

        return true
    }

    override fun hashCode(): Int {
        var result = source.contentHashCode()
        result = 31 * result + target.contentHashCode()
        result = 31 * result + timestamp.hashCode()
        result = 31 * result + type.hashCode()
        result = 31 * result + body.contentHashCode()
        return result
    }

    override fun toString(): String {
        return "MessageStruct(source=${source.contentToString()}, target=${target.contentToString()}, timestamp=$timestamp, type=$type, body=${body.contentToString()})"
    }


    companion object : Deserializer<MessageStruct>() {
        override val size = 329

        override fun create(byteBuffer: ByteBuffer) = MessageStruct().apply {
            byteBuffer.get(source)
            byteBuffer.get(target)
            timestamp = byteBuffer.long
            type = byteBuffer.get()
            byteBuffer.get(body)
        }
    }
}