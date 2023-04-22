package ru.heatrk.antivirus.presentation.dialogs

sealed interface MessageDialogState {
    object Gone: MessageDialogState

    data class Error(
        val title: String,
        val message: String
    ) : MessageDialogState

    data class Info(
        val title: String,
        val message: String
    ) : MessageDialogState
}