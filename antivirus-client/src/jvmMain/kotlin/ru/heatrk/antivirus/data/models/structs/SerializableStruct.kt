package ru.heatrk.antivirus.data.models.structs

import ru.heatrk.antivirus.utils.DynamicByteBuffer

abstract class SerializableStruct {

    fun write(bytes: ByteArray) {
        write(DynamicByteBuffer(bytes))
    }

    abstract fun write(byteBuffer: DynamicByteBuffer)

    abstract class Deserializer<T: SerializableStruct> {
        fun create(bytes: ByteArray): T = create(DynamicByteBuffer(bytes))

        abstract fun create(byteBuffer: DynamicByteBuffer): T
    }
}