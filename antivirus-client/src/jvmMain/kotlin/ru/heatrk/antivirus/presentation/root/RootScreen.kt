package ru.heatrk.antivirus.presentation.root

import androidx.compose.runtime.Composable
import ru.heatrk.antivirus.presentation.root.RootComponent

@Composable
fun RootScreen(component: RootComponent) {
    component.router.Container()
}