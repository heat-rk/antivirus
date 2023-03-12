package ru.heatrk.antivirus.data.repositories

import kotlinx.coroutines.CoroutineDispatcher
import kotlinx.coroutines.withContext
import ru.heatrk.antivirus.data.api.AntivirusApi
import ru.heatrk.antivirus.data.models.ApiMessage
import ru.heatrk.antivirus.data.models.MessageStatus
import ru.heatrk.antivirus.data.models.structs.MessageBodyStatusStruct
import ru.heatrk.antivirus.domain.repositories.MessagingRepository

class MessagingRepositoryImpl(
    private val antivirusApi: AntivirusApi,
    private val ioDispatcher: CoroutineDispatcher
): MessagingRepository {
    override suspend fun getStatus() = withContext(ioDispatcher) {
        val response = antivirusApi.getStatus()

        if (response is ApiMessage.Ok && response.body.status == MessageStatus.OK.id) {
            val body = MessageBodyStatusStruct.create(response.body.body)
            body.status == MessageBodyStatusStruct.OK
        } else {
            false
        }
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