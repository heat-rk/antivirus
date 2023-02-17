package ru.heatrk.antivirus.di

import org.kodein.di.DI
import org.kodein.di.bindFactory
import ru.heatrk.tasktimetracker.presentation.ru.heatrk.antivirus.presentation.screens.antivirus_root.AntivirusRootComponent

val componentsModule = DI.Module("componentsModule") {
    bindFactory<AntivirusRootComponent.Args, AntivirusRootComponent> { args ->
        AntivirusRootComponent.create(
            args = args,
            di = di
        )
    }
}