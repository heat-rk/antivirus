package ru.heatrk.antivirus.domain.models

sealed interface ScanState {
    object Empty : ScanState

    data class Running(
        val entries: List<Pair<String, ScannerEntryStatus>>,
        val isPaused: Boolean
    ) : ScanState

    data class VirusesDetected(
        val viruses: List<String>
    ) : ScanState
}