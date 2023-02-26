package ru.heatrk.antivirus.presentation.screens.antivirus_root

import com.arkivanov.decompose.ComponentContext
import kotlinx.coroutines.*
import kotlinx.coroutines.flow.*
import org.kodein.di.DI
import ru.heatrk.antivirus.AppConfig
import ru.heatrk.antivirus.domain.models.Message
import ru.heatrk.antivirus.domain.models.MessagingParticipant
import ru.heatrk.antivirus.domain.models.incoming.IncomingMessageBodyStatus
import ru.heatrk.antivirus.domain.models.outgoing.OutgoingMessageType
import ru.heatrk.antivirus.domain.repositories.MessagingRepository
import ru.heatrk.antivirus.presentation.common.Component
import ru.heatrk.antivirus.utils.tickerFlow

class AntivirusRootComponent(
    componentContext: ComponentContext,
    private val messagingRepository: MessagingRepository,
    private val defaultDispatcher: CoroutineDispatcher,
    di: DI
): Component(componentContext) {

    private val _state = MutableStateFlow<AntivirusRootViewState>(AntivirusRootViewState.Loading)
    val state = _state.asStateFlow()

    private var statusResponseWaitingJob: Job? = null

    init {
        messagingRepository.incomingMessages
            .filter(::isMessageRequiresHandling)
            .onEach(::handleMessage)
            .launchIn(componentScope)

        tickerFlow(
            period = AppConfig.STATUS_MESSAGE_TIME_CHECK_DELAY,
            initialDelay = 0L
        )
            .flowOn(defaultDispatcher)
            .filter { statusResponseWaitingJob == null }
            .onEach {
                waitForAnswer()

                println("send")

                messagingRepository.sendMessage(
                    Message.createOutgoingMessage(
                        source = MessagingParticipant.CLIENT_STATUS_RECEIVER,
                        target = MessagingParticipant.SERVER_STATUS_NOTIFIER,
                        type = OutgoingMessageType.STATUS_REQUEST
                    )
                )
            }
            .launchIn(componentScope)
    }

    private fun handleMessage(message: Message) {
        when (message.body) {
            is IncomingMessageBodyStatus -> {
                cancelWaitForAnswer()

                _state.value = if (message.body.status == IncomingMessageBodyStatus.Status.OK) {
                    AntivirusRootViewState.Ok
                } else {
                    AntivirusRootViewState.ServiceUnavailable
                }
            }
        }
    }

    private fun isMessageRequiresHandling(message: Message) =
        message.target ==  MessagingParticipant.CLIENT_STATUS_RECEIVER

    private fun waitForAnswer() {
        statusResponseWaitingJob = componentScope.launch(defaultDispatcher) {
            delay(AppConfig.STATUS_RESPONSE_TIME_LIMIT_MILLIS)
            _state.value = AntivirusRootViewState.ServiceUnavailable
            statusResponseWaitingJob = null
        }
    }

    private fun cancelWaitForAnswer() {
        statusResponseWaitingJob?.cancel()
        statusResponseWaitingJob = null
    }

    data class Args(
        val componentContext: ComponentContext
    )

    companion object {
        fun create(
            args: Args,
            messagingRepository: MessagingRepository,
            defaultDispatcher: CoroutineDispatcher,
            di: DI
        ) = AntivirusRootComponent(
            componentContext = args.componentContext,
            di = di,
            messagingRepository = messagingRepository,
            defaultDispatcher = defaultDispatcher
        )
    }
}