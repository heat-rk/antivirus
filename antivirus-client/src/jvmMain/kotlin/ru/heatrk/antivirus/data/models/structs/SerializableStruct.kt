package ru.heatrk.antivirus.data.models.structs

import java.nio.ByteBuffer

abstract class SerializableStruct {

    fun write(bytes: ByteArray) {
        write(ByteBuffer.wrap(bytes))
    }

    abstract fun write(byteBuffer: ByteBuffer)

    abstract class Deserializer<T: SerializableStruct> {
        abstract val size: Int

        fun create(bytes: ByteArray): T  = create(ByteBuffer.wrap(bytes))

        abstract fun create(byteBuffer: ByteBuffer): T
    }
}