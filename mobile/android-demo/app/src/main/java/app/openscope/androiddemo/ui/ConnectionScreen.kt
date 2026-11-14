package app.openscope.androiddemo.ui

import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.heightIn
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.FilterChip
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedButton
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import app.openscope.androiddemo.DemoViewModel
import app.openscope.androiddemo.domain.ConnectionMode
import app.openscope.androiddemo.domain.DemoCopy
import app.openscope.androiddemo.domain.DemoScenarios
import app.openscope.androiddemo.domain.DemoSnapshot
import app.openscope.androiddemo.domain.EntitlementPlaceholder
import app.openscope.androiddemo.domain.OrderPlaceholder
import app.openscope.androiddemo.domain.PcDecision
import app.openscope.androiddemo.domain.TransportState

@Composable
fun ConnectionScreen(state: DemoSnapshot, viewModel: DemoViewModel, tablet: Boolean) {
    val scroll = rememberScrollState()
    if (tablet) {
        Row(Modifier.fillMaxSize(), horizontalArrangement = Arrangement.spacedBy(12.dp)) {
            Column(
                Modifier
                    .weight(1.1f)
                    .verticalScroll(scroll),
                verticalArrangement = Arrangement.spacedBy(10.dp),
            ) {
                ModeAndHealth(state, viewModel)
                PairingCard(state, viewModel)
            }
            Column(
                Modifier
                    .weight(0.9f)
                    .heightIn(max = 2000.dp)
                    .verticalScroll(rememberScrollState()),
                verticalArrangement = Arrangement.spacedBy(10.dp),
            ) {
                SessionCard(state, viewModel)
                SavedCard(state, viewModel)
                PlaceholderCard(state, viewModel)
            }
        }
    } else {
        Column(
            Modifier.fillMaxSize().verticalScroll(scroll),
            verticalArrangement = Arrangement.spacedBy(10.dp),
        ) {
            ModeAndHealth(state, viewModel)
            PairingCard(state, viewModel)
            SessionCard(state, viewModel)
            SavedCard(state, viewModel)
            PlaceholderCard(state, viewModel)
        }
    }
}

@Composable
private fun ModeAndHealth(state: DemoSnapshot, viewModel: DemoViewModel) {
    SectionCard("连接模式") {
        Text("三种名称都在本进程里模拟。失败会停在当前模式。", style = MaterialTheme.typography.bodySmall)
        Row(horizontalArrangement = Arrangement.spacedBy(8.dp), modifier = Modifier.horizontalScroll(rememberScrollState())) {
            ConnectionMode.entries.forEach { mode ->
                FilterChip(
                    selected = state.mode == mode,
                    onClick = { viewModel.requestMode(mode) },
                    label = { Text(mode.label) },
                )
            }
        }
        Text(state.mode.detail, style = MaterialTheme.typography.bodySmall)
        Text("代数 ${state.epoch} · 静默改走官方云：${if (state.silentCloudSwitch) "发生" else "未发生"}")
        Text("明文降级：${if (state.plaintextFallback) "发生" else "未发生"}")
        if (state.mode == ConnectionMode.SelfHosted) {
            var address by rememberSaveable { mutableStateOf("") }
            OutlinedTextField(
                value = address,
                onValueChange = { address = it },
                modifier = Modifier.fillMaxWidth(),
                label = { Text("自托管地址占位") },
                supportingText = { Text("只保存文字。没有 SSH、密码或主机管理凭据。") },
            )
            Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                OutlinedButton(onClick = { viewModel.setSelfHostAddress(address) }) { Text("保存地址占位") }
                OutlinedButton(onClick = { viewModel.injectCertFailure() }) { Text("模拟证书失败") }
            }
            if (state.selfHostAddress.isNotBlank()) Text("当前占位：${state.selfHostAddress}")
        }
        if (state.mode == ConnectionMode.LanDirect) {
            OutlinedButton(onClick = { viewModel.injectLanFailure() }) { Text("模拟局域网失败") }
        }
        state.failureNote?.let { Text(it) }
        if (state.transport == TransportState.Failed) {
            OutlinedButton(onClick = { viewModel.continueCurrentMode() }) {
                Text("仍用${state.mode.label}继续演示")
            }
        }
    }
    SectionCard("五层状态") {
        Text("传输：${state.health.transport}")
        Text("认证与配对：${state.health.pairing}")
        Text("通道：${state.health.channel}")
        Text("采集：${state.health.acquisition}")
        Text("新鲜度：${state.health.freshness}")
        Text(state.roleLabel, style = MaterialTheme.typography.bodySmall)
    }
}

