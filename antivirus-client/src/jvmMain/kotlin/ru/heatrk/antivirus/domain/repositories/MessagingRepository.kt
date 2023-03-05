package ru.heatrk.antivirus.domain.repositories

import kotlinx.coroutines.flow.Flow
import ru.heatrk.antivirus.data.models.ApiMessage
import ru.heatrk.antivirus.domain.models.Message

interface MessagingRepository {
    val incomingMessages: Flow<Message>

    suspend fun isServiceEnabled(): ApiMessage<Boolean>
    suspend fun sendMessage(message: Message): ApiMessage<Unit>
    suspend fun startService(): ApiMessage<Unit>
    suspend fun stopService(): ApiMessage<Unit>
}