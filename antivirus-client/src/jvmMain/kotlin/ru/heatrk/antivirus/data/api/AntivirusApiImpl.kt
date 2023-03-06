package ru.heatrk.antivirus.data.api

import com.sun.jna.Pointer
import com.sun.jna.platform.win32.*
import com.sun.jna.platform.win32.WinNT.HANDLE
import com.sun.jna.ptr.IntByReference
import kotlinx.coroutines.*
import kotlinx.coroutines.flow.MutableSharedFlow
import kotlinx.coroutines.flow.asSharedFlow
import ru.heatrk.antivirus.data.models.ApiMessage
import ru.heatrk.antivirus.data.models.structs.MessageStruct

class AntivirusApiImpl: AntivirusApi {
    private val kernel32 = Kernel32.INSTANCE
    private val advapi32 = Advapi32.INSTANCE

    private val coroutineScope = CoroutineScope(Dispatchers.IO + SupervisorJob())

    private var pipeListeningJob: Job? = null

    private val _incomingMessages = MutableSharedFlow<ApiMessage<MessageStruct>>()
    override val incomingMessages = _incomingMessages.asSharedFlow()

    private var outputPipe: HANDLE = WinNT.INVALID_HANDLE_VALUE
    private var inputPipe: HANDLE = WinNT.INVALID_HANDLE_VALUE

    init {
        coroutineScope.launch {
            startService()
        }
    }

    override suspend fun send(struct: MessageStruct): ApiMessage<Unit> {
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

        if (serviceStatusProcess.dwCurrentState != Winsvc.SERVICE_STOPPED &&
            serviceStatusProcess.dwCurrentState != Winsvc.SERVICE_STOP_PENDING) {

            advapi32.CloseServiceHandle(scManager)
            advapi32.CloseServiceHandle(service)

            return ApiMessage.Ok(true)
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

        return ApiMessage.Ok(isEnabled)
    }

    override suspend fun startService(): ApiMessage<Unit> {
        reset()

        val windowsServiceStartResult = startWindowsService()

        if (windowsServiceStartResult is ApiMessage.Fail) {
            return windowsServiceStartResult
        }

        outputPipe = kernel32.CreateFile(
            PIPE_SERVICE_INPUT_PATH,
            WinNT.GENERIC_WRITE,
            0,
            WinBase.SECURITY_ATTRIBUTES(),
            WinNT.OPEN_EXISTING,
            0,
            HANDLE(Pointer.NULL)
        )

        if (outputPipe == WinNT.INVALID_HANDLE_VALUE) {
            return ApiMessage.Fail(
                description = "Output pipe connecting",
                errorCode = kernel32.GetLastError()
            )
        }

        inputPipe = kernel32.CreateFile(
            PIPE_SERVICE_OUTPUT_PATH,
            WinNT.GENERIC_READ,
            0,
            WinBase.SECURITY_ATTRIBUTES(),
            WinNT.OPEN_EXISTING,
            0,
            HANDLE(Pointer.NULL)
        )

        if (inputPipe == WinNT.INVALID_HANDLE_VALUE) {
            return ApiMessage.Fail(
                description = "Input pipe connecting",
                errorCode = kernel32.GetLastError()
            )
        }

        pipeListeningJob = coroutineScope.launch {
            val buffer = ByteArray(PIPE_BUFFSIZE)

            while (inputPipe != WinNT.INVALID_HANDLE_VALUE) {
                val isSuccess = kernel32.ReadFile(
                    inputPipe,
                    buffer,
                    PIPE_BUFFSIZE,
                    IntByReference(),
                    WinBase.OVERLAPPED()
                )

                if (isSuccess) {
                    _incomingMessages.emit(ApiMessage.Ok(MessageStruct.create(buffer)))
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

    override suspend fun stopService(): ApiMessage<Unit> {
        reset()
        return stopWindowsService()
    }

    private fun reset() {
        pipeListeningJob?.cancel()
        pipeListeningJob = null
        inputPipe = WinNT.INVALID_HANDLE_VALUE
        outputPipe = WinNT.INVALID_HANDLE_VALUE
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
        private const val PIPE_BUFFSIZE = 512
        private const val ANTIVIRUS_SERVICE_NAME = "AntivirusService"
        private const val WINDOWS_SERVICE_STOP_TIMEOUT = 30000
    }
}