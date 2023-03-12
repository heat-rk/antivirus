@file:Suppress("OPT_IN_IS_NOT_ENABLED")

package ru.heatrk.antivirus.presentation.routing

import androidx.compose.runtime.Composable
import com.arkivanov.decompose.ComponentContext
import com.arkivanov.decompose.extensions.compose.jetbrains.stack.Children
import com.arkivanov.decompose.router.stack.ChildStack
import com.arkivanov.decompose.router.stack.StackNavigation
import com.arkivanov.decompose.router.stack.childStack
import com.arkivanov.decompose.router.stack.pop
import com.arkivanov.decompose.value.Value
import com.arkivanov.essenty.parcelable.Parcelable
import org.kodein.di.DI
import org.kodein.di.instance
import ru.heatrk.antivirus.presentation.screens.antivirus_root.AntivirusRootComponent
import ru.heatrk.antivirus.presentation.screens.antivirus_root.AntivirusRootScreen

class DecomposeRouter(
    componentContext: ComponentContext,
    private val di: DI
): Router {
    private val navigation = StackNavigation<Config>()

    private val stack =
        componentContext.childStack(
            source = navigation,
            initialConfiguration = Config.AntivirusRootScreen,
            handleBackButton = true,
            childFactory = ::createChild,
        )

    private val childStack: Value<ChildStack<*, @Composable () -> Unit>> get() = stack

    @Composable
    override fun Container() {
        Children(childStack) {
            it.instance()
        }
    }

    override fun navigateUp() {
        navigation.pop()
    }

    private fun createChild(config: Config, componentContext: ComponentContext): @Composable () -> Unit =
        when (config) {
            is Config.AntivirusRootScreen -> createAntivirusRootScreen(componentContext)
        }

    private fun createAntivirusRootScreen(componentContext: ComponentContext): @Composable () -> Unit = {
        val trackerComponent by di.instance<AntivirusRootComponent.Args, AntivirusRootComponent>(
            arg = AntivirusRootComponent.Args(componentContext = componentContext)
        )

        AntivirusRootScreen(component = trackerComponent)
    }

    private sealed class Config: Parcelable {
        object AntivirusRootScreen: Config()
    }
}