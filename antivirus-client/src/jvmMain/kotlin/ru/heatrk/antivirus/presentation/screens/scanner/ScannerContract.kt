package ru.heatrk.antivirus.presentation.screens.scanner

import ru.heatrk.antivirus.presentation.dialogs.MessageDialogState

sealed interface ScannerViewState {
    object Loading : ScannerViewState

    data class Idle(
        val isEnabled: Boolean,
        val isFileDialogVisible: Boolean = false,
        val messageDialogState: MessageDialogState = MessageDialogState.Gone,
        val showAllOkMessage: Boolean = false
    ) : ScannerViewState

    data class Running(
        val progress: Float,
        val scanningPath: String,
        val virusesDetected: Int,
        val isPaused: Boolean,
        val messageDialogState: MessageDialogState = MessageDialogState.Gone
    ) : ScannerViewState

    data class VirusesDetected(
        val virusesDetected: Int,
        val isFileDialogVisible: Boolean = false,
        val messageDialogState: MessageDialogState = MessageDialogState.Gone
    ) : ScannerViewState
}

sealed interface ScannerIntent {
    data class Start(val path: String) : ScannerIntent
    object Pause : ScannerIntent
    object Resume : ScannerIntent
    object Stop : ScannerIntent
    object More : ScannerIntent
    object ShowFileSelectionDialog : ScannerIntent
    object HideFileSelectionDialog : ScannerIntent
    object MessageDialogDismiss : ScannerIntent
}