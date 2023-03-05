package ru.heatrk.antivirus.mappers.incoming

import ru.heatrk.antivirus.data.models.ApiMessage
import ru.heatrk.antivirus.data.models.structs.MessageStruct
import ru.heatrk.antivirus.domain.models.Message
import ru.heatrk.antivirus.domain.models.incoming.IncomingMessageBodyError
import ru.heatrk.antivirus.domain.models.incoming.IncomingMessageType
import ru.heatrk.antivirus.domain.models.MessagingParticipant
import ru.heatrk.antivirus.utils.timestampToLocalDateTime

fun ApiMessage<MessageStruct>.toDomain(): Message = when (this) {
    is ApiMessage.Fail -> Message(
        source = MessagingParticipant.UNKNOWN,
        target = MessagingParticipant.UNKNOWN,
        dateTime = timestampToLocalDateTime(System.currentTimeMillis()),
        type = IncomingMessageType.ERROR,
        body = IncomingMessageBodyError(
            message = "error: $description; code: $errorCode"
        )
    )

    is ApiMessage.Ok -> body.toDomain()
}