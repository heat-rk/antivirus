package ru.heatrk.antivirus.mappers.incoming

import ru.heatrk.antivirus.data.models.structs.MessageBodyStatusStruct
import ru.heatrk.antivirus.domain.models.incoming.IncomingMessageBodyStatus

private const val MESSAGE_TYPE_STATUS_OK_BYTE: Byte = 0

fun MessageBodyStatusStruct.toDomain() = IncomingMessageBodyStatus(
    status = statusOf(status)
)

private fun statusOf(byte: Byte?) = when (byte) {
    MESSAGE_TYPE_STATUS_OK_BYTE -> IncomingMessageBodyStatus.Status.OK
    null -> throw Exception("Status message to domain mapping error [type].")
    else -> IncomingMessageBodyStatus.Status.ERROR
}