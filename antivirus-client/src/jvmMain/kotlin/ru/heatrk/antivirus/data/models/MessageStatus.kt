package ru.heatrk.antivirus.data.models

enum class MessageStatus(val id: Byte) {
    OK(0), ERROR(1), REQUEST(2)
}