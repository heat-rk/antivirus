package ru.heatrk.antivirus.di

import com.arkivanov.decompose.ComponentContext
import com.arkivanov.decompose.DefaultComponentContext
import com.arkivanov.essenty.lifecycle.LifecycleRegistry
import org.kodein.di.DI
import org.kodein.di.DirectDIAware
import org.kodein.di.bindSingleton
import org.kodein.di.instance
import ru.heatrk.antivirus.presentation.root.RootComponent
import ru.heatrk.antivirus.presentation.routing.DecomposeRouter
import ru.heatrk.antivirus.presentation.routing.Router

private const val DEFAULT_COMPONENT_CONTEXT = "defaultComponentContext"
const val ROOT_COMPONENT = "rootComponent"

val appModule = DI.Module("appModule") {
    bindSingleton { LifecycleRegistry() }
    bindSingleton<ComponentContext>(tag = DEFAULT_COMPONENT_CONTEXT) { DefaultComponentContext(instance()) }
    bindSingleton<Router>{ DecomposeRouter(defaultComponentContextInstance(), di) }
    bindSingleton(tag = ROOT_COMPONENT){ RootComponent(defaultComponentContextInstance(), instance()) }
}

fun DirectDIAware.defaultComponentContextInstance() = instance<ComponentContext>(DEFAULT_COMPONENT_CONTEXT)

