package ru.heatrk.antivirus.domain.repositories

import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.flow.StateFlow
import ru.heatrk.antivirus.data.models.ApiMessage
import ru.heatrk.antivirus.domain.models.ScanState

interface MessagingRepository {
    suspend fun isServiceEnabled(): ApiMessage<Boolean>
    suspend fun startService(): ApiMessage<Unit>
    suspend fun stopService(): ApiMessage<Unit>
    suspend fun startScan(path: String): ApiMessage<Unit>
    suspend fun pauseScan(): ApiMessage<Unit>
    suspend fun resumeScan(): ApiMessage<Unit>
    fun scanState(scope: CoroutineScope) : StateFlow<ScanState>
}