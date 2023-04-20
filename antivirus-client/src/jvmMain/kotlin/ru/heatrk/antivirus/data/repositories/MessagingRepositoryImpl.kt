package ru.heatrk.antivirus.data.repositories

import kotlinx.coroutines.CoroutineDispatcher
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.FlowPreview
import kotlinx.coroutines.flow.*
import kotlinx.coroutines.withContext
import ru.heatrk.antivirus.data.api.AntivirusApi
import ru.heatrk.antivirus.data.models.ScannerCacheData
import ru.heatrk.antivirus.domain.models.ScanState
import ru.heatrk.antivirus.domain.models.ScannerEntryStatus
import ru.heatrk.antivirus.domain.models.ScannerStatus
import ru.heatrk.antivirus.domain.repositories.MessagingRepository

class MessagingRepositoryImpl(
    private val antivirusApi: AntivirusApi,
    private val ioDispatcher: CoroutineDispatcher
): MessagingRepository {
    override suspend fun isServiceEnabled() = withContext(ioDispatcher) {
        antivirusApi.isServiceEnabled()
    }

    override suspend fun startService() = withContext(ioDispatcher) {
        antivirusApi.startService()
    }

    override suspend fun stopService() = withContext(ioDispatcher) {
        antivirusApi.stopService()
    }

    override suspend fun startScan(path: String) = withContext(ioDispatcher) {
        antivirusApi.startScan(path)
    }

    override suspend fun pauseScan() = withContext(ioDispatcher) {
        antivirusApi.pauseScan()
    }

    override suspend fun resumeScan() = withContext(ioDispatcher) {
        antivirusApi.resumeScan()
    }

    @OptIn(FlowPreview::class)
    override fun scanState(scope: CoroutineScope): StateFlow<ScanState> =
        antivirusApi.scannerCacheData
            .map(::scanState)
            .debounce(500)
            .stateIn(
                scope = scope,
                started = SharingStarted.Eagerly,
                initialValue = scanState(antivirusApi.scannerCacheData.value)
            )

    private fun scanState(data: ScannerCacheData?) = when {
        data == null -> {
            ScanState.Empty
        }

        data.scannerStatus == ScannerStatus.SCANNING.id ||
        data.scannerStatus == ScannerStatus.PAUSED.id -> {
            ScanState.Running(
                entries = data.entries.map { entry ->
                    entry.first to ScannerEntryStatus.byId(entry.second)
                },
                isPaused = data.scannerStatus == ScannerStatus.PAUSED.id
            )
        }

        else -> {
            val virusesEntries = data.entries
                .filter { it.second == ScannerEntryStatus.SCANNED_INFECTED.id }

            if (virusesEntries.isNotEmpty()) {
                ScanState.VirusesDetected(
                    viruses = virusesEntries.map(Pair<String, Byte>::first)
                )
            } else {
                ScanState.Empty
            }
        }
    }
}