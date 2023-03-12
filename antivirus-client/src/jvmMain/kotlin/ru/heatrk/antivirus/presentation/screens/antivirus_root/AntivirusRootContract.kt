package ru.heatrk.antivirus.presentation.screens.antivirus_root

import ru.heatrk.antivirus.presentation.dialogs.DialogState

sealed interface AntivirusRootIntent {
    object DialogDismiss : AntivirusRootIntent
    object Reload : AntivirusRootIntent
}

sealed interface AntivirusRootViewState {
    object Loading : AntivirusRootViewState

    data class Ok(
        val dialogState: DialogState = DialogState.Gone
    ): AntivirusRootViewState

    data class Error(
        val message: String
    ): AntivirusRootViewState
}