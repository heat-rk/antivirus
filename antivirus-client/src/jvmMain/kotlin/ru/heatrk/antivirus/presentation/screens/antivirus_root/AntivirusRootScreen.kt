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
import ru.heatrk.antivirus.presentation.dialogs.MessageDialog
import ru.heatrk.antivirus.presentation.screens.scanner.Scanner
import ru.heatrk.antivirus.presentation.screens.scanner.ScannerIntent
import ru.heatrk.antivirus.presentation.screens.scanner.ScannerViewState
import ru.heatrk.antivirus.presentation.screens.service_control.ServiceControl
import ru.heatrk.antivirus.presentation.screens.service_control.ServiceControlIntent
import ru.heatrk.antivirus.presentation.screens.service_control.ServiceControlViewState
import ru.heatrk.antivirus.presentation.values.dimens.ElementsDimens
import ru.heatrk.antivirus.presentation.values.dimens.InsetsDimens
import ru.heatrk.antivirus.presentation.values.strings.strings
import ru.heatrk.antivirus.presentation.values.styles.ApplicationTheme

@Composable
fun AntivirusRootScreen(
    component: AntivirusRootComponent
) {
    val antivirusRootViewState by component.state.collectAsState()
    val serviceControlViewState by component.serviceControlComponent.state.collectAsState()
    val scannerViewState by component.scannerComponent.state.collectAsState()

    AntivirusRootScreen(
        state = antivirusRootViewState,
        onIntent = component::onIntent,
        serviceControlViewState = serviceControlViewState,
        onServiceControlIntent = component.serviceControlComponent::onIntent,
        scannerViewState = scannerViewState,
        onScannerIntent = component.scannerComponent::onIntent
    )
}

@Composable
private fun AntivirusRootScreen(
    state: AntivirusRootViewState,
    onIntent: (AntivirusRootIntent) -> Unit,
    serviceControlViewState: ServiceControlViewState,
    onServiceControlIntent: (ServiceControlIntent) -> Unit,
    scannerViewState: ScannerViewState,
    onScannerIntent: (ScannerIntent) -> Unit
) {
    when (state) {
        is AntivirusRootViewState.Loading -> {
            AntivirusRootLoadingScreen(state)
        }

        is AntivirusRootViewState.Ok -> {
            AntivirusRootOkScreen(
                state = state,
                onIntent = onIntent,
                serviceControlViewState = serviceControlViewState,
                onServiceControlIntent = onServiceControlIntent,
                scannerViewState = scannerViewState,
                onScannerIntent = onScannerIntent
            )
        }

        is AntivirusRootViewState.Error -> {
            AntivirusRootErrorScreen(
                state = state,
                onIntent = onIntent
            )
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
    state: AntivirusRootViewState.Ok,
    onIntent: (AntivirusRootIntent) -> Unit,
    serviceControlViewState: ServiceControlViewState,
    onServiceControlIntent: (ServiceControlIntent) -> Unit,
    scannerViewState: ScannerViewState,
    onScannerIntent: (ScannerIntent) -> Unit
) {
    MessageDialog(
        messageDialogState = state.messageDialogState,
        onDismiss = { onIntent(AntivirusRootIntent.MessageDialogDismiss) }
    )

    Column(
        modifier = Modifier
            .fillMaxWidth()
            .wrapContentHeight()
            .padding(InsetsDimens.Default)
    ) {
        ServiceControl(
            state = serviceControlViewState,
            onIntent = onServiceControlIntent,
            modifier = Modifier
                .fillMaxWidth()
                .wrapContentHeight()
        )

        Spacer(modifier = Modifier.height(InsetsDimens.Default))

        Scanner(
            state = scannerViewState,
            onIntent = onScannerIntent,
            modifier = Modifier
                .fillMaxWidth()
                .wrapContentHeight()
        )
    }
}

@Composable
private fun AntivirusRootErrorScreen(
    state: AntivirusRootViewState.Error,
    onIntent: (AntivirusRootIntent) -> Unit,
) {
    Box(
        contentAlignment = Alignment.Center,
        modifier = Modifier.fillMaxSize()
    ) {
        Surface(
            shape = ApplicationTheme.shapes.medium,
            modifier = Modifier
                .size(
                    width = ElementsDimens.ErrorWindowWidth,
                    height = ElementsDimens.ErrorWindowHeight
                )
        ) {
            Column(
                horizontalAlignment = Alignment.CenterHorizontally,
                verticalArrangement = Arrangement.SpaceEvenly,
                modifier = Modifier.fillMaxSize()
            ) {
                Text(
                    text = strings.error,
                    style = ApplicationTheme.typography.h3
                )

                Text(
                    text = state.message,
                    style = ApplicationTheme.typography.subtitle1
                )

                Button(
                    onClick = { onIntent(AntivirusRootIntent.Reload) }
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
        AntivirusRootScreen(
            state = AntivirusRootViewState.Ok(),
            onIntent = {},
            serviceControlViewState = ServiceControlViewState.Ok(
                isServiceEnabled = true
            ),
            onServiceControlIntent = {},
            scannerViewState = ScannerViewState.Running(
                progress = 0.3f,
                scanningPath = "D://programs/game/start.exe",
                virusesDetected = 2,
                isPaused = false
            ),
            onScannerIntent = {}
        )
    }
}