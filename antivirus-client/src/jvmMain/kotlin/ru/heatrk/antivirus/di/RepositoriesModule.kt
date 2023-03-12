package ru.heatrk.antivirus.di

import org.kodein.di.DI
import org.kodein.di.bindProvider
import org.kodein.di.instance
import ru.heatrk.antivirus.data.repositories.MessagingRepositoryImpl
import ru.heatrk.antivirus.domain.repositories.MessagingRepository

val repositoriesModule = DI.Module("repositories") {
    bindProvider<MessagingRepository> {
        MessagingRepositoryImpl(
            antivirusApi = instance(),
            ioDispatcher = ioDispatcherInstance()
        )
    }
}