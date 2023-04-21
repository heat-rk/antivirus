package ru.heatrk.antivirus.presentation.screens

interface ProtectionListener {
    fun onStatusReceived(status: ProtectionStatus)
}