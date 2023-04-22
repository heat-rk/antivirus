@file:OptIn(ExperimentalMaterialApi::class)

package ru.heatrk.antivirus.presentation.dialogs

import androidx.compose.desktop.ui.tooling.preview.Preview
import androidx.compose.foundation.layout.*
import androidx.compose.material.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.res.painterResource
import ru.heatrk.antivirus.presentation.values.dimens.ElementsDimens
import ru.heatrk.antivirus.presentation.values.dimens.InsetsDimens
import ru.heatrk.antivirus.presentation.values.images.Drawables
import ru.heatrk.antivirus.presentation.values.strings.strings
import ru.heatrk.antivirus.presentation.values.styles.ApplicationTheme

@Composable
fun MessageDialog(
    messageDialogState: MessageDialogState,
    onDismiss: () -> Unit,
) {
    when (messageDialogState) {
        is MessageDialogState.Error -> {
            MessageDialog(
                title = messageDialogState.title,
                titleIcon = Drawables.ErrorIcon,
                titleTint = ApplicationTheme.colors.error,
                message = messageDialogState.message,
                onDismiss = onDismiss
            )
        }

        is MessageDialogState.Info -> {
            MessageDialog(
                title = messageDialogState.title,
                titleIcon = Drawables.InfoIcon,
                titleTint = ApplicationTheme.colors.primary,
                message = messageDialogState.message,
                onDismiss = onDismiss
            )
        }

        MessageDialogState.Gone -> Unit
    }
}

@Composable
fun MessageDialog(
    title: String,
    titleIcon: String,
    titleTint: Color,
    message: String,
    onDismiss: () -> Unit,
) {
    AlertDialog(
        onDismissRequest = onDismiss,
        title = {
            Row(
                verticalAlignment = Alignment.CenterVertically,
                modifier = Modifier.fillMaxWidth()
            ) {
                Icon(
                    painter = painterResource(titleIcon),
                    tint = titleTint,
                    contentDescription = null,
                    modifier = Modifier.size(ElementsDimens.DialogTitleIconSize)
                )

                Spacer(modifier = Modifier.width(InsetsDimens.Default))

                Text(
                    text = title,
                    color = titleTint
                )
            }
        },
        confirmButton = {
            Button(
                onClick = onDismiss
            ) {
                Text(text = strings.ok)
            }
        },
        text = {
            Text(text = message)
        },
        modifier = Modifier
            .width(ElementsDimens.DialogWidth)
            .wrapContentHeight()
    )
}

@Preview
@Composable
fun MessageDialogPreview() {
    ApplicationTheme {
        MessageDialog(
            messageDialogState = MessageDialogState.Error(
                title = "Ошибка!",
                message = "Что-то пошло не так!"
            ),
            onDismiss = {}
        )
    }
}