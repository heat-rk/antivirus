package ru.heatrk.antivirus.data.api

import kotlinx.coroutines.flow.SharedFlow
import ru.heatrk.antivirus.data.models.ApiMessage
import ru.heatrk.antivirus.data.models.structs.MessageStruct

interface AntivirusApi {
    val incomingMessages: SharedFlow<ApiMessage>
    suspend fun send(struct: MessageStruct): Boolean
}