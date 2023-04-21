package ru.heatrk.antivirus.presentation.routing

import androidx.compose.runtime.Composable

interface Router {
    @Composable
    fun Container()

    fun navigateUp()

    fun openScanningMoreInfo()
}