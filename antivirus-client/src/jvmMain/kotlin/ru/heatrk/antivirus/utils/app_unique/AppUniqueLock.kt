package ru.heatrk.antivirus.utils.app_unique

import java.io.File
import java.nio.channels.FileChannel
import java.nio.channels.FileLock

data class AppUniqueLock(
    val id: String,
    val lockFile: File,
    val portFile: File,
    val lockFileChannel: FileChannel,
    val lockFileLock: FileLock
)
