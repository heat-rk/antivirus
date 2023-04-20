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

    override val scanUnavailable = "Сканирование невозможно. Проверьте работу сервиса!"

    override val scan = "Сканировать"
    override val clickToScan = "Нажми, чтобы начать сканирование!"
    override val pause = "Пауза"
    override val paused = "Приостановлено"
    override val stop = "Остановить"
    override val resume = "Продолжить"
    override val more = "Подробнее"
    override val ellipsis = "..."

    override val selectFilesToScan = "Выберите файлы для сканирования"

    override fun errorMessage(description: String, errorCode: Int?) =
        buildString {
            append("Что-то пошло не так. Ошибка: $description")

            if (errorCode != null) {
                append("; код $errorCode")
            }
        }

    override fun scanOf(path: String) =
        "Сканирование: $path"

    override fun virusesDetected(count: Int) =
        "Вирусов обнаружено: $count"
}