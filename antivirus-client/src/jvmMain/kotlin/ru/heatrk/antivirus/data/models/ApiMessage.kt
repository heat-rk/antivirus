package ru.heatrk.antivirus.data.models

import ru.heatrk.antivirus.data.models.structs.MessageStruct

sealed interface ApiMessage {
    class Ok(val structureWrapper: MessageStruct): ApiMessage

    object Fail: ApiMessage
}