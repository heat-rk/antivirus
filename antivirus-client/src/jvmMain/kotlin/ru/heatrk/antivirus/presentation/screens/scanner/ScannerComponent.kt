package ru.heatrk.antivirus.presentation.screens.scanner

import com.arkivanov.decompose.ComponentContext
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.launchIn
import kotlinx.coroutines.flow.onEach
import kotlinx.coroutines.launch
import ru.heatrk.antivirus.data.models.ApiMessage
import ru.heatrk.antivirus.domain.models.ScanState
import ru.heatrk.antivirus.domain.models.ScannerEntryStatus
import ru.heatrk.antivirus.domain.repositories.MessagingRepository
import ru.heatrk.antivirus.presentation.common.Component
import ru.heatrk.antivirus.presentation.dialogs.MessageDialogState
import ru.heatrk.antivirus.presentation.screens.ServiceStatus
import ru.heatrk.antivirus.presentation.screens.ServiceStatusListener
import ru.heatrk.antivirus.presentation.values.strings.strings

class ScannerComponent(
    componentContext: ComponentContext,
    private val messagingRepository: MessagingRepository
) : Component(componentContext), ServiceStatusListener {

    private val _serviceStatus = MutableStateFlow(ServiceStatus.LOADING)
    private val _scanState = messagingRepository.scanState(componentScope)

    private val _state = MutableStateFlow<ScannerViewState>(ScannerViewState.Loading)
    val state = _state.asStateFlow()

    init {
        _serviceStatus.onEach { updateState() }.launchIn(componentScope)
        _scanState.onEach { updateState() }.launchIn(componentScope)
    }

    override fun onStatusReceived(status: ServiceStatus) {
        _serviceStatus.value = status
    }

    fun onIntent(intent: ScannerIntent) {
        when (intent) {
            is ScannerIntent.More -> Unit
            is ScannerIntent.Pause -> applyPauseScanIntent(intent)
            is ScannerIntent.Resume -> applyResumeScanIntent(intent)
            is ScannerIntent.Start -> applyStartScanIntent(intent)
            is ScannerIntent.Stop -> applyStopScanIntent(intent)
            is ScannerIntent.ShowFileSelectionDialog -> applySelectFileIntent(intent)
            is ScannerIntent.HideFileSelectionDialog -> applyHideFileSelectionDialogIntent(intent)
            is ScannerIntent.MessageDialogDismiss -> applyMessageDialogDismiss(intent)
        }
    }

    private fun updateState() = when (_serviceStatus.value) {
        ServiceStatus.LOADING -> {
            updateLoadingState()
        }
        ServiceStatus.DISABLED -> {
            updateIdleState(isEnabled = false)
        }
        else -> when (val scanState = _scanState.value) {
            ScanState.Empty -> {
                updateIdleState(isEnabled = true)
            }
            is ScanState.Running -> {
                val scanned = scanState.entries
                    .count { it.second != ScannerEntryStatus.NOT_SCANNED }

                val virusesCount = scanState.entries
                    .count { it.second == ScannerEntryStatus.SCANNED_INFECTED }

                val scanningPath = scanState.entries
                    .find { it.second == ScannerEntryStatus.NOT_SCANNED }
                    ?.first ?: strings.ellipsis

                val progress = if (scanState.entries.isNotEmpty()) {
                    scanned / scanState.entries.size.toFloat()
                } else {
                    0f
                }

                updateRunningState(
                    progress = progress,
                    scanningPath = scanningPath,
                    virusesDetected = virusesCount,
                    isPaused = scanState.isPaused
                )
            }
            is ScanState.VirusesDetected -> {
                updateVirusesDetectedState(virusesDetected = scanState.viruses.size)
            }
        }
    }

    private fun applyMessageDialogDismiss(intent: ScannerIntent.MessageDialogDismiss) {
        _state.value = when (val state = state.value) {
            is ScannerViewState.Idle -> {
                state.copy(messageDialogState = MessageDialogState.Gone)
            }
            is ScannerViewState.Running -> {
                state.copy(messageDialogState = MessageDialogState.Gone)
            }
            is ScannerViewState.VirusesDetected -> {
                state.copy(messageDialogState = MessageDialogState.Gone)
            }
            else -> state
        }
    }

    private fun applySelectFileIntent(intent: ScannerIntent.ShowFileSelectionDialog) {
        when (val state = state.value) {
            is ScannerViewState.Idle -> {
                _state.value = state.copy(isFileDialogVisible = true)
            }
            is ScannerViewState.VirusesDetected -> {
                _state.value = state.copy(isFileDialogVisible = true)
            }
            else -> Unit
        }
    }

    private fun applyHideFileSelectionDialogIntent(
        intent: ScannerIntent.HideFileSelectionDialog
    ) {
        when (val state = state.value) {
            is ScannerViewState.Idle -> {
                _state.value = state.copy(isFileDialogVisible = false)
            }
            is ScannerViewState.VirusesDetected -> {
                _state.value = state.copy(isFileDialogVisible = false)
            }
            else -> Unit
        }
    }

    private fun applyStartScanIntent(intent: ScannerIntent.Start) = componentScope.launch {
        handleRequestResult(messagingRepository.startScan(path = intent.path))
    }

    private fun applyPauseScanIntent(intent: ScannerIntent.Pause) = componentScope.launch {
        handleRequestResult(messagingRepository.pauseScan())
    }

    private fun applyResumeScanIntent(intent: ScannerIntent.Resume) = componentScope.launch {
        handleRequestResult(messagingRepository.resumeScan())
    }

    private fun applyStopScanIntent(intent: ScannerIntent.Stop) = componentScope.launch {
        handleRequestResult(messagingRepository.stopScan())
    }

    private fun updateLoadingState() {
        _state.value = ScannerViewState.Loading
    }

    private fun updateIdleState(
        isEnabled: Boolean
    ) {
        val state = _state.value

        _state.value = if (state is ScannerViewState.Idle) {
            state.copy(isEnabled = isEnabled)
        } else {
            val showAllOkMessage = state is ScannerViewState.Running

            ScannerViewState.Idle(
                isEnabled = isEnabled,
                showAllOkMessage = showAllOkMessage
            )
        }
    }

    private fun updateRunningState(
        progress: Float,
        scanningPath: String,
        virusesDetected: Int,
        isPaused: Boolean
    ) {
        val state = _state.value

        _state.value = if (state is ScannerViewState.Running) {
            state.copy(
                progress = progress,
                scanningPath = scanningPath,
                virusesDetected = virusesDetected,
                isPaused = isPaused
            )
        } else {
            ScannerViewState.Running(
                progress = progress,
                scanningPath = scanningPath,
                virusesDetected = virusesDetected,
                isPaused = isPaused
            )
        }
    }

    private fun updateVirusesDetectedState(
        virusesDetected: Int
    ) {
        val state = _state.value

        _state.value = if (state is ScannerViewState.VirusesDetected) {
            state.copy(virusesDetected = virusesDetected)
        } else {
            ScannerViewState.VirusesDetected(virusesDetected = virusesDetected)
        }
    }

    private fun <T> handleRequestResult(result: ApiMessage<T>, onSuccess: ((T) -> Unit)? = null) {
        when (result) {
            is ApiMessage.Ok -> {
                onSuccess?.invoke(result.body)
            }
            is ApiMessage.Fail -> {
                _state.value = when (val state = _state.value) {
                    is ScannerViewState.Idle -> {
                        state.copy(
                            messageDialogState = MessageDialogState.Error(
                                title = strings.error,
                                message = strings.errorMessage(
                                    description = result.description,
                                    errorCode = result.errorCode
                                )
                            )
                        )
                    }
                    is ScannerViewState.Running -> {
                        state.copy(
                            messageDialogState = MessageDialogState.Error(
                                title = strings.error,
                                message = strings.errorMessage(
                                    description = result.description,
                                    errorCode = result.errorCode
                                )
                            )
                        )
                    }
                    is ScannerViewState.VirusesDetected -> {
                        state.copy(
                            messageDialogState = MessageDialogState.Error(
                                title = strings.error,
                                message = strings.errorMessage(
                                    description = result.description,
                                    errorCode = result.errorCode
                                )
                            )
                        )
                    }
                    else -> state
                }
            }
        }
    }
}