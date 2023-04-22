package ru.heatrk.antivirus.di

import org.kodein.di.DI
import org.kodein.di.bindSingleton
import org.kodein.di.instance
import ru.heatrk.antivirus.data.api.AntivirusApi
import ru.heatrk.antivirus.data.api.AntivirusApiImpl

val apiModule = DI.Module("apiModule") {
    bindSingleton<AntivirusApi> {
        AntivirusApiImpl(
            appDirectoryProvider = instance()
        )
    }
}