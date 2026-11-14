package app.openscope.androiddemo

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.activity.viewModels
import androidx.compose.runtime.DisposableEffect
import androidx.lifecycle.compose.LocalLifecycleOwner
import androidx.lifecycle.Lifecycle
import androidx.lifecycle.LifecycleEventObserver
import app.openscope.androiddemo.ui.DemoApp
import app.openscope.androiddemo.ui.DemoTheme
import java.io.File

class MainActivity : ComponentActivity() {
    private val viewModel: DemoViewModel by viewModels {
        DemoViewModel.factory(File(filesDir, "saved-demo-devices.txt"))
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {
            val lifecycleOwner = LocalLifecycleOwner.current
            DisposableEffect(lifecycleOwner) {
                val observer = LifecycleEventObserver { _, event ->
                    when (event) {
                        Lifecycle.Event.ON_STOP -> viewModel.onBackground()
                        Lifecycle.Event.ON_START -> viewModel.onForeground()
                        else -> Unit
                    }
                }
                lifecycleOwner.lifecycle.addObserver(observer)
                onDispose { lifecycleOwner.lifecycle.removeObserver(observer) }
            }
            DemoTheme {
                DemoApp(viewModel)
            }
        }
    }
}
