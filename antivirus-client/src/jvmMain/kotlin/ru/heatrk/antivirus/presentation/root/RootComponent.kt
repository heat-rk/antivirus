package ru.heatrk.antivirus.presentation.root

import com.arkivanov.decompose.ComponentContext
import ru.heatrk.antivirus.presentation.routing.Router

class RootComponent(
    componentContext: ComponentContext,
    val router: Router
): ComponentContext by componentContext