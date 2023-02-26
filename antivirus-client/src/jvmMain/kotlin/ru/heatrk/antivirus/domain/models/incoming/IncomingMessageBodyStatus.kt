package ru.heatrk.antivirus.domain.models.incoming

import ru.heatrk.antivirus.domain.models.MessageBody

data class IncomingMessageBodyStatus(
    val status: Status
): MessageBody {
    enum class Status {
        OK, ERROR
    }
}