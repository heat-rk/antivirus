package ru.heatrk.antivirus.presentation.dialogs

import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import kotlinx.coroutines.DelicateCoroutinesApi
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch
import kotlinx.coroutines.swing.Swing
import ru.heatrk.antivirus.presentation.values.strings.strings
import java.io.File
import javax.swing.JFileChooser

@OptIn(DelicateCoroutinesApi::class)
@Composable
fun FileDialog(
    onCloseRequest: (files: File?) -> Unit
) {
    DisposableEffect(Unit) {
        val job = GlobalScope.launch(Dispatchers.Swing) {
            val fileChooser = JFileChooser()

            fileChooser.dialogTitle = strings.selectFilesToScan
            fileChooser.isMultiSelectionEnabled = false
            fileChooser.isAcceptAllFileFilterUsed = true
            fileChooser.fileSelectionMode = JFileChooser.FILES_AND_DIRECTORIES

            val returned = fileChooser.showOpenDialog(null)

            onCloseRequest(
                when(returned) {
                    JFileChooser.APPROVE_OPTION -> fileChooser.selectedFile
                    else -> null
                }
            )
        }

        onDispose {
            job.cancel()
        }
    }
}