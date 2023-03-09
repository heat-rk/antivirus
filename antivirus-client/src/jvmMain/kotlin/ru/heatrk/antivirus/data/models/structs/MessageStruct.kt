package ru.heatrk.antivirus.data.models.structs

import com.sun.jna.Native
import java.nio.ByteBuffer
import java.util.UUID

class MessageStruct : SerializableStruct() {
    val method: ByteArray = ByteArray(32)
    val uuid: ByteArray = Native.toByteArray(UUID.randomUUID().toString())
    var timestamp: Long = System.currentTimeMillis()
    var status: Byte = 0
    val body: ByteArray = ByteArray(256)

    fun isUuidEquals(other: MessageStruct) =
        uuid.contentEquals(other.uuid) && method.contentEquals(other.method)

    override fun write(byteBuffer: ByteBuffer) {
        byteBuffer.put(method)
        byteBuffer.put(uuid)
        byteBuffer.putLong(timestamp)
        byteBuffer.put(status)
        byteBuffer.put(body)
    }

    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (javaClass != other?.javaClass) return false

        other as MessageStruct

        if (!method.contentEquals(other.method)) return false
        if (!uuid.contentEquals(other.uuid)) return false
        if (timestamp != other.timestamp) return false
        if (status != other.status) return false
        if (!body.contentEquals(other.body)) return false

        return true
    }

    override fun hashCode(): Int {
        var result = method.contentHashCode()
        result = 31 * result + uuid.contentHashCode()
        result = 31 * result + timestamp.hashCode()
        result = 31 * result + status.hashCode()
        result = 31 * result + body.contentHashCode()
        return result
    }

    override fun toString(): String {
        return "MessageStruct(method=${method.contentToString()}, uuid=${uuid.contentToString()}, timestamp=$timestamp, status=$status, body=${body.contentToString()})"
    }


    companion object : Deserializer<MessageStruct>() {
        override val size = 334

        override fun create(byteBuffer: ByteBuffer) = MessageStruct().apply {
            byteBuffer.get(method)
            byteBuffer.get(uuid)
            timestamp = byteBuffer.long
            status = byteBuffer.get()
            byteBuffer.get(body)
        }
    }
}