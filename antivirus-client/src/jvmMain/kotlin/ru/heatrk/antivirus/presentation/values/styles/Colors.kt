package ru.heatrk.antivirus.presentation.values.styles

import androidx.compose.material.lightColors
import androidx.compose.runtime.Immutable
import androidx.compose.runtime.staticCompositionLocalOf
import androidx.compose.ui.graphics.Color

@Immutable
data class ApplicationColors(

    val primaryDisabled: Color,
    val materialColors: androidx.compose.material.Colors
) {
    val primary = materialColors.primary
    val primaryVariant = materialColors.primaryVariant
    val onPrimary = materialColors.onPrimary
    val secondary = materialColors.onSecondary
    val secondaryVariant = materialColors.secondaryVariant
    val onSecondary = materialColors.onSecondary
    val background = materialColors.background
    val onBackground = materialColors.onBackground
    val surface = materialColors.surface
    val onSurface = materialColors.onSurface
    val error = materialColors.error
    val onError = materialColors.onError
    val isLight = materialColors.isLight
}

val LocalApplicationColors = staticCompositionLocalOf { lightApplicationColors }

val lightApplicationColors = ApplicationColors(
    primaryDisabled = Color(0xFFBBA7D1),
    materialColors = lightColors(
        primary = Color(0xFF8D28FA),
        primaryVariant = Color(0xFF6124A3),
        onPrimary = Color(0xFFFFFFFF),
        secondary = Color(0xFF8D77FC),
        secondaryVariant = Color(0xFF6551CF),
        onSecondary = Color(0xFF000000),
        background = Color(0xFFFFFFFF),
        onBackground = Color(0xFF000000),
        surface = Color(0xFFe9d7fc),
        onSurface = Color(0xFF000000),
        error = Color(0xFF8A2929),
        onError = Color(0xFFFFFFFF)
    )
)

val darkApplicationColors = ApplicationColors(
    primaryDisabled = Color(0xFFBBA7D1),
    materialColors = lightColors(
        primary = Color(0xFF8D28FA),
        primaryVariant = Color(0xFF6124A3),
        onPrimary = Color(0xFFFFFFFF),
        secondary = Color(0xFF8D77FC),
        secondaryVariant = Color(0xFF6551CF),
        onSecondary = Color(0xFF000000),
        background = Color(0xFFFFFFFF),
        onBackground = Color(0xFF000000),
        surface = Color(0xFFe9d7fc),
        onSurface = Color(0xFF000000),
        error = Color(0xFF8A2929),
        onError = Color(0xFFFFFFFF)
    )
)