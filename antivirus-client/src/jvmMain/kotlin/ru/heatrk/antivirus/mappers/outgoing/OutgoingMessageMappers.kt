package ru.heatrk.antivirus.mappers.outgoing

import com.sun.jna.Native
import ru.heatrk.antivirus.data.models.structs.MessageStruct
import ru.heatrk.antivirus.domain.models.Message
import ru.heatrk.antivirus.domain.models.outgoing.OutgoingMessageType
import ru.heatrk.antivirus.utils.toTimestamp

fun Message.toStruct(): MessageStruct {
    val struct = MessageStruct()

    Native.toByteArray(source.id).copyInto(struct.source)
    Native.toByteArray(target.id).copyInto(struct.target)

    struct.timestamp = dateTime.toTimestamp()
    struct.type = type

    when (type) {
        OutgoingMessageType.STATUS_REQUEST -> Unit
        else -> throw Exception("Structure wrapper to domain mapping error (unsupported message type $type).")
    }

    return struct
}