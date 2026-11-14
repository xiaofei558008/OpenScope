package app.openscope.androiddemo.ui

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.OutlinedButton
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import app.openscope.androiddemo.DemoViewModel
import app.openscope.androiddemo.domain.DemoSnapshot

@Composable
fun BrowseScreen(state: DemoSnapshot, viewModel: DemoViewModel) {
    var query by rememberSaveable { mutableStateOf("") }
    Column(
        Modifier.fillMaxSize().verticalScroll(rememberScrollState()),
        verticalArrangement = Arrangement.spacedBy(10.dp),
    ) {
        SectionCard(state.project.projectName) {
            if (!state.project.allowed) {
                Text("当前没有本通道的观察权。不展示变量值，也不把旧通道的结果留在这里。")
            } else if (state.project.empty) {
                Text("空工程。没有模块和变量。")
            } else if (state.project.rows.isEmpty()) {
                Text("没有符合「${state.project.appliedQuery}」的变量。")
            } else {
                val grouped = state.project.rows.groupBy { it.module }
                grouped.forEach { (module, rows) ->
                    Text(module)
                    rows.forEach { row ->
                        val channel = row.channel?.let { " · 曲线 ${it + 1}" } ?: " · 不进入波形"
                        Text("${row.name} · ${row.type} ${row.unit}$channel")
                    }
                }
            }
            Text("来源 ${state.waves.sourceLabel}。搜索取消或换代后，旧响应不会覆盖当前结果。迟到丢弃 ${state.project.staleDropped} 次。")
        }
        SectionCard("检索") {
            OutlinedTextField(
                value = query,
                onValueChange = { query = it },
                modifier = Modifier.fillMaxWidth(),
                label = { Text("模块或变量名") },
            )
            Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                OutlinedButton(onClick = { viewModel.searchNow(query) }) { Text("搜索") }
                OutlinedButton(onClick = {
                    query = ""
                    viewModel.cancelSearch()
                    viewModel.searchNow("")
                }) { Text("取消搜索") }
                OutlinedButton(onClick = { viewModel.demonstrateStaleSearch() }) { Text("模拟迟到响应") }
            }
            Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                OutlinedButton(onClick = { viewModel.setProjectEmpty(true) }) { Text("查看空工程") }
                OutlinedButton(onClick = { viewModel.setProjectEmpty(false) }) { Text("恢复演示工程") }
            }
        }
    }
}
