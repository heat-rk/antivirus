package ru.heatrk.antivirus.presentation.screens.scanner

import com.arkivanov.decompose.ComponentContext
import ru.heatrk.antivirus.presentation.common.Component
import ru.heatrk.antivirus.presentation.screens.ServiceStatusListener

class ScannerComponent(
    componentContext: ComponentContext
) : Component(componentContext), ServiceStatusListener {
    override fun onStatusLoading() {
        TODO("Not yet implemented")
    }

    override fun onStatusReceived(isEnabled: Boolean) {
        TODO("Not yet implemented")
    }
}