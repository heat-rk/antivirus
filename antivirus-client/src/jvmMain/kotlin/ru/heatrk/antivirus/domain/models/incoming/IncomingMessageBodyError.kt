package ru.heatrk.antivirus.domain.models.incoming

import ru.heatrk.antivirus.domain.models.MessageBody

data class IncomingMessageBodyError(
    val message: String? = null
): MessageBody