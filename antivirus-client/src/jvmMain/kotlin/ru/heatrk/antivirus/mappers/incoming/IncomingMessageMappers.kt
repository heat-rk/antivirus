package ru.heatrk.antivirus.mappers.incoming

import com.sun.jna.Native
import ru.heatrk.antivirus.data.models.structs.MessageBodyErrorStruct
import ru.heatrk.antivirus.data.models.structs.MessageBodyStatusStruct
import ru.heatrk.antivirus.data.models.structs.MessageStruct
import ru.heatrk.antivirus.domain.models.Message
import ru.heatrk.antivirus.domain.models.MessagingParticipant
import ru.heatrk.antivirus.domain.models.incoming.IncomingMessageType
import ru.heatrk.antivirus.utils.timestampToLocalDateTime


fun MessageStruct.toDomain(): Message {
    val type = type

    val messageBody = when (type) {
        IncomingMessageType.STATUS -> {
            MessageBodyStatusStruct.create(body).toDomain()
        }
        IncomingMessageType.ERROR -> {
            MessageBodyErrorStruct.create(body).toDomain()
        }

        else -> throw Exception("Structure wrapper to domain mapping error (unsupported message type $type).")
    }

    return Message(
        source = toMessageParticipant(source),
        target = toMessageParticipant(target),
        dateTime = timestampToLocalDateTime(timestamp),
        type = type,
        body = messageBody
    )
}

private fun toMessageParticipant(bytes: ByteArray): MessagingParticipant {
    val bytesString = Native.toString(bytes)

    MessagingParticipant.values().forEach { participant ->
        if (participant.id.equals(bytesString, ignoreCase = true)) {
            return participant
        }
    }

    throw IllegalArgumentException("No such messaging participant ($bytesString).")
}