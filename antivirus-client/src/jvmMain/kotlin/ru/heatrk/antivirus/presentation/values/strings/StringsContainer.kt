package ru.heatrk.antivirus.presentation.values.strings

import java.util.*

val strings get() = when(Locale.getDefault().language) {
    "ru" -> StringContainerRu
    else -> StringContainerRu
}

interface StringsContainer {
    val applicationName: String get() = "Antivirus"

    val reload: String

    val serviceUnavailableTitle: String
    val serviceUnavailableMessage: String
}