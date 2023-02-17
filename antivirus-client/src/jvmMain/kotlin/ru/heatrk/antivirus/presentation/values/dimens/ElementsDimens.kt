package ru.heatrk.antivirus.presentation.values.dimens

import androidx.compose.runtime.Composable
import androidx.compose.ui.platform.LocalDensity
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

object ElementsDimens {
    val AppDefaultWidth = 800.dp
    val AppDefaultHeight = 600.dp

    val BorderWidth = 1.dp

    val StartStopButtonSize = 50.dp

    val EntriesGroupCounterSize
        @Composable get() = with(LocalDensity.current) {
            32.sp.toDp()
        }
}