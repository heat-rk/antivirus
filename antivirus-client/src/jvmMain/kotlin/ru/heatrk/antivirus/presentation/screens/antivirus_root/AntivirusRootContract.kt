package ru.heatrk.antivirus.presentation.screens.antivirus_root

sealed interface AntivirusRootViewState {
    object Loading : AntivirusRootViewState

    object ServiceUnavailable : AntivirusRootViewState

    object Ok: AntivirusRootViewState
}