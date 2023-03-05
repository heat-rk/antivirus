package ru.heatrk.antivirus.data.repositories

import kotlinx.coroutines.CoroutineDispatcher
import kotlinx.coroutines.flow.map
import kotlinx.coroutines.withContext
import ru.heatrk.antivirus.data.api.AntivirusApi
import ru.heatrk.antivirus.data.models.ApiMessage
import ru.heatrk.antivirus.domain.models.Message
import ru.heatrk.antivirus.domain.repositories.MessagingRepository
import ru.heatrk.antivirus.mappers.incoming.toDomain
import ru.heatrk.antivirus.mappers.outgoing.toStruct

class MessagingRepositoryImpl(
    private val antivirusApi: AntivirusApi,
    private val ioDispatcher: CoroutineDispatcher
): MessagingRepository {
    override val incomingMessages get() = antivirusApi.incomingMessages
        .map { it.toDomain() }

    override suspend fun sendMessage(message: Message) = withContext(ioDispatcher) {
        antivirusApi.send(message.toStruct())
    }

    override suspend fun isServiceEnabled() = withContext(ioDispatcher) {
        antivirusApi.isServiceEnabled()
    }

    override suspend fun startService() = withContext(ioDispatcher) {
        antivirusApi.startService()
    }

    override suspend fun stopService() = withContext(ioDispatcher) {
        antivirusApi.stopService()
    }
}