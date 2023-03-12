package ru.heatrk.antivirus.domain.repositories

import ru.heatrk.antivirus.data.models.ApiMessage

interface MessagingRepository {
    suspend fun isServiceEnabled(): ApiMessage<Boolean>
    suspend fun startService(): ApiMessage<Unit>
    suspend fun stopService(): ApiMessage<Unit>

    suspend fun getStatus(): Boolean
}