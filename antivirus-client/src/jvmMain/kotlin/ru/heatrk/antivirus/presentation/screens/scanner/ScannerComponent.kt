package ru.heatrk.antivirus.presentation.screens.scanner

import com.arkivanov.decompose.ComponentContext
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch
import ru.heatrk.antivirus.domain.repositories.MessagingRepository
import ru.heatrk.antivirus.presentation.common.Component
import ru.heatrk.antivirus.presentation.screens.ServiceStatusListener

class ScannerComponent(
    componentContext: ComponentContext,
    private val messagingRepository: MessagingRepository
) : Component(componentContext), ServiceStatusListener {

    private val _state = MutableStateFlow<ScannerViewState>(ScannerViewState.Loading)
    val state = _state.asStateFlow()

    override fun onStatusLoading() {
        _state.value = ScannerViewState.Loading
    }

    override fun onStatusReceived(isEnabled: Boolean) {
        loadIdleState(isEnabled = isEnabled)
    }

    fun onIntent(intent: ScannerIntent) {
        when (intent) {
            is ScannerIntent.More -> Unit
            is ScannerIntent.Pause -> Unit
            is ScannerIntent.Resume -> Unit
            is ScannerIntent.Start -> applyStartScanIntent(intent)
            is ScannerIntent.Stop -> Unit
            is ScannerIntent.ShowFileSelectionDialog -> applySelectFileIntent(intent)
            is ScannerIntent.HideFileSelectionDialog -> applyHideFileSelectionDialogIntent(intent)
        }
    }

    private fun applySelectFileIntent(intent: ScannerIntent.ShowFileSelectionDialog) {
        when (val state = state.value) {
            is ScannerViewState.Idle -> {
                _state.update { state.copy(isFileDialogVisible = true) }
            }
            is ScannerViewState.VirusesDetected -> {
                _state.update { state.copy(isFileDialogVisible = true) }
            }
            else -> Unit
        }
    }

    private fun applyHideFileSelectionDialogIntent(
        intent: ScannerIntent.HideFileSelectionDialog
    ) {
        when (val state = state.value) {
            is ScannerViewState.Idle -> {
                _state.update { state.copy(isFileDialogVisible = false) }
            }
            is ScannerViewState.VirusesDetected -> {
                _state.update { state.copy(isFileDialogVisible = false) }
            }
            else -> Unit
        }
    }

    private fun applyStartScanIntent(intent: ScannerIntent.Start) {

    }

    private fun loadIdleState(isEnabled: Boolean) = componentScope.launch {
        if (!isEnabled) {
            _state.value = ScannerViewState.Idle(isEnabled = false)
            return@launch
        }

        val lastScanResults = messagingRepository.getLastScan()

        if (lastScanResults.isNotEmpty()) {
            _state.value = ScannerViewState.VirusesDetected(
                virusesDetected = lastScanResults.size
            )
        } else {
            _state.value = ScannerViewState.Idle(isEnabled = true)
        }
    }
}