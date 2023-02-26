package ru.heatrk.antivirus.mappers.incoming

import com.sun.jna.Native
import ru.heatrk.antivirus.data.models.structs.MessageBodyErrorStruct
import ru.heatrk.antivirus.domain.models.incoming.IncomingMessageBodyError

fun MessageBodyErrorStruct.toDomain() = IncomingMessageBodyError(
    message = Native.toString(message)
)

