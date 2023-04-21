package ru.heatrk.antivirus.presentation.screens.scanner.more

import kotlinx.collections.immutable.ImmutableList
import ru.heatrk.antivirus.presentation.dialogs.MessageDialogState
import ru.heatrk.antivirus.presentation.screens.scanner.ScannerIntent

sealed interface ScanningMoreInfoViewState {
    object Loading : ScanningMoreInfoViewState

    data class Ok(
        val isScanned: Boolean,
        val isPaused: Boolean,
        val items: ImmutableList<ScanningMoreInfoItem>,
        val messageDialogState: MessageDialogState = MessageDialogState.Gone
    ) : ScanningMoreInfoViewState
}

sealed interface ScanningMoreInfoIntent {
    object Back : ScanningMoreInfoIntent
    object Pause : ScanningMoreInfoIntent
    object Resume : ScanningMoreInfoIntent
    object Stop : ScanningMoreInfoIntent
    object Quarantine : ScanningMoreInfoIntent
    data class CheckedChange(
        val item: ScanningMoreInfoItem,
        val isChecked: Boolean
    ) : ScanningMoreInfoIntent
}