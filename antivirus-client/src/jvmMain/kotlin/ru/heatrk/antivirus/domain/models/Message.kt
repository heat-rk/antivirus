package ru.heatrk.antivirus.domain.models

import ru.heatrk.antivirus.utils.timestampToLocalDateTime
import java.time.LocalDateTime

data class Message(
    val source: MessagingParticipant,
    val target: MessagingParticipant,
    val dateTime: LocalDateTime,
    val type: Byte,
    val body: MessageBody? = null
) {
    companion object {
        fun createOutgoingMessage(
            source: MessagingParticipant,
            target: MessagingParticipant,
            type: Byte,
            body: MessageBody? = null
        ) = Message(
            source = source,
            target = target,
            type = type,
            dateTime = timestampToLocalDateTime(System.currentTimeMillis()),
            body = body
        )
    }
}