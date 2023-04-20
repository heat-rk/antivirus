package ru.heatrk.antivirus.presentation.screens

interface ServiceStatusListener {
    fun onStatusReceived(status: ServiceStatus)
}