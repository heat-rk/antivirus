package ru.heatrk.antivirus.presentation.values.strings

object StringContainerRu: StringsContainer {
    override val info = "Внимание!"
    override val error = "Ошибка!"
    override val reload = "Перезагрузить"
    override val ok = "Ок"

    override val startService = "Запустить сервис"
    override val stopService = "Остановить сервис"

    override val enableAntivirusService = "Включить службу антивируса"

    override val infoButtonContentDescription = "Информация о работе сервиса"

    override val serviceStatusInfoDialogMessage =
        "Обратите внимание на то, " +
                "что полностью отключить службу работы антивируса невозможно! " +
                "Служба автоматически начинает свою работу во время запуска ОС."

    override fun errorMessage(description: String, errorCode: Int?) =
        buildString {
            append("Что-то пошло не так. Ошибка: $description")

            if (errorCode != null) {
                append("; код $errorCode")
            }
        }
}