package ru.heatrk.antivirus.data.models

import com.sun.jna.Native

enum class MessageMethod(val id: ByteArray) {
    GET_STATUS(Native.toByteArray("get_status")),
    SCAN_LAST(Native.toByteArray("scan_last")),
    SCAN_START(Native.toByteArray("scan_start")),
    SCAN_PAUSE(Native.toByteArray("scan_pause")),
    SCAN_STOP(Native.toByteArray("scan_stop")),
}