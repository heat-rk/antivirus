package ru.heatrk.antivirus.mappers.incoming

import ru.heatrk.antivirus.data.models.ApiMessage
import ru.heatrk.antivirus.domain.models.Message
import ru.heatrk.antivirus.domain.models.incoming.IncomingMessageBodyError
import ru.heatrk.antivirus.domain.models.incoming.IncomingMessageType
import ru.heatrk.antivirus.domain.models.MessagingParticipant
import ru.heatrk.antivirus.utils.timestampToLocalDateTime

fun ApiMessage.toDomain(): Message = when (this) {
    ApiMessage.Fail -> Message(
        source = MessagingParticipant.UNKNOWN,
        target = MessagingParticipant.UNKNOWN,
        dateTime = timestampToLocalDateTime(System.currentTimeMillis()),
        type = IncomingMessageType.ERROR,
        body = IncomingMessageBodyError()
    )

    is ApiMessage.Ok -> structureWrapper.toDomain()
}