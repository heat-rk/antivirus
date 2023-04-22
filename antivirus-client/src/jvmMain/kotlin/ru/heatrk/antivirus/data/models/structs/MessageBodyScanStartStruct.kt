package ru.heatrk.antivirus.data.models.structs

import ru.heatrk.antivirus.utils.DynamicByteBuffer

class MessageBodyScanStartStruct : SerializableStruct() {
    var path: ByteArray? = null

    override fun write(byteBuffer: DynamicByteBuffer) {
        byteBuffer.putInt(path?.size ?: 0)
        byteBuffer.put(path ?: ByteArray(0))
    }

    companion object : Deserializer<MessageBodyScanStartStruct>() {
        override fun create(byteBuffer: DynamicByteBuffer) = MessageBodyScanStartStruct().apply {
            path = ByteArray(byteBuffer.getInt())
            byteBuffer.get(path)
        }
    }
}