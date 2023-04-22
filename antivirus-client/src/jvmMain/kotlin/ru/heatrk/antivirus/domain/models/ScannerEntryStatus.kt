package ru.heatrk.antivirus.domain.models

enum class ScannerEntryStatus(val id: Byte) {
    NOT_SCANNED(0), SCANNED_INFECTED(1), SCANNED_NOT_INFECTED(2);

    companion object {
        fun byId(id: Byte) = when (id) {
            1.toByte() -> SCANNED_INFECTED
            2.toByte() -> SCANNED_NOT_INFECTED
            else -> NOT_SCANNED
        }
    }
}