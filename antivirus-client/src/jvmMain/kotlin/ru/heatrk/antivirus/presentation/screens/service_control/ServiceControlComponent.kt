package ru.heatrk.antivirus.presentation.screens.service_control

import com.arkivanov.decompose.ComponentContext
import kotlinx.coroutines.channels.Channel
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.receiveAsFlow
import kotlinx.coroutines.launch
import ru.heatrk.antivirus.presentation.common.Component
import ru.heatrk.antivirus.presentation.dialogs.MessageDialogState
import ru.heatrk.antivirus.presentation.screens.ServiceStatusListener
import ru.heatrk.antivirus.presentation.values.strings.strings

class ServiceControlComponent(
    componentContext: ComponentContext
) : Component(componentContext), ServiceStatusListener {
    private val _state = MutableStateFlow<ServiceControlViewState>(ServiceControlViewState.Loading)
    val state = _state.asStateFlow()

    private val _serviceStartEvents = Channel<Unit>(Channel.BUFFERED)
    val serviceStartEvents = _serviceStartEvents.receiveAsFlow()

    private val _serviceStopEvents = Channel<Unit>(Channel.BUFFERED)
    val serviceStopEvents = _serviceStopEvents.receiveAsFlow()

    fun onIntent(intent: ServiceControlIntent) = componentScope.launch {
        when (intent) {
            is ServiceControlIntent.EnabledChange -> {
                if (intent.isEnabled) {
                    _serviceStartEvents.send(Unit)
                } else {
                    _serviceStopEvents.send(Unit)
                }
            }

            ServiceControlIntent.ShowInfo -> {
                val state = _state.value

                if (state !is ServiceControlViewState.Ok) {
                    return@launch
                }

                _state.value = state.copy(
                    messageDialogState = MessageDialogState.Info(
                        title = strings.info,
                        message = strings.serviceStatusInfoDialogMessage
                    )
                )
            }

            ServiceControlIntent.DialogDismiss -> {
                val state = _state.value

                if (state !is ServiceControlViewState.Ok) {
                    return@launch
                }

                _state.value = state.copy(messageDialogState = MessageDialogState.Gone)
            }
        }
    }

    override fun onStatusLoading() = Unit

    override fun onStatusReceived(isEnabled: Boolean) {
        _state.value = ServiceControlViewState.Ok(isServiceEnabled = isEnabled)
    }

    data class Args(
        val componentContext: ComponentContext
    )

    companion object {
        fun create(
            args: Args
        ) = ServiceControlComponent(
            componentContext = args.componentContext
        )
    }
}