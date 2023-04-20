package ru.heatrk.antivirus.data.models

data class ScannerCacheData(
    val scannerStatus: Byte,
    val entries: List<Pair<String, Byte>>
)
