@file:OptIn(ExperimentalMaterialApi::class)

package ru.heatrk.antivirus.presentation.dialogs

import androidx.compose.desktop.ui.tooling.preview.Preview
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.material.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.res.painterResource
import ru.heatrk.antivirus.presentation.values.dimens.ElementsDimens
import ru.heatrk.antivirus.presentation.values.dimens.InsetsDimens
import ru.heatrk.antivirus.presentation.values.images.Drawables
import ru.heatrk.antivirus.presentation.values.strings.strings
import ru.heatrk.antivirus.presentation.values.styles.ApplicationTheme

@Composable
fun MessageDialog(
    dialogState: DialogState,
    onDismiss: () -> Unit,
) {
    when (dialogState) {
        is DialogState.Error -> {
            MessageDialog(
                title = dialogState.title,
                titleIcon = Drawables.ErrorIcon,
                titleTint = ApplicationTheme.colors.error,
                message = dialogState.message,
                onDismiss = onDismiss
            )
        }

        is DialogState.Info -> {
            MessageDialog(
                title = dialogState.title,
                titleIcon = Drawables.InfoIcon,
                titleTint = ApplicationTheme.colors.primary,
                message = dialogState.message,
                onDismiss = onDismiss
            )
        }

        DialogState.Gone -> Unit
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
            dialogState = DialogState.Error(
                title = "Ошибка!",
                message = "Что-то пошло не так!"
            ),
            onDismiss = {}
        )
    }
}