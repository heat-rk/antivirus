package ru.heatrk.antivirus.presentation.screens.service_control

import androidx.compose.foundation.layout.*
import androidx.compose.material.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.painterResource
import ru.heatrk.antivirus.presentation.dialogs.MessageDialog
import ru.heatrk.antivirus.presentation.values.dimens.ElementsDimens
import ru.heatrk.antivirus.presentation.values.dimens.InsetsDimens
import ru.heatrk.antivirus.presentation.values.images.Drawables
import ru.heatrk.antivirus.presentation.values.strings.strings
import ru.heatrk.antivirus.presentation.values.styles.ApplicationTheme

@Composable
fun ServiceControl(
    state: ServiceControlViewState,
    onIntent: (ServiceControlIntent) -> Unit,
    modifier: Modifier
) {
    when (state) {
        is ServiceControlViewState.Loading -> ServiceControlLoading(state, modifier)
        is ServiceControlViewState.Ok -> ServiceControlOk(state, onIntent, modifier)
    }
}

@Composable
private fun ServiceControlLoading(
    state: ServiceControlViewState.Loading,
    modifier: Modifier
) {
    Box(
        contentAlignment = Alignment.Center,
        modifier = modifier
    ) {
        CircularProgressIndicator()
    }
}

@Composable
private fun ServiceControlOk(
    state: ServiceControlViewState.Ok,
    onIntent: (ServiceControlIntent) -> Unit,
    modifier: Modifier
) {
    MessageDialog(
        dialogState = state.dialogState,
        onDismiss = { onIntent(ServiceControlIntent.DialogDismiss) }
    )

    Surface(
        shape = ApplicationTheme.shapes.medium,
        modifier = modifier
    ) {
        Row(
            verticalAlignment = Alignment.CenterVertically,
            modifier = Modifier
                .fillMaxWidth()
                .wrapContentHeight()
                .padding(InsetsDimens.Default)
        ) {
            IconButton(
                onClick = { onIntent(ServiceControlIntent.ShowInfo) },
                modifier = Modifier.size(ElementsDimens.InfoIconButtonSize)
            ) {
                Icon(
                    painter = painterResource(Drawables.InfoIcon),
                    tint = ApplicationTheme.colors.primary,
                    contentDescription = strings.infoButtonContentDescription
                )
            }

            Spacer(modifier = Modifier.width(InsetsDimens.Default))

            Text(text = strings.enableAntivirusService)

            Spacer(modifier = Modifier.weight(1f))

            Switch(
                checked = state.isServiceEnabled,
                onCheckedChange = { onIntent(ServiceControlIntent.EnabledChange(it)) }
            )
        }
    }
}