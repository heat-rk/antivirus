package ru.heatrk.antivirus.presentation.screens.scanner

sealed interface ScannerViewState {
    data class Idle(val isEnabled: Boolean) : ScannerViewState

    data class Running(
        val progress: Float,
        val scanningPath: String,
        val virusesDetected: Int,
        val isPaused: Boolean
    ) : ScannerViewState

    data class VirusesDetected(
        val virusesDetected: Int
    ) : ScannerViewState
}

sealed interface ScannerIntent {
    data class Start(val path: String) : ScannerIntent
    object Pause : ScannerIntent
    object Resume : ScannerIntent
    object Stop : ScannerIntent
    object More : ScannerIntent
}