package ru.heatrk.antivirus.data.models

import com.sun.jna.Native

enum class MessageMethod(val id: ByteArray) {
    SCAN_START(Native.toByteArray("scan_start")),
    SCAN_PAUSE(Native.toByteArray("scan_pause")),
    SCAN_RESUME(Native.toByteArray("scan_resume")),
    SCAN_STOP(Native.toByteArray("scan_stop")),
}