package ru.heatrk.antivirus.data.models

import com.sun.jna.Native

enum class MessageMethod(val id: ByteArray) {
    GET_STATUS(Native.toByteArray("get_status"))
}