package ru.heatrk.antivirus.domain.models

enum class ScannerStatus(val id: Byte) {
    SCANNING(0), SCANNED(1), PAUSED(2);
}