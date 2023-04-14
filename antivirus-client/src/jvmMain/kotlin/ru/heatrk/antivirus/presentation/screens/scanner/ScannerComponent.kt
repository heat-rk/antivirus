package ru.heatrk.antivirus.presentation.screens.scanner

import com.arkivanov.decompose.ComponentContext
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.update
import ru.heatrk.antivirus.presentation.common.Component
import ru.heatrk.antivirus.presentation.screens.ServiceStatusListener

class ScannerComponent(
    componentContext: ComponentContext
) : Component(componentContext), ServiceStatusListener {

    private val _state = MutableStateFlow<ScannerViewState>(ScannerViewState.Idle(isEnabled = false))
    val state = _state.asStateFlow()

    override fun onStatusLoading() {
        _state.value = ScannerViewState.Idle(isEnabled = false)
    }

    override fun onStatusReceived(isEnabled: Boolean) {
        _state.value = ScannerViewState.Idle(isEnabled = isEnabled)
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
        println(intent.path)
    }
}