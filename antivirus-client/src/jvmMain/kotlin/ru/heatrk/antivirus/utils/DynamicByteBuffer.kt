package ru.heatrk.antivirus.utils

import java.nio.Buffer
import java.nio.ByteBuffer
import java.nio.ByteOrder

class DynamicByteBuffer(
    private var byteBuffer: ByteBuffer
) : Comparable<ByteBuffer> {
    

    constructor() : this(ByteBuffer.allocate(0))
    constructor(bytes: ByteArray) : this(ByteBuffer.wrap(bytes))

    fun capacity(): Int {
        return byteBuffer.capacity()
    }

    fun clear() {
        byteBuffer.clear()
    }

    fun flip(): Buffer? {
        return byteBuffer.flip()
    }

    fun hasRemaining(): Boolean {
        return byteBuffer.hasRemaining()
    }

    fun isReadOnly(): Boolean {
        return byteBuffer.isReadOnly
    }

    fun limit(): Int {
        return byteBuffer.limit()
    }

    fun limit(newLimit: Int): Buffer? {
        return byteBuffer.limit(newLimit)
    }

    fun mark(): Buffer? {
        return byteBuffer.mark()
    }

    fun position(): Int {
        return byteBuffer.position()
    }

    fun position(newPosition: Int): Buffer? {
        return byteBuffer.position(newPosition)
    }

    fun remaining(): Int {
        return byteBuffer.remaining()
    }

    fun reset(): Buffer? {
        return byteBuffer.reset()
    }

    fun rewind(): Buffer? {
        return byteBuffer.rewind()
    }

    fun array(): ByteArray? {
        return byteBuffer.array()
    }

    fun arrayOffset(): Int {
        return byteBuffer.arrayOffset()
    }

    fun compact(): ByteBuffer? {
        return byteBuffer.compact()
    }

    override fun compareTo(other: ByteBuffer): Int {
        return byteBuffer.compareTo(other)
    }

    fun duplicate(): ByteBuffer? {
        return byteBuffer.duplicate()
    }

    fun get(): Byte {
        return byteBuffer.get()
    }

    fun get(dst: ByteArray?): ByteBuffer? {
        return byteBuffer[dst]
    }

    fun get(dst: ByteArray?, offset: Int, length: Int): ByteBuffer? {
        return byteBuffer[dst, offset, length]
    }

    fun get(index: Int): Byte {
        return byteBuffer[index]
    }

    fun getChar(): Char {
        return byteBuffer.char
    }

    fun getChar(index: Int): Char {
        return byteBuffer.getChar(index)
    }

    fun getDouble(): Double {
        return byteBuffer.double
    }

    fun getDouble(index: Int): Double {
        return byteBuffer.getDouble(index)
    }

    fun getFloat(): Float {
        return byteBuffer.float
    }

    fun getFloat(index: Int): Float {
        return byteBuffer.getFloat(index)
    }

    fun getInt(): Int {
        return byteBuffer.int
    }

    fun getInt(index: Int): Int {
        return byteBuffer.getInt(index)
    }

    fun getLong(): Long {
        return byteBuffer.long
    }

    fun getLong(index: Int): Long {
        return byteBuffer.getLong(index)
    }

    fun getShort(): Short {
        return byteBuffer.short
    }

    fun getShort(index: Int): Short {
        return byteBuffer.getShort(index)
    }

    fun hasArray(): Boolean {
        return byteBuffer.hasArray()
    }

    fun isDirect(): Boolean {
        return byteBuffer.isDirect
    }

    fun order(): ByteOrder {
        return byteBuffer.order()
    }

    fun order(bo: ByteOrder): ByteBuffer {
        return byteBuffer.order(bo)
    }

    fun put(b: Byte): ByteBuffer {
        ensureSpace(1)
        return byteBuffer.put(b)
    }

    fun put(src: ByteArray): ByteBuffer {
        ensureSpace(src.size)
        return byteBuffer.put(src)
    }

    fun put(src: ByteArray, offset: Int, length: Int): ByteBuffer {
        ensureSpace(length)
        return byteBuffer.put(src, offset, length)
    }

    fun put(src: ByteBuffer): ByteBuffer? {
        ensureSpace(src.remaining())
        return byteBuffer.put(src)
    }

    fun put(index: Int, b: Byte): ByteBuffer {
        ensureSpace(1)
        return byteBuffer.put(index, b)
    }

    fun putChar(value: Char): ByteBuffer {
        ensureSpace(2)
        return byteBuffer.putChar(value)
    }

    fun putChar(index: Int, value: Char): ByteBuffer {
        ensureSpace(2)
        return byteBuffer.putChar(index, value)
    }

    fun putDouble(value: Double): ByteBuffer {
        ensureSpace(8)
        return byteBuffer.putDouble(value)
    }

    fun putDouble(index: Int, value: Double): ByteBuffer {
        ensureSpace(8)
        return byteBuffer.putDouble(index, value)
    }

    fun putFloat(value: Float): ByteBuffer {
        ensureSpace(4)
        return byteBuffer.putFloat(value)
    }

    fun putFloat(index: Int, value: Float): ByteBuffer {
        ensureSpace(4)
        return byteBuffer.putFloat(index, value)
    }

    fun putInt(value: Int): ByteBuffer {
        ensureSpace(4)
        return byteBuffer.putInt(value)
    }

    fun putInt(index: Int, value: Int): ByteBuffer {
        ensureSpace(4)
        return byteBuffer.putInt(index, value)
    }

    fun putLong(index: Int, value: Long): ByteBuffer {
        ensureSpace(8)
        return byteBuffer.putLong(index, value)
    }

    fun putLong(value: Long): ByteBuffer {
        ensureSpace(8)
        return byteBuffer.putLong(value)
    }

    fun putShort(index: Int, value: Short): ByteBuffer {
        ensureSpace(2)
        return byteBuffer.putShort(index, value)
    }

    fun putShort(value: Short): ByteBuffer {
        ensureSpace(2)
        return byteBuffer.putShort(value)
    }

    fun slice(): ByteBuffer {
        return byteBuffer.slice()
    }

    override fun hashCode(): Int {
        return byteBuffer.hashCode()
    }

    override fun toString(): String {
        return byteBuffer.toString()
    }

    fun getByteBuffer(): ByteBuffer {
        return byteBuffer
    }

    private fun ensureSpace(needed: Int) {
        if (remaining() >= needed) {
            return
        }
        val newCapacity = capacity() + needed
        val expanded = ByteBuffer.allocate(newCapacity)
        expanded.order(byteBuffer.order())
        byteBuffer.flip()
        expanded.put(byteBuffer)
        byteBuffer = expanded
    }

    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (javaClass != other?.javaClass) return false

        other as DynamicByteBuffer

        if (byteBuffer != other.byteBuffer) return false

        return true
    }
}