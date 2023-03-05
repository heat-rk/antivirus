package ru.heatrk.antivirus.data.api

import kotlinx.coroutines.flow.SharedFlow
import ru.heatrk.antivirus.data.models.ApiMessage
import ru.heatrk.antivirus.data.models.structs.MessageStruct

interface AntivirusApi {
    val incomingMessages: SharedFlow<ApiMessage<MessageStruct>>

    suspend fun isServiceEnabled(): ApiMessage<Boolean>
    suspend fun send(struct: MessageStruct): ApiMessage<Unit>
    suspend fun startService(): ApiMessage<Unit>
    suspend fun stopService(): ApiMessage<Unit>
}