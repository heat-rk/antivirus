package ru.heatrk.antivirus.presentation.screens.service_control

import ru.heatrk.antivirus.presentation.dialogs.MessageDialogState

sealed interface ProtectionControlViewState {
    object Loading : ProtectionControlViewState

    data class Ok(
        val isServiceEnabled: Boolean,
        val messageDialogState: MessageDialogState = MessageDialogState.Gone
    ) : ProtectionControlViewState
}

sealed interface ProtectionControlIntent {
    object ShowInfo : ProtectionControlIntent
    object DialogDismiss : ProtectionControlIntent

    data class EnabledChange(val isEnabled: Boolean) : ProtectionControlIntent
}