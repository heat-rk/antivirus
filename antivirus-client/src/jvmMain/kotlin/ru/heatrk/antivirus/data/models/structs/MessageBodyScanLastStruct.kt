package ru.heatrk.antivirus.data.models.structs

import java.nio.ByteBuffer

class MessageBodyScanLastStruct : SerializableStruct() {
    val viruses = mutableListOf<ByteArray>()

    override fun write(byteBuffer: ByteBuffer) {
        byteBuffer.putInt(viruses.size)

        viruses.forEach { virus ->
            byteBuffer.putInt(virus.size)
            byteBuffer.put(virus)
        }
    }

    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (javaClass != other?.javaClass) return false

        other as MessageBodyScanLastStruct

        if (viruses != other.viruses) return false

        return true
    }

    override fun hashCode(): Int {
        return viruses.hashCode()
    }

    override fun toString(): String {
        return "MessageBodyScanLastStruct(viruses=$viruses)"
    }

    companion object : Deserializer<MessageBodyScanLastStruct>() {
        override fun create(byteBuffer: ByteBuffer) = MessageBodyScanLastStruct().apply {
            val virusesCount = byteBuffer.int

            for (i in 0 until virusesCount) {
                val pathLength = byteBuffer.int
                val path = ByteArray(pathLength)
                byteBuffer.get(path)
            }
        }
    }
}