package ru.heatrk.antivirus.data.api

import kotlinx.coroutines.flow.SharedFlow
import kotlinx.coroutines.flow.StateFlow
import ru.heatrk.antivirus.data.models.ApiMessage
import ru.heatrk.antivirus.data.models.ScannerCacheData
import ru.heatrk.antivirus.data.models.structs.MessageStruct

interface AntivirusApi {
    val incomingMessages: SharedFlow<ApiMessage<MessageStruct>>
    val scannerCacheData: StateFlow<ScannerCacheData?>

    suspend fun isServiceEnabled(): ApiMessage<Boolean>
    suspend fun startService(): ApiMessage<Unit>
    suspend fun stopService(): ApiMessage<Unit>
    suspend fun startScan(path: String): ApiMessage<Unit>
    suspend fun pauseScan(): ApiMessage<Unit>
    suspend fun stopScan(): ApiMessage<Unit>
    suspend fun resumeScan(): ApiMessage<Unit>
}