@Composable
private fun PairingCard(state: DemoSnapshot, viewModel: DemoViewModel) {
    var manual by rememberSaveable { mutableStateOf("demo-ok") }
    SectionCard("配对") {
        Text(
            "模拟扫码不使用相机，也不申请权限。手输是同一条演示路径。",
            style = MaterialTheme.typography.bodySmall,
        )
        Row(Modifier.horizontalScroll(rememberScrollState()), horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            DemoScenarios.all.forEach { item ->
                FilterChip(
                    selected = manual == item.id,
                    onClick = {
                        manual = item.id
                        viewModel.parseInput(item.id)
                    },
                    label = { Text(item.id) },
                )
            }
        }
        Text(DemoScenarios.all.find { it.id == manual }?.blurb.orEmpty(), style = MaterialTheme.typography.bodySmall)
        OutlinedTextField(
            value = manual,
            onValueChange = { manual = it },
            modifier = Modifier.fillMaxWidth(),
            label = { Text("手输演示场景") },
        )
        Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            OutlinedButton(onClick = { viewModel.parseInput(manual) }) { Text("解析手输") }
            OutlinedButton(onClick = {
                manual = "javascript:alert(1)"
                viewModel.parseInput(manual)
            }) { Text("填入恶意示例") }
        }
        val preview = state.preview
        if (preview != null) {
            Text("声明的服务器：${preview.declaredServer}")
            Text("声明的 PC：${preview.declaredPc}")
            if (preview.verified) {
                Text("已验证的服务器：${preview.verifiedServer}")
                Text("已验证的 PC：${preview.verifiedPc}")
            } else {
                Text("已验证的服务器：尚未验证")
                Text("已验证的 PC：尚未验证")
            }
            if (preview.rejected) Text("拒绝：${preview.rejectCode}。打开次数 0，访问次数 ${state.fetchCount}。")
        }
        Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            OutlinedButton(onClick = { viewModel.verify() }, enabled = state.preview?.rejected == false) {
                Text("验证来源")
            }
            val mismatch = state.preview?.mismatch == true && state.preview?.acknowledged == false
            OutlinedButton(
                onClick = {
                    if (mismatch) viewModel.acknowledgeVerified()
                    viewModel.applyPairing()
                },
                enabled = state.preview?.verified == true && state.preview?.rejected == false,
            ) {
                Text(if (mismatch) "按已验证身份申请" else "申请配对")
            }
        }
        Text(state.localPairingNote)
        state.lastMessage?.let { Text(it, style = MaterialTheme.typography.bodySmall) }
        Text(
            "等待 PC 配对确认是独立进度，不是命令七态。手机不能自行批准。",
            style = MaterialTheme.typography.bodySmall,
        )
        Row(Modifier.horizontalScroll(rememberScrollState()), horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            OutlinedButton(onClick = { viewModel.pcDecide(PcDecision.Approve) }, enabled = state.pcActionsEnabled) {
                Text("模拟 PC 同意")
            }
            OutlinedButton(onClick = { viewModel.pcDecide(PcDecision.Deny) }, enabled = state.pcActionsEnabled) {
                Text("模拟 PC 拒绝")
            }
            OutlinedButton(onClick = { viewModel.pcDecide(PcDecision.Cancel) }, enabled = state.pcActionsEnabled) {
                Text("模拟 PC 取消")
            }
            OutlinedButton(onClick = { viewModel.pcDecide(PcDecision.Timeout) }, enabled = state.pcActionsEnabled) {
                Text("模拟 PC 超时")
            }
        }
        TextButton(onClick = { viewModel.selfApprove() }) { Text("尝试由手机自行批准") }
        state.storedApproval?.let { Text("记录中的审批：${DemoCopy.approval(it)}") }
        Text(
            "兑换计数 ${state.redemptionCount} · 本次新发授权 ${yesNo(state.grantCreatedOnLastAttempt)} · " +
                "延长有效期 ${yesNo(state.ttlExtended)} · 再次消耗 ${yesNo(state.reconsumed)}",
            style = MaterialTheme.typography.bodySmall,
        )
    }
}

