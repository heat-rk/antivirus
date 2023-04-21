package ru.heatrk.antivirus.di

import org.kodein.di.DI
import org.kodein.di.bindFactory
import org.kodein.di.instance
import ru.heatrk.antivirus.presentation.screens.antivirus_root.AntivirusRootComponent
import ru.heatrk.antivirus.presentation.screens.scanner.more.ScanningMoreInfoComponent

val componentsModule = DI.Module("componentsModule") {
    bindFactory<AntivirusRootComponent.Args, AntivirusRootComponent> { args ->
        AntivirusRootComponent.create(
            args = args,
            di = di,
            messagingRepository = instance(),
            defaultDispatcher = defaultDispatcherInstance()
        )
    }

    bindFactory<ScanningMoreInfoComponent.Args, ScanningMoreInfoComponent> { args ->
        ScanningMoreInfoComponent.create(
            args = args,
            messagingRepository = instance()
        )
    }
}