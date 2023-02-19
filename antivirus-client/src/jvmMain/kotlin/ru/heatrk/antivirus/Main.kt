package ru.heatrk.antivirus

import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.window.Window
import androidx.compose.ui.window.WindowState
import androidx.compose.ui.window.application
import kotlinx.coroutines.flow.launchIn
import kotlinx.coroutines.flow.onEach
import kotlinx.coroutines.runBlocking
import org.kodein.di.compose.rememberInstance
import org.kodein.di.compose.withDI
import ru.heatrk.antivirus.data.AntivirusApiImpl
import ru.heatrk.antivirus.data.models.ApiMessage
import ru.heatrk.antivirus.di.ROOT_COMPONENT
import ru.heatrk.antivirus.di.modules
import ru.heatrk.antivirus.presentation.root.RootComponent
import ru.heatrk.antivirus.presentation.root.RootScreen
import ru.heatrk.antivirus.presentation.values.dimens.ElementsDimens
import ru.heatrk.antivirus.presentation.values.strings.strings
import ru.heatrk.antivirus.presentation.values.styles.ApplicationTheme
import ru.heatrk.antivirus.utils.app_unique.AppUniqueUtil

@Composable
fun App(rootComponent: RootComponent)  {
    ApplicationTheme {
        RootScreen(rootComponent)
    }
}

fun main() {
    if (AppUniqueUtil.isAlreadyRunning("ru.heatrk.antivirus")) {
        return
    }

    runBlocking {
        AntivirusApiImpl().apply {
            send("Hello, C++!\n".toByteArray())

            incomingMessages().onEach {
                when (it) {
                    is ApiMessage.Fail -> println("error!")
                    is ApiMessage.Ok -> println(String(it.body))
                }
            }.launchIn(this@runBlocking)
        }
    }

    application {
        withDI(*modules) {
            val rootComponent: RootComponent by rememberInstance(ROOT_COMPONENT)

            val windowState by remember {
                mutableStateOf(
                    WindowState(
                        width = ElementsDimens.AppDefaultWidth,
                        height = ElementsDimens.AppDefaultHeight
                    )
                )
            }

            Window(
                onCloseRequest = ::exitApplication,
                state = windowState,
                title = strings.applicationName
            ) {
                App(rootComponent)
            }
        }
    }
}
