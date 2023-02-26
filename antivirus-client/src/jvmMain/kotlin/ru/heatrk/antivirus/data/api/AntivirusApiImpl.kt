package ru.heatrk.antivirus.data.api

import com.sun.jna.Pointer
import com.sun.jna.platform.win32.Kernel32
import com.sun.jna.platform.win32.WinBase
import com.sun.jna.platform.win32.WinNT
import com.sun.jna.platform.win32.WinNT.HANDLE
import com.sun.jna.ptr.IntByReference
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.SupervisorJob
import kotlinx.coroutines.flow.MutableSharedFlow
import kotlinx.coroutines.flow.asSharedFlow
import kotlinx.coroutines.launch
import ru.heatrk.antivirus.data.models.ApiMessage
import ru.heatrk.antivirus.data.models.structs.MessageStruct

class AntivirusApiImpl: AntivirusApi {
    private val kernel32 = Kernel32.INSTANCE

    private val coroutineScope = CoroutineScope(Dispatchers.IO + SupervisorJob())

    private val _incomingMessages = MutableSharedFlow<ApiMessage>()
    override val incomingMessages = _incomingMessages.asSharedFlow()

    private val outputPipe: HANDLE = kernel32.CreateFile(
        PIPE_SERVICE_INPUT_PATH,
        WinNT.GENERIC_WRITE,
        0,
        WinBase.SECURITY_ATTRIBUTES(),
         WinNT.OPEN_EXISTING,
        0,
        HANDLE(Pointer.NULL)
    )

    private val inputPipe: HANDLE = kernel32.CreateFile(
        PIPE_SERVICE_OUTPUT_PATH,
        WinNT.GENERIC_READ,
        0,
        WinBase.SECURITY_ATTRIBUTES(),
        WinNT.OPEN_EXISTING,
        0,
        HANDLE(Pointer.NULL)
    )


    init {
        coroutineScope.launch {
            val buffer = ByteArray(MessageStruct.size)

            while (inputPipe != WinNT.INVALID_HANDLE_VALUE) {
                val isSuccess = kernel32.ReadFile(
                    inputPipe,
                    buffer,
                    PIPE_BUFFSIZE,
                    IntByReference(),
                    WinBase.OVERLAPPED()
                )

                val instance = MessageStruct.create(buffer)

                if (isSuccess) {
                    _incomingMessages.emit(ApiMessage.Ok(instance))
                } else {
                    _incomingMessages.emit(ApiMessage.Fail)
                }
            }
        }
    }

    override suspend fun send(struct: MessageStruct): Boolean {
        val bytes = ByteArray(MessageStruct.size).also { struct.write(it) }

        return kernel32.WriteFile(
            outputPipe,
            bytes,
            PIPE_BUFFSIZE,
            IntByReference(),
            null
        )
    }

    companion object {
        private const val PIPE_SERVICE_INPUT_PATH = "\\\\.\\pipe\\antivirus-pipe-service-input"
        private const val PIPE_SERVICE_OUTPUT_PATH = "\\\\.\\pipe\\antivirus-pipe-service-output"
        private const val PIPE_BUFFSIZE = 512
    }
}