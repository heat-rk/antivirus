package ru.heatrk.antivirus.presentation.values.strings

import java.util.*

val strings get() = when(Locale.getDefault().language) {
    "ru" -> StringContainerRu
    else -> StringContainerRu
}

interface StringsContainer {
    val applicationName: String get() = "Antivirus"

    val info: String
    val error: String
    val reload: String
    val ok: String

    val startService: String
    val stopService: String

    val enableAntivirusService: String

    val infoButtonContentDescription: String

    val serviceStatusInfoDialogMessage: String

    fun errorMessage(description: String, errorCode: Int?): String
}