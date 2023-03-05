package ru.heatrk.antivirus.presentation.screens

interface ServiceStatusListener {
    fun onStatusLoading()
    fun onStatusReceived(isEnabled: Boolean)
}