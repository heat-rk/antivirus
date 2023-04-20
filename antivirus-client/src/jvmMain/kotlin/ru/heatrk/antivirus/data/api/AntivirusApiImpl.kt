package ru.heatrk.antivirus.data.api

import com.sun.jna.Native
import com.sun.jna.Pointer
import com.sun.jna.platform.win32.Advapi32
import com.sun.jna.platform.win32.Kernel32
import com.sun.jna.platform.win32.WinBase
import com.sun.jna.platform.win32.WinBase.OVERLAPPED
import com.sun.jna.platform.win32.WinNT.*
import com.sun.jna.platform.win32.Winsvc
import com.sun.jna.ptr.IntByReference
import kotlinx.coroutines.*
import kotlinx.coroutines.flow.MutableSharedFlow
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asSharedFlow
import kotlinx.coroutines.flow.asStateFlow
import ru.heatrk.antivirus.data.models.ApiMessage
import ru.heatrk.antivirus.data.models.MessageMethod
import ru.heatrk.antivirus.data.models.MessageStatus
import ru.heatrk.antivirus.data.models.ScannerCacheData
import ru.heatrk.antivirus.data.models.structs.MessageBodyScanStartStruct
import ru.heatrk.antivirus.data.models.structs.MessageStruct
import ru.heatrk.antivirus.utils.DynamicByteBuffer
import ru.heatrk.antivirus.utils.app_directory_provider.AppDirectoryProvider
import java.io.File
import java.nio.ByteBuffer

