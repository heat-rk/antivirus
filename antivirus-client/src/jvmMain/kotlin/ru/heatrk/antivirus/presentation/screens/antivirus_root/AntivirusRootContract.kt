package ru.heatrk.antivirus.presentation.screens.antivirus_root

import ru.heatrk.antivirus.presentation.dialogs.MessageDialogState

sealed interface AntivirusRootIntent {
    object MessageDialogDismiss : AntivirusRootIntent
    object Reload : AntivirusRootIntent
}

sealed interface AntivirusRootViewState {
    object Loading : AntivirusRootViewState

    data class Ok(
        val messageDialogState: MessageDialogState = MessageDialogState.Gone
    ): AntivirusRootViewState

    data class Error(
        val message: String
    ): AntivirusRootViewState
}