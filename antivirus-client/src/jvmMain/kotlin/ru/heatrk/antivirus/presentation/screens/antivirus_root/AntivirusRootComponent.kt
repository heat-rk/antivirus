package ru.heatrk.antivirus.presentation.screens.antivirus_root

import com.arkivanov.decompose.ComponentContext
import com.arkivanov.decompose.childContext
import kotlinx.coroutines.CoroutineDispatcher
import kotlinx.coroutines.Job
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.launchIn
import kotlinx.coroutines.flow.onEach
import kotlinx.coroutines.launch
import org.kodein.di.DI
import ru.heatrk.antivirus.data.models.ApiMessage
import ru.heatrk.antivirus.domain.repositories.MessagingRepository
import ru.heatrk.antivirus.presentation.common.Component
import ru.heatrk.antivirus.presentation.dialogs.DialogState
import ru.heatrk.antivirus.presentation.screens.service_control.ServiceControlComponent
import ru.heatrk.antivirus.presentation.values.strings.strings

class AntivirusRootComponent(
    componentContext: ComponentContext,
    private val messagingRepository: MessagingRepository,
    private val defaultDispatcher: CoroutineDispatcher,
    di: DI
): Component(componentContext) {

    val serviceControlComponent = ServiceControlComponent(
        componentContext = childContext(key = "serviceControlContext")
    )

    private val _state = MutableStateFlow<AntivirusRootViewState>(AntivirusRootViewState.Loading)
    val state = _state.asStateFlow()

    private val listenersJobs = mutableListOf<Job>()

    init {
        load()
    }

    fun onIntent(intent: AntivirusRootIntent) = componentScope.launch {
        when (intent) {
            AntivirusRootIntent.DialogDismiss -> {
                val state = _state.value

                if (state !is AntivirusRootViewState.Ok) {
                    return@launch
                }

                _state.value = state.copy(dialogState = DialogState.Gone)
            }

            AntivirusRootIntent.Reload -> {
                load()
            }
        }
    }

    private fun load() = componentScope.launch {
        _state.value = AntivirusRootViewState.Loading

        when (val result = messagingRepository.isServiceEnabled()) {
            is ApiMessage.Ok -> {
                _state.value = AntivirusRootViewState.Ok()
                serviceControlComponent.onStatusReceived(result.body)
                initListeners()
            }

            is ApiMessage.Fail -> {
                _state.value = AntivirusRootViewState.Error(
                    strings.errorMessage(
                        description = result.description,
                        errorCode = result.errorCode
                    )
                )
            }
        }
    }

    private fun initListeners() {
        listenersJobs.forEach(Job::cancel)
        listenersJobs.clear()

        serviceControlComponent.serviceStartEvents
            .onEach {
                serviceControlComponent.onStatusLoading()

                when (val result = messagingRepository.startService()) {
                    is ApiMessage.Ok -> {
                        serviceControlComponent.onStatusReceived(true)
                    }

                    is ApiMessage.Fail -> {
                        serviceControlComponent.onStatusReceived(false)

                        _state.value = AntivirusRootViewState.Ok(
                            dialogState = DialogState.Error(
                                title = strings.error,
                                message = strings.errorMessage(
                                    description = result.description,
                                    errorCode = result.errorCode
                                )
                            )
                        )
                    }
                }
            }
            .launchIn(componentScope)
            .also { listenersJobs.add(it) }

        serviceControlComponent.serviceStopEvents
            .onEach {
                serviceControlComponent.onStatusLoading()

                when (val result = messagingRepository.stopService()) {
                    is ApiMessage.Ok -> {
                        serviceControlComponent.onStatusReceived(false)
                    }

                    is ApiMessage.Fail -> {
                        serviceControlComponent.onStatusReceived(true)

                        _state.value = AntivirusRootViewState.Ok(
                            dialogState = DialogState.Error(
                                title = strings.error,
                                message = strings.errorMessage(
                                    description = result.description,
                                    errorCode = result.errorCode
                                )
                            )
                        )
                    }
                }
            }
            .launchIn(componentScope)
            .also { listenersJobs.add(it) }
    }

    data class Args(
        val componentContext: ComponentContext
    )

    companion object {
        fun create(
            args: Args,
            messagingRepository: MessagingRepository,
            defaultDispatcher: CoroutineDispatcher,
            di: DI
        ) = AntivirusRootComponent(
            componentContext = args.componentContext,
            di = di,
            messagingRepository = messagingRepository,
            defaultDispatcher = defaultDispatcher
        )
    }
}