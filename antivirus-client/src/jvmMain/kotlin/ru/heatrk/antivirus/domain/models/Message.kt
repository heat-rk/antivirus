package ru.heatrk.antivirus.domain.models

import java.time.LocalDateTime

data class Message(
    val dateTime: LocalDateTime,
    val body: MessageBody? = null
)