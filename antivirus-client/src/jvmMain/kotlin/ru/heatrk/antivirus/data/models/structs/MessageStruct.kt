package ru.heatrk.antivirus.data.models.structs

import com.sun.jna.Native
import ru.heatrk.antivirus.utils.DynamicByteBuffer
import java.util.UUID

class MessageStruct : SerializableStruct() {
    val method: ByteArray = ByteArray(32)
    val uuid: ByteArray = Native.toByteArray(UUID.randomUUID().toString())
    var timestamp: Long = System.currentTimeMillis()
    var status: Byte = 0
    var bodySize: Int = 0
    var body: ByteArray? = null

    fun isUuidEquals(other: MessageStruct) =
        uuid.contentEquals(other.uuid) && method.contentEquals(other.method)

    override fun write(byteBuffer: DynamicByteBuffer) {
        byteBuffer.put(method)
        byteBuffer.put(uuid)
        byteBuffer.putLong(timestamp)
        byteBuffer.put(status)
        byteBuffer.putInt(bodySize)
        byteBuffer.put(body ?: ByteArray(0))
    }

    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (javaClass != other?.javaClass) return false

        other as MessageStruct

        if (!method.contentEquals(other.method)) return false
        if (!uuid.contentEquals(other.uuid)) return false
        if (timestamp != other.timestamp) return false
        if (status != other.status) return false
        if (bodySize != other.bodySize) return false
        if (!body.contentEquals(other.body)) return false

        return true
    }

    override fun hashCode(): Int {
        var result = method.contentHashCode()
        result = 31 * result + uuid.contentHashCode()
        result = 31 * result + timestamp.hashCode()
        result = 31 * result + status.hashCode()
        result = 31 * result + bodySize.hashCode()
        result = 31 * result + body.contentHashCode()
        return result
    }

    override fun toString(): String {
        return "MessageStruct(method=${method.contentToString()}, uuid=${uuid.contentToString()}, timestamp=$timestamp, status=$status, bodySize=$bodySize, body=${body.contentToString()})"
    }


    companion object : Deserializer<MessageStruct>() {
        override fun create(byteBuffer: DynamicByteBuffer) = MessageStruct().apply {
            byteBuffer.get(method)
            byteBuffer.get(uuid)
            timestamp = byteBuffer.getLong()
            status = byteBuffer.get()
            bodySize = byteBuffer.getInt()
            body = ByteArray(bodySize)
            byteBuffer.get(body)
        }
    }
}