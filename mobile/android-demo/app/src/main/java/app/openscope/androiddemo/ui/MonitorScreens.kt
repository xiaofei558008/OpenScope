package app.openscope.androiddemo.ui

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedButton
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.StrokeCap
import androidx.compose.ui.unit.dp
import app.openscope.androiddemo.DemoViewModel
import app.openscope.androiddemo.domain.CloudRaw
import app.openscope.androiddemo.domain.DemoCatalog
import app.openscope.androiddemo.domain.DemoCopy
import app.openscope.androiddemo.domain.DemoSnapshot
import app.openscope.androiddemo.domain.Freshness
import app.openscope.androiddemo.domain.PcCommandConfirm
import app.openscope.androiddemo.domain.Sample

private val ChannelColors = listOf(
    Color(0xFF0F6E6E),
    Color(0xFFB86E00),
    Color(0xFF3E5C8A),
    Color(0xFF5C6670),
)

@Composable
fun NumericScreen(state: DemoSnapshot, viewModel: DemoViewModel, tablet: Boolean) {
    Column(
        Modifier.fillMaxSize().verticalScroll(rememberScrollState()),
        verticalArrangement = Arrangement.spacedBy(10.dp),
    ) {
        Text("显示节奏低于演示生成节奏。改显示不会改变真实采样，这里也没有真实采样。")
        Text("新鲜度：${state.health.freshness}")
        val rows = if (tablet) state.numbers.chunked(2) else state.numbers.map { listOf(it) }
        rows.forEach { pair ->
            Row(horizontalArrangement = Arrangement.spacedBy(10.dp), modifier = Modifier.fillMaxWidth()) {
                pair.forEach { card ->
                    SectionCard(card.name, Modifier.weight(1f)) {
                        Text(card.displayValue + if (card.unit.isBlank()) "" else " ${card.unit}")
                        Text("类型 ${card.typeName}")
                        Text("来源 ${card.source}")
                        Text("时间来源 ${card.timeSource}")
                        Text("最后更新 ${card.updatedAt}")
                        Text("质量 ${card.quality}")
                    }
                }
            }
        }
        Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            OutlinedButton(onClick = { viewModel.holdFreshness(Freshness.Timeout) }) { Text("模拟新鲜度超时") }
            OutlinedButton(onClick = { viewModel.holdFreshness(null) }) { Text("按样本重新判断") }
        }
    }
}

@Composable
fun WaveformScreen(state: DemoSnapshot, viewModel: DemoViewModel, tablet: Boolean) {
    Column(
        Modifier.fillMaxSize().verticalScroll(rememberScrollState()),
        verticalArrangement = Arrangement.spacedBy(10.dp),
    ) {
        SectionCard("合成波形") {
            Text(if (state.waves.frozen) "视窗已冻结。生成仍可继续，这不是 ECU 暂停。" else "视窗跟随最新演示样本。")
            Text("来源 ${state.waves.sourceLabel} · 缩放 ${state.waves.zoomStep} · 缺口 ${state.waves.gapCount}")
            Text("界面可见点数 ${state.waves.liveCount}。超订被拒 ${state.waves.extraRejected} 次，曲线槽仍是 4。")
            state.waves.sourceChangeNote?.let { Text(it) }
            val names = DemoCatalog.rows.filter { it.channel != null }
            val channels = state.waves.channels
            if (channels.all { it.isEmpty() }) {
                Text("无样本。空白不是数值 0。")
            } else {
                val paired = names.zip(channels)
                val grid = if (tablet) paired.chunked(2) else paired.map { listOf(it) }
                grid.forEach { row ->
                    Row(horizontalArrangement = Arrangement.spacedBy(8.dp), modifier = Modifier.fillMaxWidth()) {
                        row.forEach { (meta, samples) ->
                            Column(Modifier.weight(1f)) {
                                val color = ChannelColors[meta.channel ?: 0]
                                Text("${meta.name} · ${meta.unit.ifBlank { "无单位" }}")
                                WaveCanvas(samples, color, Modifier.fillMaxWidth().height(96.dp))
                                val tail = samples.takeLast(3).joinToString("  ") { sample ->
                                    val mark = if (sample.gapBefore) "缺口 " else ""
                                    "$mark${sample.seq}:${format(sample.value)}"
                                }
                                Text(if (tail.isBlank()) "无数值替代" else "数值替代 $tail", style = MaterialTheme.typography.bodySmall)
                            }
                        }
                    }
                }
            }
            Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                OutlinedButton(onClick = { viewModel.toggleFreeze() }) {
                    Text(if (state.waves.frozen) "返回实时" else "冻结视窗")
                }
                OutlinedButton(onClick = { viewModel.zoomIn() }) { Text("放大") }
                OutlinedButton(onClick = { viewModel.zoomOut() }) { Text("缩小") }
                OutlinedButton(onClick = { viewModel.rejectExtraSubscription() }) { Text("尝试第 5 条曲线") }
            }
            Text(state.settingsNote, style = MaterialTheme.typography.bodySmall)
        }
    }
}

