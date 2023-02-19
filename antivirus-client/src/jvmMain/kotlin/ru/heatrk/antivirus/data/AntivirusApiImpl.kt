package ru.heatrk.antivirus.data

import com.sun.jna.Pointer
import com.sun.jna.platform.win32.Kernel32
import com.sun.jna.platform.win32.WinBase
import com.sun.jna.platform.win32.WinNT
import com.sun.jna.ptr.IntByReference
import kotlinx.coroutines.flow.flow
import ru.heatrk.antivirus.data.models.ApiMessage

class AntivirusApiImpl: AntivirusApi {

    private val kernel32 = Kernel32.INSTANCE

    private val pipe = kernel32.CreateFile(
        PIPE_PATH,
        WinNT.GENERIC_READ or WinNT.GENERIC_WRITE,
        0,
        WinBase.SECURITY_ATTRIBUTES(),
         WinNT.OPEN_EXISTING,
        0,
        WinNT.HANDLE(Pointer.NULL)
    )

    override suspend fun send(byteArray: ByteArray) = kernel32.WriteFile(
        pipe,
        byteArray,
        PIPE_BUFFER_SIZE,
        IntByReference(),
        WinBase.OVERLAPPED()
    )

    override suspend fun incomingMessages() = flow {
        val buffer = ByteArray(PIPE_BUFFER_SIZE)

        while (pipe != WinNT.INVALID_HANDLE_VALUE) {
            val isSuccess = kernel32.ReadFile(
                pipe,
                buffer,
                PIPE_BUFFER_SIZE,
                IntByReference(),
                WinBase.OVERLAPPED()
            )

            if (isSuccess) {
                emit(ApiMessage.Ok(buffer))
            } else {
                emit(ApiMessage.Fail())
            }
        }
    }

    companion object {
        private const val PIPE_PATH = "\\\\.\\pipe\\antivirus-pipe"
        private const val PIPE_BUFFER_SIZE = 1024
    }
}