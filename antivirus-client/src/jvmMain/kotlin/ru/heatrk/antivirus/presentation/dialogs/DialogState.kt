package ru.heatrk.antivirus.presentation.dialogs

sealed interface DialogState {
    object Gone: DialogState

    data class Error(
        val title: String,
        val message: String
    ) : DialogState

    data class Info(
        val title: String,
        val message: String
    ) : DialogState
}