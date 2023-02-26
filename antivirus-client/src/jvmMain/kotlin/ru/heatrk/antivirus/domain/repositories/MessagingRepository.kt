package ru.heatrk.antivirus.domain.repositories

import kotlinx.coroutines.flow.Flow
import ru.heatrk.antivirus.domain.models.Message

interface MessagingRepository {
    val incomingMessages: Flow<Message>

    suspend fun sendMessage(message: Message)
}