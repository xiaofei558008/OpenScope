package app.openscope.androiddemo.domain

/**
 * Command display model.
 *
 * Raw cloud/PC states stay as the seven values. The UI groups them into five
 * labels and keeps a separate local confidence. Disconnect after submit marks
 * local confidence unknown and does not rewrite raw, replay, or query.
 * There is no control_query operation in this demo.
 */
class CommandBoard {
    private val items = ArrayDeque<StoredCommand>()
    private var nextId = 1

    val controlQueryCount: Int = 0
    var replayCount: Int = 0
        private set

    fun localReject(note: String): StoredCommand {
        val stored = StoredCommand(
            id = "cmd-${nextId++}",
            raw = null,
            local = LocalCommandConfidence.NotSent,
            pcConfirm = PcCommandConfirm.None,
            executed = false,
            note = note,
        )
        push(stored)
        return stored
    }

    fun inject(raw: CloudRaw): StoredCommand {
        val stored = StoredCommand(
            id = "cmd-${nextId++}",
            raw = raw,
            local = LocalCommandConfidence.Submitted,
            pcConfirm = PcCommandConfirm.Waiting,
            executed = false,
            note = "模拟回执。未执行设备动作，未调用 control_query。",
        )
        push(stored)
        return stored
    }

    fun setPcConfirm(id: String, confirm: PcCommandConfirm) {
        replace(id) { it.copy(pcConfirm = confirm) }
    }

    fun onDisconnected() {
        val updated = ArrayDeque<StoredCommand>()
        items.forEach { item ->
            val raw = item.raw
            val inFlight = item.local == LocalCommandConfidence.Submitted &&
                raw != null &&
                raw !in TERMINAL
            updated.add(
                if (inFlight) item.copy(local = LocalCommandConfidence.DisconnectedUnknown) else item,
            )
        }
        items.clear()
        items.addAll(updated)
    }

    fun views(): List<CommandView> = items.map { it.toView() }

    private fun push(stored: StoredCommand) {
        items.addLast(stored)
        while (items.size > MAX_VISIBLE) items.removeFirst()
    }

    private fun replace(id: String, block: (StoredCommand) -> StoredCommand) {
        val updated = ArrayDeque<StoredCommand>()
        items.forEach { item ->
            updated.add(if (item.id == id) block(item) else item)
        }
        items.clear()
        items.addAll(updated)
    }

    data class StoredCommand(
        val id: String,
        val raw: CloudRaw?,
        val local: LocalCommandConfidence,
        val pcConfirm: PcCommandConfirm,
        val executed: Boolean,
        val note: String,
    ) {
        fun toView(): CommandView {
            val raw = raw
            val group: String
            val sub: String
            if (local == LocalCommandConfidence.NotSent || raw == null) {
                group = "本地态"
                sub = "未送出。不伪造云回执。"
            } else if (local == LocalCommandConfidence.DisconnectedUnknown) {
                group = "未知"
                sub = "本地未知 · 原始云态仍为 ${DemoCopy.raw(raw)} ${DemoCopy.sub(raw)}"
            } else {
                group = DemoCopy.group(raw)
                sub = DemoCopy.sub(raw)
            }
            val localLabel = when (local) {
                LocalCommandConfidence.NotSent -> "本地把握：未送出"
                LocalCommandConfidence.Submitted -> "本地把握：已提交"
                LocalCommandConfidence.DisconnectedUnknown -> "本地把握：断线后未知"
            }
            val pcLabel = when (pcConfirm) {
                PcCommandConfirm.None -> "PC 确认：未请求"
                PcCommandConfirm.Waiting -> "PC 确认：等待本地确认"
                PcCommandConfirm.Confirmed -> "PC 确认：已确认（不是成功回执）"
                PcCommandConfirm.Refused -> "PC 确认：已拒绝"
                PcCommandConfirm.Cancelled -> "PC 确认：已取消"
            }
            return CommandView(
                id = id,
                raw = raw,
                rawLabel = if (raw == null) "原始云态：无" else "原始云态：${DemoCopy.raw(raw)}",
                groupLabel = "界面分组：$group",
                sublabel = sub,
                localLabel = localLabel,
                pcConfirm = pcConfirm,
                pcLabel = pcLabel,
                executed = executed,
            )
        }
    }

    companion object {
        const val MAX_VISIBLE = 8
        val TERMINAL = setOf(CloudRaw.Succeeded, CloudRaw.Failed, CloudRaw.Rejected)
    }
}
