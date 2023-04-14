package ru.heatrk.antivirus.presentation.screens.service_control

import ru.heatrk.antivirus.presentation.dialogs.MessageDialogState

sealed interface ServiceControlViewState {
    object Loading : ServiceControlViewState

    data class Ok(
        val isServiceEnabled: Boolean,
        val messageDialogState: MessageDialogState = MessageDialogState.Gone
    ) : ServiceControlViewState
}

sealed interface ServiceControlIntent {
    object ShowInfo : ServiceControlIntent
    object DialogDismiss : ServiceControlIntent

    data class EnabledChange(val isEnabled: Boolean) : ServiceControlIntent
}