package ru.heatrk.antivirus.utils.app_unique

import java.io.File
import java.io.RandomAccessFile
import java.nio.channels.FileChannel
import java.nio.channels.FileLock


object AppUniqueUtil {
    private val lockFilesDir = File(System.getProperty("user.home"), ".app_unique_util")
    private val globalLockFile = File(lockFilesDir, "global.lock")
    private var globalFileChannel: FileChannel? = null
    private var globalFileLock: FileLock? = null
    private val locks = mutableMapOf<String, AppUniqueLock>()

    init {
        if (!lockFilesDir.exists()) {
            lockFilesDir.mkdirs()
        }
        val rt = Runtime.getRuntime()
        rt.addShutdownHook(Thread(ShutdownHook()))
    }

    fun isAlreadyRunning(id: String): Boolean {
        val lockFile: File
        val portFile: File
        val fileChannel: FileChannel
        val fileLock: FileLock?
        val nid: String = normalizeID(id)

        globalLock()

        try {
            lockFile = getLockFileForNID(nid)
            portFile = getPortFileForNID(nid)

            lockFilesDir.mkdirs()

            try {
                val raf = RandomAccessFile(lockFile, "rw")
                fileChannel = raf.channel
                fileLock = fileChannel.tryLock() ?: return true
            } catch (t: Throwable) {
                return true
            }

            val lock = AppUniqueLock(
                id = id,
                lockFile = lockFile,
                portFile = portFile,
                lockFileChannel = fileChannel,
                lockFileLock = fileLock
            )

            locks[nid] = lock
        } finally {
            globalUnlock()
        }

        return false
    }

    private fun releaseLock(lock: AppUniqueLock) {
        try {
            lock.lockFileLock.release()
        } catch (_: Throwable) { /* Do nothing */ }

        try {
            lock.lockFileChannel.close()
        } catch (_: Throwable) { /* Do nothing */ }

        lock.portFile.delete()
        lock.lockFile.delete()
    }

    private fun globalLock() {
        do {
            lockFilesDir.mkdirs()
            try {
                val raf = RandomAccessFile(globalLockFile, "rw")
                val channel: FileChannel = raf.channel
                val lock = channel.lock()
                globalFileChannel = channel
                globalFileLock = lock
                break
            } catch (_: Throwable) { /* Do nothing */ }
        } while (true)
    }

    private fun globalUnlock() {
        val channel = globalFileChannel ?: return
        val lock = globalFileLock ?: return

        globalFileChannel = null
        globalFileLock = null

        try {
            lock.release()
        } catch (_: Throwable) {}

        try {
            channel.close()
        } catch (t: Throwable) { /* Do nothing */ }
    }

    private fun normalizeID(id: String): String {
        val hashcode = id.hashCode()
        val positive = hashcode >= 0
        val longCode = if (positive) hashcode.toLong() else -hashcode.toLong()
        val hexString = StringBuffer(java.lang.Long.toHexString(longCode))

        while (hexString.length < 8) {
            hexString.insert(0, '0')
        }

        if (positive) {
            hexString.insert(0, '0')
        } else {
            hexString.insert(0, '1')
        }

        return hexString.toString()
    }

    private fun getLockFileForNID(nid: String): File {
        val filename = normalizeID(nid) + ".lock"
        return File(lockFilesDir, filename)
    }

    private fun getPortFileForNID(nid: String): File {
        val filename = normalizeID(nid) + ".port"
        return File(lockFilesDir, filename)
    }

    private class ShutdownHook : Runnable {
        override fun run() {
            globalLock()
            try {
                val i = locks.keys.iterator()
                while (i.hasNext()) {
                    val lock = locks.remove(i.next()) ?: break
                    releaseLock(lock)
                }
            } finally {
                globalUnlock()
            }
        }
    }
}