class AntivirusApiImpl(
    appDirectoryProvider: AppDirectoryProvider
) : AntivirusApi {

    private val kernel32 = Kernel32.INSTANCE
    private val advapi32 = Advapi32.INSTANCE

    private val coroutineScope = CoroutineScope(Dispatchers.IO + SupervisorJob())

    private var pipeListeningJob: Job? = null

    private val _incomingMessages = MutableSharedFlow<ApiMessage<MessageStruct>>()
    override val incomingMessages = _incomingMessages.asSharedFlow()

    private val _scannerCacheData = MutableStateFlow<ScannerCacheData?>(null)
    override val scannerCacheData = _scannerCacheData.asStateFlow()

    private var outputPipe: HANDLE = INVALID_HANDLE_VALUE
    private var inputPipe: HANDLE = INVALID_HANDLE_VALUE

    private val appDirectory = appDirectoryProvider.providePath()

    init {
        coroutineScope.launch {
            updateScannerCacheData()

            val file = kernel32.CreateFile(
                appDirectory,
                FILE_LIST_DIRECTORY,
                FILE_SHARE_READ,
                WinBase.SECURITY_ATTRIBUTES(),
                OPEN_EXISTING,
                FILE_FLAG_BACKUP_SEMANTICS,
                HANDLE(Pointer.NULL)
            )

            if (file == INVALID_HANDLE_VALUE) {
                println("Cant CreateFile for changes listening")
                return@launch
            }

            while (isActive) {
                var fileNotifyInformation: FILE_NOTIFY_INFORMATION?
                        = FILE_NOTIFY_INFORMATION(FILE_NOTIFY_INFORMATION_SIZE)

                val isReadChangesSuccess = kernel32.ReadDirectoryChangesW(
                    file,
                    fileNotifyInformation,
                    FILE_NOTIFY_INFORMATION_SIZE,
                    true,
                    FILE_NOTIFY_CHANGE_LAST_WRITE or FILE_NOTIFY_CHANGE_FILE_NAME,
                    IntByReference(),
                    OVERLAPPED()
                ) { _, _, _ -> /* Empty */ }

                if (!isReadChangesSuccess) {
                    println("Error while read directory changes (code ${kernel32.GetLastError()})")
                    delay(1000)
                    continue
                }

                while (fileNotifyInformation != null) {
                    val isActionValid =
                        fileNotifyInformation.Action == FILE_ACTION_MODIFIED ||
                            fileNotifyInformation.Action == FILE_ACTION_REMOVED ||
                            fileNotifyInformation.Action == FILE_ACTION_ADDED

                    if (!isActionValid) {
                        fileNotifyInformation = fileNotifyInformation.next()
                        continue
                    }

                    val isFileValid =
                        String(fileNotifyInformation.FileName) == SCANNER_DATA_FILE_NAME

                    if (!isFileValid) {
                        fileNotifyInformation = fileNotifyInformation.next()
                        continue
                    }

                    updateScannerCacheData()

                    fileNotifyInformation = null
                }
            }

        }
    }

    override suspend fun startScan(path: String): ApiMessage<Unit> {
        val requestMessage = MessageStruct().apply {
            MessageMethod.SCAN_START.id.copyInto(this.method)
            status = MessageStatus.REQUEST.id
            val bodyBytesBuffer = DynamicByteBuffer()
            MessageBodyScanStartStruct().apply {
                this.path = Native.toByteArray(path)
            }.write(bodyBytesBuffer)
            bodyBytesBuffer.position(0)
            bodySize = bodyBytesBuffer.capacity()
            body = ByteArray(bodySize)
            bodyBytesBuffer.get(body)
        }

        return send(requestMessage)
    }

    override suspend fun pauseScan(): ApiMessage<Unit> {
        val requestMessage = MessageStruct().apply {
            MessageMethod.SCAN_PAUSE.id.copyInto(this.method)
            status = MessageStatus.REQUEST.id
        }

        return send(requestMessage)
    }

    override suspend fun resumeScan(): ApiMessage<Unit> {
        val requestMessage = MessageStruct().apply {
            MessageMethod.SCAN_RESUME.id.copyInto(this.method)
            status = MessageStatus.REQUEST.id
        }

        return send(requestMessage)
    }

    override suspend fun isServiceEnabled(): ApiMessage<Boolean> {
        val scManager = advapi32.OpenSCManager(null, null, Winsvc.SC_MANAGER_CONNECT)
            ?: return ApiMessage.Fail(
                description = "OpenSCManager [Service enabled check]",
                errorCode = kernel32.GetLastError()
            )

        val serviceStatusProcess = Winsvc.SERVICE_STATUS_PROCESS()

        val service: Winsvc.SC_HANDLE? = advapi32.OpenService(
            scManager,
            ANTIVIRUS_SERVICE_NAME,
            Winsvc.SERVICE_QUERY_STATUS
        )

        if (service == null) {
            advapi32.CloseServiceHandle(scManager)

            return ApiMessage.Fail(
                description = "OpenService [Service enabled check]",
                errorCode = kernel32.GetLastError()
            )
        }

        if (
            !advapi32.QueryServiceStatusEx(
                service,
                Winsvc.SC_STATUS_TYPE.SC_STATUS_PROCESS_INFO,
                serviceStatusProcess,
                serviceStatusProcess.size(),
                IntByReference()
            )
        ) {
            advapi32.CloseServiceHandle(scManager)
            advapi32.CloseServiceHandle(service)

            return ApiMessage.Fail(
                description = "QueryServiceStatusEx [Service enabled check]",
                errorCode = kernel32.GetLastError()
            )
        }

        var startTickCount = kernel32.GetTickCount()
        var oldCheckPoint = serviceStatusProcess.dwCheckPoint

        while (serviceStatusProcess.dwCurrentState == Winsvc.SERVICE_STOP_PENDING) {
            var dwWaitTime = serviceStatusProcess.dwWaitHint / 10L

            if (dwWaitTime < 1000) {
                dwWaitTime = 1000
            } else if ( dwWaitTime > 10000 ) {
                dwWaitTime = 10000
            }

            delay(dwWaitTime)

            if (
                !advapi32.QueryServiceStatusEx(
                    service,
                    Winsvc.SC_STATUS_TYPE.SC_STATUS_PROCESS_INFO,
                    serviceStatusProcess,
                    serviceStatusProcess.size(),
                    IntByReference()
                )
            ) {
                advapi32.CloseServiceHandle(scManager)
                advapi32.CloseServiceHandle(service)

                return ApiMessage.Fail(
                    description = "QueryServiceStatusEx [Service enabled check]",
                    errorCode = kernel32.GetLastError()
                )
            }

            if (serviceStatusProcess.dwCheckPoint > oldCheckPoint) {
                startTickCount = kernel32.GetTickCount()
                oldCheckPoint = serviceStatusProcess.dwCheckPoint
            } else if (kernel32.GetTickCount() - startTickCount > serviceStatusProcess.dwWaitHint) {
                advapi32.CloseServiceHandle(scManager)
                advapi32.CloseServiceHandle(service)

                return ApiMessage.Fail(
                    description = "Service enabled check timeout [Service enabled check]"
                )
            }
        }

        val isEnabled = serviceStatusProcess.dwCurrentState == Winsvc.SERVICE_RUNNING

        advapi32.CloseServiceHandle(scManager)
        advapi32.CloseServiceHandle(service)

        return if (isEnabled) {
            val pipeConnectionResult = connectPipes()

            if (pipeConnectionResult is ApiMessage.Ok) {
                ApiMessage.Ok(true)
            } else {
                ApiMessage.Fail(description = "Pipes connection failed [Service enabled check]")
            }
        } else {
            ApiMessage.Ok(false)
        }
    }

    override suspend fun startService(): ApiMessage<Unit> {
        reset()

        val windowsServiceStartResult = startWindowsService()

        if (windowsServiceStartResult is ApiMessage.Fail) {
            return windowsServiceStartResult
        }

        return connectPipes()
    }

    override suspend fun stopService(): ApiMessage<Unit> {
        reset()
        return stopWindowsService()
    }

    private fun reset() {
        pipeListeningJob?.cancel()
        pipeListeningJob = null
        inputPipe = INVALID_HANDLE_VALUE
        outputPipe = INVALID_HANDLE_VALUE
    }

    private fun connectPipes(): ApiMessage<Unit> {
        if (inputPipe != INVALID_HANDLE_VALUE && outputPipe != INVALID_HANDLE_VALUE) {
            return ApiMessage.Ok(Unit)
        }

        outputPipe = kernel32.CreateFile(
            PIPE_SERVICE_INPUT_PATH,
            GENERIC_WRITE,
            0,
            WinBase.SECURITY_ATTRIBUTES(),
            OPEN_EXISTING,
            0,
            HANDLE(Pointer.NULL)
        )

        if (outputPipe == INVALID_HANDLE_VALUE) {
            return ApiMessage.Fail(
                description = "Output pipe connecting",
                errorCode = kernel32.GetLastError()
            )
        }

        inputPipe = kernel32.CreateFile(
            PIPE_SERVICE_OUTPUT_PATH,
            GENERIC_READ,
            0,
            WinBase.SECURITY_ATTRIBUTES(),
            OPEN_EXISTING,
            0,
            HANDLE(Pointer.NULL)
        )

        if (inputPipe == INVALID_HANDLE_VALUE) {
            return ApiMessage.Fail(
                description = "Input pipe connecting",
                errorCode = kernel32.GetLastError()
            )
        }

        pipeListeningJob = coroutineScope.launch {
            val buffer = ByteArray(PIPE_BUFFSIZE)

            while (inputPipe != INVALID_HANDLE_VALUE) {
                val isSuccess = kernel32.ReadFile(
                    inputPipe,
                    buffer,
                    PIPE_BUFFSIZE,
                    IntByReference(),
                    OVERLAPPED()
                )

                if (isSuccess) {
                    println("Message bytes:\n${buffer.contentToString()}\n-------------")

                    _incomingMessages.emit(ApiMessage.Ok(
                        MessageStruct.create(buffer).also {
                            println("Mapped message:\n${it}\n------------")
                        }
                    ))
                } else {
                    _incomingMessages.emit(ApiMessage.Fail(
                        description = "Input pipe reading",
                        errorCode = kernel32.GetLastError()
                    ))
                }
            }
        }

        return ApiMessage.Ok(Unit)
    }

    private fun updateScannerCacheData() {
        val scannerDataFilePath = appDirectory + SCANNER_DATA_FILE_NAME
        val scannerDataFile = File(scannerDataFilePath)

        if (!scannerDataFile.exists()) {
            _scannerCacheData.value = null
            return
        }

        val scannerDataFileBytes = scannerDataFile.readBytes()

        if (scannerDataFileBytes.isEmpty()) {
            _scannerCacheData.value = null
            return
        }

        val byteBuffer = ByteBuffer.wrap(scannerDataFileBytes)

        val scannerStatus = byteBuffer.get()

        val entries = mutableListOf<Pair<String, Byte>>()

        while (byteBuffer.hasRemaining()) {
            val entryStatus = byteBuffer.get()
            val entryLength = byteBuffer.int
            val entry = ByteArray(entryLength)
            byteBuffer.get(entry)
            entries.add(Native.toString(entry) to entryStatus)
        }

        _scannerCacheData.value = ScannerCacheData(
            scannerStatus = scannerStatus,
            entries = entries
        )
    }

    private fun send(struct: MessageStruct): ApiMessage<Unit> {
        val bytes = ByteArray(PIPE_BUFFSIZE).also { struct.write(it) }

        return if (
            kernel32.WriteFile(
                outputPipe,
                bytes,
                PIPE_BUFFSIZE,
                IntByReference(),
                null
            )
        ) {
            ApiMessage.Ok(Unit)
        } else {
            ApiMessage.Fail(
                description = "Output pipe writing",
                errorCode = kernel32.GetLastError()
            )
        }
    }

    private suspend fun startWindowsService(): ApiMessage<Unit> {
        val scManager = advapi32.OpenSCManager(null, null, Winsvc.SC_MANAGER_CONNECT)
            ?: return ApiMessage.Fail(
                description = "OpenSCManager [Start service]",
                errorCode = kernel32.GetLastError()
            )

        val serviceStatusProcess = Winsvc.SERVICE_STATUS_PROCESS()

        val service: Winsvc.SC_HANDLE? = advapi32.OpenService(
            scManager,
            ANTIVIRUS_SERVICE_NAME,
            Winsvc.SERVICE_START or Winsvc.SERVICE_QUERY_STATUS
        )

        if (service == null) {
            advapi32.CloseServiceHandle(scManager)

            return ApiMessage.Fail(
                description = "OpenService [Start service]",
                errorCode = kernel32.GetLastError()
            )
        }

        if (
            !advapi32.QueryServiceStatusEx(
                service,
                Winsvc.SC_STATUS_TYPE.SC_STATUS_PROCESS_INFO,
                serviceStatusProcess,
                serviceStatusProcess.size(),
                IntByReference()
            )
        ) {
            advapi32.CloseServiceHandle(scManager)
            advapi32.CloseServiceHandle(service)

            return ApiMessage.Fail(
                description = "QueryServiceStatusEx [Start service]",
                errorCode = kernel32.GetLastError()
            )
        }

        if (serviceStatusProcess.dwCurrentState != Winsvc.SERVICE_STOPPED &&
            serviceStatusProcess.dwCurrentState != Winsvc.SERVICE_STOP_PENDING) {

            advapi32.CloseServiceHandle(scManager)
            advapi32.CloseServiceHandle(service)

            return ApiMessage.Ok(Unit)
        }

        var startTickCount = kernel32.GetTickCount()
        var oldCheckPoint = serviceStatusProcess.dwCheckPoint

        while (serviceStatusProcess.dwCurrentState == Winsvc.SERVICE_STOP_PENDING) {
            var dwWaitTime = serviceStatusProcess.dwWaitHint / 10L

            if (dwWaitTime < 1000) {
                dwWaitTime = 1000
            } else if ( dwWaitTime > 10000 ) {
                dwWaitTime = 10000
            }

            delay(dwWaitTime)

            if (
                !advapi32.QueryServiceStatusEx(
                    service,
                    Winsvc.SC_STATUS_TYPE.SC_STATUS_PROCESS_INFO,
                    serviceStatusProcess,
                    serviceStatusProcess.size(),
                    IntByReference()
                )
            ) {
                advapi32.CloseServiceHandle(scManager)
                advapi32.CloseServiceHandle(service)

                return ApiMessage.Fail(
                    description = "QueryServiceStatusEx [Start service]",
                    errorCode = kernel32.GetLastError()
                )
            }

            if (serviceStatusProcess.dwCheckPoint > oldCheckPoint) {
                startTickCount = kernel32.GetTickCount()
                oldCheckPoint = serviceStatusProcess.dwCheckPoint
            } else if (kernel32.GetTickCount() - startTickCount > serviceStatusProcess.dwWaitHint) {

                advapi32.CloseServiceHandle(scManager)
                advapi32.CloseServiceHandle(service)

                return ApiMessage.Fail(
                    description = "Service stop timeout [Start service]"
                )
            }
        }

        if (!advapi32.StartService(service, 0, null)) {
            advapi32.CloseServiceHandle(scManager)
            advapi32.CloseServiceHandle(service)

            return ApiMessage.Fail(
                description = "StartService",
                errorCode = kernel32.GetLastError()
            )
        }

        if (
            !advapi32.QueryServiceStatusEx(
                service,
                Winsvc.SC_STATUS_TYPE.SC_STATUS_PROCESS_INFO,
                serviceStatusProcess,
                serviceStatusProcess.size(),
                IntByReference()
            )
        ) {
            advapi32.CloseServiceHandle(scManager)
            advapi32.CloseServiceHandle(service)

            return ApiMessage.Fail(
                description = "QueryServiceStatusEx [Start service]",
                errorCode = kernel32.GetLastError()
            )
        }

        while (serviceStatusProcess.dwCurrentState == Winsvc.SERVICE_START_PENDING) {
            var dwWaitTime = serviceStatusProcess.dwWaitHint / 10L

            if (dwWaitTime < 1000) {
                dwWaitTime = 1000
            } else if ( dwWaitTime > 10000 ) {
                dwWaitTime = 10000
            }

            delay(dwWaitTime)

            if (
                !advapi32.QueryServiceStatusEx(
                    service,
                    Winsvc.SC_STATUS_TYPE.SC_STATUS_PROCESS_INFO,
                    serviceStatusProcess,
                    serviceStatusProcess.size(),
                    IntByReference()
                )
            ) {
                advapi32.CloseServiceHandle(scManager)
                advapi32.CloseServiceHandle(service)

                return ApiMessage.Fail(
                    description = "QueryServiceStatusEx [Start service]",
                    errorCode = kernel32.GetLastError()
                )
            }

            if (serviceStatusProcess.dwCheckPoint > oldCheckPoint) {
                startTickCount = kernel32.GetTickCount()
                oldCheckPoint = serviceStatusProcess.dwCheckPoint
            } else if (kernel32.GetTickCount() - startTickCount > serviceStatusProcess.dwWaitHint) {
                break
            }
        }

        val isRunning = serviceStatusProcess.dwCurrentState == Winsvc.SERVICE_RUNNING

        advapi32.CloseServiceHandle(scManager)
        advapi32.CloseServiceHandle(service)

        return if (isRunning) {
            ApiMessage.Ok(Unit)
        } else {
            ApiMessage.Fail(description = "Unknown")
        }
    }

    private suspend fun stopWindowsService(): ApiMessage<Unit> {
        val startTime = kernel32.GetTickCount()

        val scManager = advapi32.OpenSCManager(null, null, Winsvc.SC_MANAGER_CONNECT)
            ?: return ApiMessage.Fail(
                description = "OpenSCManager [Stop service]",
                errorCode = kernel32.GetLastError()
            )

        val serviceStatusProcess = Winsvc.SERVICE_STATUS_PROCESS()

        val service: Winsvc.SC_HANDLE? = advapi32.OpenService(
            scManager,
            ANTIVIRUS_SERVICE_NAME,
            Winsvc.SERVICE_STOP or Winsvc.SERVICE_QUERY_STATUS
        )

        if (service == null) {
            advapi32.CloseServiceHandle(scManager)

            return ApiMessage.Fail(
                description = "OpenService [Stop service]",
                errorCode = kernel32.GetLastError()
            )
        }

        if (
            !advapi32.QueryServiceStatusEx(
                service,
                Winsvc.SC_STATUS_TYPE.SC_STATUS_PROCESS_INFO,
                serviceStatusProcess,
                serviceStatusProcess.size(),
                IntByReference()
            )
        ) {
            advapi32.CloseServiceHandle(scManager)
            advapi32.CloseServiceHandle(service)

            return ApiMessage.Fail(
                description = "QueryServiceStatusEx [Stop service]",
                errorCode = kernel32.GetLastError()
            )
        }

        if (serviceStatusProcess.dwCurrentState == Winsvc.SERVICE_STOPPED) {
            advapi32.CloseServiceHandle(scManager)
            advapi32.CloseServiceHandle(service)

            return ApiMessage.Ok(Unit)
        }

        while (serviceStatusProcess.dwCurrentState == Winsvc.SERVICE_STOP_PENDING) {
            var dwWaitTime = serviceStatusProcess.dwWaitHint / 10L

            if (dwWaitTime < 1000) {
                dwWaitTime = 1000
            } else if ( dwWaitTime > 10000 ) {
                dwWaitTime = 10000
            }

            delay(dwWaitTime)

            if (
                !advapi32.QueryServiceStatusEx(
                    service,
                    Winsvc.SC_STATUS_TYPE.SC_STATUS_PROCESS_INFO,
                    serviceStatusProcess,
                    serviceStatusProcess.size(),
                    IntByReference()
                )
            ) {
                advapi32.CloseServiceHandle(scManager)
                advapi32.CloseServiceHandle(service)

                return ApiMessage.Fail(
                    description = "QueryServiceStatusEx [Stop service]",
                    errorCode = kernel32.GetLastError()
                )
            }

            if (serviceStatusProcess.dwCurrentState == Winsvc.SERVICE_STOPPED) {
                advapi32.CloseServiceHandle(scManager)
                advapi32.CloseServiceHandle(service)
                return ApiMessage.Ok(Unit)
            }

            if (kernel32.GetTickCount() - startTime > WINDOWS_SERVICE_STOP_TIMEOUT) {
                advapi32.CloseServiceHandle(scManager)
                advapi32.CloseServiceHandle(service)

                return ApiMessage.Fail(description = "Stop timeout [Stop service]")
            }
        }

        if (!advapi32.ControlService(service, Winsvc.SERVICE_CONTROL_STOP, Winsvc.SERVICE_STATUS())) {
            advapi32.CloseServiceHandle(scManager)
            advapi32.CloseServiceHandle(service)

            return ApiMessage.Fail(
                description = "ControlService [Stop service]",
                errorCode = kernel32.GetLastError()
            )
        }

        while (serviceStatusProcess.dwCurrentState != Winsvc.SERVICE_STOPPED) {
            delay(serviceStatusProcess.dwWaitHint.toLong())

            if (
                !advapi32.QueryServiceStatusEx(
                    service,
                    Winsvc.SC_STATUS_TYPE.SC_STATUS_PROCESS_INFO,
                    serviceStatusProcess,
                    serviceStatusProcess.size(),
                    IntByReference()
                )
            ) {
                advapi32.CloseServiceHandle(scManager)
                advapi32.CloseServiceHandle(service)

                return ApiMessage.Fail(
                    description = "QueryServiceStatusEx [Stop service]",
                    errorCode = kernel32.GetLastError()
                )
            }

            if (serviceStatusProcess.dwCurrentState == Winsvc.SERVICE_STOPPED) {
                break
            }

            if (kernel32.GetTickCount() - startTime > WINDOWS_SERVICE_STOP_TIMEOUT) {
                advapi32.CloseServiceHandle(scManager)
                advapi32.CloseServiceHandle(service)
                return ApiMessage.Fail(description = "Stop timeout [Stop service]")
            }
        }

        return if (serviceStatusProcess.dwCurrentState == Winsvc.SERVICE_STOPPED) {
            ApiMessage.Ok(Unit)
        } else {
            ApiMessage.Fail(description = "Unknown")
        }
    }

    companion object {
        private const val PIPE_SERVICE_INPUT_PATH = "\\\\.\\pipe\\antivirus-pipe-service-input"
        private const val PIPE_SERVICE_OUTPUT_PATH = "\\\\.\\pipe\\antivirus-pipe-service-output"
        private const val PIPE_BUFFSIZE = 4096
        private const val ANTIVIRUS_SERVICE_NAME = "AntivirusService"
        private const val WINDOWS_SERVICE_STOP_TIMEOUT = 30000
        private const val REQUEST_TIMEOUT = 5000L
        private const val RESPONSE_TIMEOUT = 5000L
        private const val FILE_NOTIFY_INFORMATION_SIZE = 1024
        private const val SCANNER_DATA_FILE_NAME = "scanner-data"
    }
}