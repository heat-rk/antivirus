package ru.heatrk.antivirus.presentation.screens.scanner

import androidx.compose.desktop.ui.tooling.preview.Preview
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.material.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.window.rememberWindowState
import ru.heatrk.antivirus.presentation.dialogs.FileDialog
import ru.heatrk.antivirus.presentation.values.dimens.ElementsDimens
import ru.heatrk.antivirus.presentation.values.dimens.InsetsDimens
import ru.heatrk.antivirus.presentation.values.images.Drawables
import ru.heatrk.antivirus.presentation.values.strings.strings
import ru.heatrk.antivirus.presentation.values.styles.ApplicationTheme

@Composable
fun Scanner(
    state: ScannerViewState,
    onIntent: (ScannerIntent) -> Unit,
    modifier: Modifier = Modifier
) {
    when (state) {
        is ScannerViewState.Idle -> ScannerIdle(state, onIntent, modifier)
        is ScannerViewState.Running -> ScannerRunning(state, onIntent, modifier)
        is ScannerViewState.VirusesDetected -> ScannerVirusesDetected(state, onIntent, modifier)
    }
}

@Composable
private fun ScannerVirusesDetected(
    state: ScannerViewState.VirusesDetected,
    onIntent: (ScannerIntent) -> Unit,
    modifier: Modifier = Modifier
) {
    if (state.isFileDialogVisible) {
        FileDialog { file ->
            file?.let { onIntent(ScannerIntent.Start(file.absolutePath)) }
            onIntent(ScannerIntent.HideFileSelectionDialog)
        }
    }

    Surface(
        shape = ApplicationTheme.shapes.medium,
        modifier = modifier
    ) {
        Column(
            horizontalAlignment = Alignment.CenterHorizontally,
            verticalArrangement = Arrangement.Center,
            modifier = Modifier
                .padding(InsetsDimens.ExtraLarge)
                .fillMaxWidth()
                .wrapContentHeight()
        ) {
            Icon(
                painter = painterResource(Drawables.ErrorIcon),
                tint = ApplicationTheme.colors.error,
                contentDescription = null,
                modifier = Modifier
                    .size(ElementsDimens.ScanWarningIconSize)
            )

            Spacer(modifier = Modifier.height(InsetsDimens.Default))

            Text(
                text = strings.virusesDetected(state.virusesDetected),
                textAlign = TextAlign.Center
            )

            Spacer(modifier = Modifier.height(InsetsDimens.Default))

            Row(
                horizontalArrangement = Arrangement.SpaceEvenly,
                modifier = Modifier
                    .fillMaxWidth()
                    .wrapContentHeight()
            ) {
                Button(
                    onClick = { onIntent(ScannerIntent.ShowFileSelectionDialog) }
                ) {
                    Text(text = strings.scan)
                }

                Button(
                    onClick = { onIntent(ScannerIntent.More) }
                ) {
                    Text(text = strings.more)
                }
            }
        }
    }
}

@Composable
private fun ScannerIdle(
    state: ScannerViewState.Idle,
    onIntent: (ScannerIntent) -> Unit,
    modifier: Modifier = Modifier
) {
    if (state.isFileDialogVisible) {
        FileDialog { file ->
            file?.let { onIntent(ScannerIntent.Start(file.absolutePath)) }
            onIntent(ScannerIntent.HideFileSelectionDialog)
        }
    }

    Surface(
        shape = ApplicationTheme.shapes.medium,
        modifier = modifier
            .clickable(enabled = state.isEnabled) {
                onIntent(ScannerIntent.ShowFileSelectionDialog)
            }
    ) {
        Column(
            horizontalAlignment = Alignment.CenterHorizontally,
            verticalArrangement = Arrangement.Center,
            modifier = Modifier
                .padding(InsetsDimens.ExtraLarge)
                .fillMaxWidth()
                .wrapContentHeight()
        ) {
            Icon(
                painter = painterResource(Drawables.VirusScanIcon),
                tint = if (state.isEnabled) {
                    ApplicationTheme.colors.primary
                } else {
                    ApplicationTheme.colors.primaryDisabled
                },
                contentDescription = null
            )

            Spacer(modifier = Modifier.height(InsetsDimens.Default))

            Text(
                text = if (state.isEnabled) {
                    strings.clickToScan
                } else {
                    strings.scanUnavailable
                },
                textAlign = TextAlign.Center
            )
        }
    }
}

@Composable
private fun ScannerRunning(
    state: ScannerViewState.Running,
    onIntent: (ScannerIntent) -> Unit,
    modifier: Modifier = Modifier
) {
    Surface(
        shape = ApplicationTheme.shapes.medium,
        modifier = modifier
    ) {
        Column(
            verticalArrangement = Arrangement.Center,
            modifier = Modifier
                .padding(InsetsDimens.ExtraLarge)
                .fillMaxWidth()
                .wrapContentHeight()
        ) {
            Text(
                text = if (state.isPaused) {
                    strings.paused
                } else {
                    strings.scanOf(state.scanningPath)
                },
                overflow = TextOverflow.Ellipsis,
                maxLines = 1,
                modifier = Modifier
                    .fillMaxWidth()
            )

            Spacer(modifier = Modifier.height(InsetsDimens.Default))

            LinearProgressIndicator(
                progress = state.progress,
                color = if (state.isPaused) {
                    ApplicationTheme.colors.primaryDisabled
                } else {
                    ApplicationTheme.colors.primary
                },
                modifier = Modifier
                    .fillMaxWidth()
                    .height(ElementsDimens.ScanProgressIndicatorHeight)
            )

            Row(
                verticalAlignment = Alignment.CenterVertically,
                modifier = Modifier
                    .fillMaxWidth()
                    .wrapContentHeight()
            ) {
                Button(
                    onClick = { onIntent(ScannerIntent.More) }
                ) {
                    Text(text = strings.more)
                }

                Spacer(modifier = Modifier.weight(1f))

                Button(
                    onClick = {
                        if (state.isPaused) {
                            onIntent(ScannerIntent.Resume)
                        } else {
                            onIntent(ScannerIntent.Pause)
                        }
                    }
                ) {
                    Text(
                        text = if (state.isPaused) {
                            strings.resume
                        } else {
                            strings.pause
                        }
                    )
                }

                Spacer(modifier = Modifier.width(InsetsDimens.Default))

                Button(
                    onClick = { onIntent(ScannerIntent.Stop) }
                ) {
                    Text(text = strings.stop)
                }
            }
        }
    }
}

@Composable
@Preview
private fun ScannerIdlePreview() {
    ApplicationTheme {
        Scanner(
            state = ScannerViewState.Idle(
                isEnabled = true
            ),
            onIntent = {},
        )
    }
}

@Composable
@Preview
private fun ScannerRunningPreview() {
    ApplicationTheme {
        Scanner(
            state = ScannerViewState.Running(
                progress = 0.3f,
                scanningPath = "D://programs/game/start.exe",
                virusesDetected = 2,
                isPaused = false
            ),
            onIntent = {},
        )
    }
}

@Composable
@Preview
private fun ScannerVirusesDetectedPreview() {
    ApplicationTheme {
        Scanner(
            state = ScannerViewState.VirusesDetected(
                virusesDetected = 2
            ),
            onIntent = {},
        )
    }
}