@Composable
private fun WaveCanvas(samples: List<Sample>, color: Color, modifier: Modifier) {
    Canvas(modifier) {
        if (samples.size < 2) return@Canvas
        val min = samples.minOf { it.value }
        val max = samples.maxOf { it.value }
        val span = (max - min).let { if (it < 0.001f) 1f else it }
        fun x(index: Int) = size.width * index / (samples.size - 1)
        fun y(value: Float) = size.height - (value - min) / span * (size.height - 8f) - 4f
        var previous = Offset.Zero
        var pen = false
        samples.forEachIndexed { index, sample ->
            val point = Offset(x(index), y(sample.value))
            if (!pen || sample.gapBefore) {
                drawCircle(color, radius = 3.5f, center = point)
            } else {
                drawLine(color, previous, point, strokeWidth = 3f, cap = StrokeCap.Round)
            }
            pen = true
            previous = point
        }
    }
}

@Composable
fun DebugScreen(state: DemoSnapshot, viewModel: DemoViewModel) {
    Column(
        Modifier.fillMaxSize().verticalScroll(rememberScrollState()),
        verticalArrangement = Arrangement.spacedBy(10.dp),
    ) {
        SectionCard("调试状态") {
            Text("控制与观察分开。${state.roleLabel}")
            Text("控制可执行：${if (state.controlExecutable) "是" else "否"}。control_query 调用 ${state.controlQueryCount} 次。重放 ${state.replayCount} 次。")
            Text("PC 确认进度单独显示。确认本身不是成功，断线也不会改写原始云态。")
            OutlinedButton(onClick = { viewModel.submitControlAttempt() }) { Text("尝试提交控制") }
            Text("注入模拟回执只改展示，不写设备。", style = MaterialTheme.typography.bodySmall)
            Row(Modifier.horizontalScroll(rememberScrollState()), horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                CloudRaw.entries.forEach { raw ->
                    OutlinedButton(onClick = { viewModel.injectReceipt(raw) }) {
                        Text(DemoCopy.raw(raw))
                    }
                }
            }
        }
        if (state.commands.isEmpty()) {
            Text("还没有命令记录。")
        }
        state.commands.forEach { command ->
            SectionCard(command.id) {
                Text(command.rawLabel)
                Text(command.groupLabel)
                Text("子文案：${command.sublabel}")
                Text(command.localLabel)
                Text(command.pcLabel)
                Text("已执行：${if (command.executed) "是" else "否"}")
                Row(Modifier.horizontalScroll(rememberScrollState()), horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                    OutlinedButton(onClick = { viewModel.setPcCommandConfirm(command.id, PcCommandConfirm.Confirmed) }) {
                        Text("PC 已确认")
                    }
                    OutlinedButton(onClick = { viewModel.setPcCommandConfirm(command.id, PcCommandConfirm.Refused) }) {
                        Text("PC 拒绝确认")
                    }
                    OutlinedButton(onClick = { viewModel.disconnect() }) { Text("提交后模拟断线") }
                }
            }
        }
    }
}

private fun format(value: Float): String {
    val rounded = (value * 10f).toInt() / 10f
    return if (rounded == 0f) "0" else rounded.toString()
}
