package ru.heatrk.antivirus.data.api

import ru.heatrk.antivirus.data.models.ApiMessage
import ru.heatrk.antivirus.data.models.structs.MessageStruct

interface AntivirusApi {
    suspend fun isServiceEnabled(): ApiMessage<Boolean>
    suspend fun startService(): ApiMessage<Unit>
    suspend fun stopService(): ApiMessage<Unit>

    suspend fun getStatus(): ApiMessage<MessageStruct>
}