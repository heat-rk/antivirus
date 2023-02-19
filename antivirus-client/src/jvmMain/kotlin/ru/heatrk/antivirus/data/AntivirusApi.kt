package ru.heatrk.antivirus.data

import kotlinx.coroutines.flow.Flow
import ru.heatrk.antivirus.data.models.ApiMessage

interface AntivirusApi {
    suspend fun send(byteArray: ByteArray): Boolean
    suspend fun incomingMessages(): Flow<ApiMessage<ByteArray>>
}