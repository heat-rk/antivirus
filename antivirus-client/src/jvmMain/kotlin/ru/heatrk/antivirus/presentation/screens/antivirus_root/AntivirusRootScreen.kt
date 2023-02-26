package ru.heatrk.antivirus.presentation.screens.antivirus_root

import androidx.compose.desktop.ui.tooling.preview.Preview
import androidx.compose.foundation.layout.*
import androidx.compose.material.Button
import androidx.compose.material.CircularProgressIndicator
import androidx.compose.material.Surface
import androidx.compose.material.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import ru.heatrk.antivirus.presentation.values.dimens.ElementsDimens
import ru.heatrk.antivirus.presentation.values.strings.strings
import ru.heatrk.antivirus.presentation.values.styles.ApplicationTheme

@Composable
fun AntivirusRootScreen(
    component: AntivirusRootComponent
) {
    val antivirusRootViewState by component.state.collectAsState()
    AntivirusRootScreen(state = antivirusRootViewState)
}

@Composable
private fun AntivirusRootScreen(
    state: AntivirusRootViewState
) {
    when (state) {
        is AntivirusRootViewState.Loading -> {
            AntivirusRootLoadingScreen(state)
        }

        is AntivirusRootViewState.Ok -> {
            AntivirusRootOkScreen(state)
        }

        is AntivirusRootViewState.ServiceUnavailable -> {
            AntivirusRootServiceUnavailableScreen(state)
        }
    }
}

@Composable
private fun AntivirusRootLoadingScreen(
    state: AntivirusRootViewState.Loading
) {
    Box(
        contentAlignment = Alignment.Center,
        modifier = Modifier.fillMaxSize()
    ) {
        CircularProgressIndicator()
    }
}

@Composable
private fun AntivirusRootOkScreen(
    state: AntivirusRootViewState.Ok
) {

}

@Composable
private fun AntivirusRootServiceUnavailableScreen(
    state: AntivirusRootViewState.ServiceUnavailable
) {
    Box(
        contentAlignment = Alignment.Center,
        modifier = Modifier.fillMaxSize()
    ) {
        Surface(
            shape = ApplicationTheme.shapes.medium,
            modifier = Modifier
                .size(
                    width = ElementsDimens.ServiceUnavailableWindowWidth,
                    height = ElementsDimens.ServiceUnavailableWindowHeight
                )
        ) {
            Column(
                horizontalAlignment = Alignment.CenterHorizontally,
                verticalArrangement = Arrangement.SpaceEvenly,
                modifier = Modifier.fillMaxSize()
            ) {
                Text(
                    text = strings.serviceUnavailableTitle,
                    style = ApplicationTheme.typography.h3
                )

                Text(
                    text = strings.serviceUnavailableMessage,
                    style = ApplicationTheme.typography.subtitle1
                )

                Button(
                    onClick = { /* TODO() */ }
                ) {
                    Text(
                        text = strings.reload,
                        style = ApplicationTheme.typography.button
                    )
                }
            }
        }
    }
}

@Composable
@Preview
private fun AntivirusRootScreenPreview() {
    ApplicationTheme {
        AntivirusRootScreen(state = AntivirusRootViewState.Ok)
    }
}