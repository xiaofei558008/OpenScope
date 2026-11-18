package app.openscope.androiddemo.ui

import androidx.compose.foundation.isSystemInDarkTheme
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Typography
import androidx.compose.material3.darkColorScheme
import androidx.compose.material3.lightColorScheme
import androidx.compose.runtime.Composable
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.TextStyle
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.sp

private val Ink = Color(0xFF16343A)
private val Teal = Color(0xFF0F5C5C)
private val Paper = Color(0xFFF3F5F7)
private val Amber = Color(0xFF8A5A00)

private val LightColors = lightColorScheme(
    primary = Teal,
    onPrimary = Color.White,
    secondary = Amber,
    onSecondary = Color.White,
    background = Paper,
    surface = Color.White,
    onBackground = Ink,
    onSurface = Ink,
    error = Color(0xFF8C2F2F),
)

private val DarkColors = darkColorScheme(
    primary = Color(0xFF8FDBD4),
    onPrimary = Color(0xFF003735),
    secondary = Color(0xFFE7C27A),
    background = Color(0xFF10181B),
    surface = Color(0xFF1A2428),
    onBackground = Color(0xFFE7EEF0),
    onSurface = Color(0xFFE7EEF0),
)

private val AppType = Typography(
    titleLarge = TextStyle(fontSize = 22.sp, fontWeight = FontWeight.SemiBold),
    titleMedium = TextStyle(fontSize = 16.sp, fontWeight = FontWeight.SemiBold),
    bodyMedium = TextStyle(fontSize = 14.sp),
    bodySmall = TextStyle(fontSize = 12.sp),
    labelLarge = TextStyle(fontSize = 14.sp, fontWeight = FontWeight.Medium),
)

@Composable
fun DemoTheme(content: @Composable () -> Unit) {
    MaterialTheme(
        colorScheme = if (isSystemInDarkTheme()) DarkColors else LightColors,
        typography = AppType,
        content = content,
    )
}
