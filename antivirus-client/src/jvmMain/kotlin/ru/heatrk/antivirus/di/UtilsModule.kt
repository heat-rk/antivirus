package ru.heatrk.antivirus.di

import org.kodein.di.DI
import org.kodein.di.bindProvider
import ru.heatrk.antivirus.utils.app_directory_provider.AppDirectoryProvider
import ru.heatrk.antivirus.utils.app_directory_provider.AppDirectoryProviderWindows

val utilsModule = DI.Module("utilsModule") {
    bindProvider<AppDirectoryProvider> {
        AppDirectoryProviderWindows()
    }
}

