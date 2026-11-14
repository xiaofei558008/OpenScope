package app.openscope.androiddemo.ui

import androidx.compose.foundation.layout.BoxWithConstraints
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.AlertDialog
import androidx.compose.material3.NavigationBar
import androidx.compose.material3.NavigationBarItem
import androidx.compose.material3.NavigationRail
import androidx.compose.material3.NavigationRailItem
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import app.openscope.androiddemo.DemoViewModel

private enum class Dest(val label: String) {
    Connect("连接"),
    Project("工程"),
    Values("数值"),
    Waves("波形"),
    Debug("调试"),
}

@Composable
fun DemoApp(viewModel: DemoViewModel) {
    val state by viewModel.state.collectAsState()
    var dest by rememberSaveable { mutableStateOf(Dest.Connect.name) }
    val current = Dest.valueOf(dest)

    state.pendingMode?.let { next ->
        AlertDialog(
            onDismissRequest = { viewModel.cancelModeSwitch() },
            title = { Text("切换连接模式") },
            text = {
                Text(
                    "离开「${state.mode.label}」，改到「${next.label}」。" +
                        "旧代数的样本会清除，观察需要重新验证。",
                )
            },
            confirmButton = {
                TextButton(onClick = { viewModel.confirmModeSwitch() }) {
                    Text("确认切换并清空旧通道")
                }
            },
            dismissButton = {
                TextButton(onClick = { viewModel.cancelModeSwitch() }) {
                    Text("留下")
                }
            },
        )
    }

    BoxWithConstraints {
        val tablet = maxWidth >= 840.dp
        Scaffold(
            topBar = { DemoBanner(state.lifecycleNote) },
            bottomBar = {
                if (!tablet) {
                    NavigationBar {
                        Dest.entries.forEach { item ->
                            NavigationBarItem(
                                selected = current == item,
                                onClick = { dest = item.name },
                                label = { Text(item.label) },
                                icon = { Text(item.label.take(1)) },
                            )
                        }
                    }
                }
            },
        ) { padding ->
            Row(
                Modifier
                    .fillMaxSize()
                    .padding(padding),
            ) {
                if (tablet) {
                    NavigationRail {
                        Dest.entries.forEach { item ->
                            NavigationRailItem(
                                selected = current == item,
                                onClick = { dest = item.name },
                                label = { Text(item.label) },
                                icon = { Text(item.label.take(1)) },
                            )
                        }
                    }
                }
                Column(Modifier.weight(1f).padding(horizontal = 12.dp, vertical = 8.dp)) {
                    when (current) {
                        Dest.Connect -> ConnectionScreen(state, viewModel, tablet)
                        Dest.Project -> BrowseScreen(state, viewModel)
                        Dest.Values -> NumericScreen(state, viewModel, tablet)
                        Dest.Waves -> WaveformScreen(state, viewModel, tablet)
                        Dest.Debug -> DebugScreen(state, viewModel)
                    }
                }
            }
        }
    }
}
