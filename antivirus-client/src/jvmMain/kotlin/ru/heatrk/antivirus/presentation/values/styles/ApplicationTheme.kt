package ru.heatrk.antivirus.presentation.values.styles

import androidx.compose.foundation.isSystemInDarkTheme
import androidx.compose.material.MaterialTheme
import androidx.compose.material.Shapes
import androidx.compose.runtime.Composable
import androidx.compose.runtime.CompositionLocalProvider
import androidx.compose.material.Typography

@Composable
fun ApplicationTheme(content: @Composable () -> Unit) {
    val applicationColors = if (isSystemInDarkTheme()) darkApplicationColors else lightApplicationColors

    CompositionLocalProvider(LocalApplicationColors provides applicationColors) {
        MaterialTheme(
            colors = applicationColors.materialColors,
            shapes = applicationShapes,
            content = content
        )
    }
}

object ApplicationTheme {
    val colors: ApplicationColors
        @Composable
        get() = LocalApplicationColors.current

    val shapes: Shapes
        @Composable
        get() = MaterialTheme.shapes

    val typography: Typography
        @Composable
        get() = MaterialTheme.typography
}