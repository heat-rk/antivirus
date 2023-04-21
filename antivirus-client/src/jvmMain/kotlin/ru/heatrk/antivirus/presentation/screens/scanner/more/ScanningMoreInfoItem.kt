package ru.heatrk.antivirus.presentation.screens.scanner.more

import ru.heatrk.antivirus.domain.models.ScannerEntryStatus

data class ScanningMoreInfoItem(
    val path: String,
    val status: ScannerEntryStatus,
    val isChecked: Boolean,
    val isCheckBoxVisible: Boolean
)
