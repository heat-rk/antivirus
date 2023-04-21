package ru.heatrk.antivirus.domain.repositories

import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.flow.StateFlow
import ru.heatrk.antivirus.data.models.ApiMessage
import ru.heatrk.antivirus.domain.models.ScanState

interface MessagingRepository {
    suspend fun isProtectionEnabled(): ApiMessage<Boolean>
    suspend fun enableProtection(): ApiMessage<Unit>
    suspend fun disableProtection(): ApiMessage<Unit>
    suspend fun startScan(path: String): ApiMessage<Unit>
    suspend fun pauseScan(): ApiMessage<Unit>
    suspend fun resumeScan(): ApiMessage<Unit>
    suspend fun stopScan(): ApiMessage<Unit>
    fun scanState(scope: CoroutineScope) : StateFlow<ScanState>
}