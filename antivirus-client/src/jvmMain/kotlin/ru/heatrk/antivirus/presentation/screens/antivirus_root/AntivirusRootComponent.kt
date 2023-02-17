package ru.heatrk.tasktimetracker.presentation.ru.heatrk.antivirus.presentation.screens.antivirus_root

import com.arkivanov.decompose.ComponentContext
import org.kodein.di.DI
import ru.heatrk.antivirus.presentation.common.Component

class AntivirusRootComponent(
    componentContext: ComponentContext,
    di: DI
): Component(componentContext) {
    data class Args(
        val componentContext: ComponentContext
    )

    companion object {
        fun create(
            args: Args,
            di: DI
        ) = AntivirusRootComponent(
            componentContext = args.componentContext,
            di = di
        )
    }
}