@Composable
private fun SessionCard(state: DemoSnapshot, viewModel: DemoViewModel) {
    SectionCard("断开与重连") {
        Text(if (state.viewingLive) "本代数正在观察。" else "本代数没有正在观察。")
        Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            OutlinedButton(onClick = { viewModel.disconnect() }) { Text("断开") }
            OutlinedButton(onClick = { viewModel.requestReconnect() }) { Text("请求重连") }
        }
        OutlinedButton(onClick = { viewModel.recoverSame() }) { Text("重新核验同一申请") }
        OutlinedButton(onClick = { viewModel.resumeViewing() }, enabled = state.resumeEnabled) {
            Text("按当前审批恢复观察")
        }
        Text("保存的设备不会自动恢复授权。重连只换代数并清掉旧样本。", style = MaterialTheme.typography.bodySmall)
        state.sourceChangeNote?.let { Text(it, style = MaterialTheme.typography.bodySmall) }
    }
}

@Composable
private fun SavedCard(state: DemoSnapshot, viewModel: DemoViewModel) {
    var label by rememberSaveable { mutableStateOf("车间平板") }
    SectionCard("记住的演示设备") {
        Text("条目只用于以后点选。它不是持久授权。删除本地条目也不会变成服务端吊销。")
        OutlinedTextField(
            value = label,
            onValueChange = { label = it },
            modifier = Modifier.fillMaxWidth(),
            label = { Text("设备显示名") },
        )
        OutlinedButton(onClick = { viewModel.rememberDevice(label) }) { Text("记住当前设备") }
        if (state.saved.isEmpty()) {
            Text("还没有记住的设备。")
        }
        state.saved.forEach { entry ->
            Text("${entry.label} · ${entry.mode.label} · ${entry.pcLabel}")
            Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                OutlinedButton(onClick = { viewModel.selectSaved(entry.localId) }) { Text("点选") }
                TextButton(onClick = { viewModel.deleteSavedLocal(entry.localId) }) { Text("删除本地记录") }
            }
        }
        TextButton(onClick = { viewModel.revokeServer() }) { Text("模拟服务端吊销") }
        Text(if (state.serverRevoked) "服务端吊销标记：已吊销。" else "服务端吊销标记：未吊销。")
    }
}

@Composable
private fun PlaceholderCard(state: DemoSnapshot, viewModel: DemoViewModel) {
    SectionCard("权益占位") {
        Text("这是占位，不是订单，也不是结账。没有价格、支付 SDK 或真实扣款。")
        Text(DemoCopy.order(state.entitlement.serverOrder))
        Text(DemoCopy.entitlement(state.entitlement.entitlement))
        Text(if (state.entitlement.returnObserved) "已观察到模拟回跳。" else "还没有模拟回跳。")
        Text(state.entitlement.note, style = MaterialTheme.typography.bodySmall)
        Row(Modifier.horizontalScroll(rememberScrollState()), horizontalArrangement = Arrangement.spacedBy(8.dp)) {
            OutlinedButton(onClick = { viewModel.observePaymentReturn() }) { Text("模拟支付回跳") }
            OutlinedButton(onClick = { viewModel.serverOrder(OrderPlaceholder.Active) }) { Text("服务端订单 active") }
            OutlinedButton(onClick = { viewModel.serverEntitlement(EntitlementPlaceholder.Active) }) {
                Text("服务端权益 active")
            }
            OutlinedButton(onClick = { viewModel.serverEntitlement(EntitlementPlaceholder.Exhausted) }) {
                Text("服务端权益 exhausted")
            }
            OutlinedButton(onClick = { viewModel.serverEntitlement(EntitlementPlaceholder.Unpurchased) }) {
                Text("服务端权益未购买")
            }
        }
        Text(state.settingsNote, style = MaterialTheme.typography.bodySmall)
    }
}

private fun yesNo(value: Boolean): String = if (value) "是" else "否"
