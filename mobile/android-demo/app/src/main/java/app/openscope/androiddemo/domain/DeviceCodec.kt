package app.openscope.androiddemo.domain

import java.io.File

/**
 * Saved demo devices are selection labels only.
 * The codec has no token, grant, TTL, or password field.
 */
object DeviceCodec {
    fun encode(items: List<SavedDeviceEntry>): String =
        items.joinToString("\n") { item ->
            listOf(
                item.localId,
                item.label,
                item.mode.name,
                item.serverLabel,
                item.pcLabel,
                item.requestId,
            ).joinToString("\t") { escape(it) }
        }

    fun decode(text: String): List<SavedDeviceEntry> {
        if (text.isBlank()) return emptyList()
        return text.lineSequence().mapNotNull { line ->
            if (line.isBlank()) return@mapNotNull null
            val parts = line.split('\t').map { unescape(it) }
            if (parts.size < 6) return@mapNotNull null
            val mode = runCatching { ConnectionMode.valueOf(parts[2]) }.getOrNull()
                ?: return@mapNotNull null
            SavedDeviceEntry(
                localId = parts[0],
                label = parts[1],
                mode = mode,
                serverLabel = parts[3],
                pcLabel = parts[4],
                requestId = parts[5],
            )
        }.toList()
    }

    private fun escape(value: String): String =
        value.replace("\\", "\\\\").replace("\t", "\\t").replace("\n", "\\n")

    private fun unescape(value: String): String {
        val out = StringBuilder()
        var index = 0
        while (index < value.length) {
            val ch = value[index]
            if (ch == '\\' && index + 1 < value.length) {
                when (value[index + 1]) {
                    '\\' -> out.append('\\')
                    't' -> out.append('\t')
                    'n' -> out.append('\n')
                    else -> out.append(value[index + 1])
                }
                index += 2
            } else {
                out.append(ch)
                index += 1
            }
        }
        return out.toString()
    }
}

interface SavedDeviceStore {
    fun load(): List<SavedDeviceEntry>
    fun save(items: List<SavedDeviceEntry>)
}

class MemoryDeviceStore(initial: List<SavedDeviceEntry> = emptyList()) : SavedDeviceStore {
    private var items = initial
    override fun load(): List<SavedDeviceEntry> = items
    override fun save(items: List<SavedDeviceEntry>) {
        this.items = items
    }
}

class FileDeviceStore(private val file: File) : SavedDeviceStore {
    override fun load(): List<SavedDeviceEntry> =
        if (!file.exists()) emptyList() else DeviceCodec.decode(file.readText())

    override fun save(items: List<SavedDeviceEntry>) {
        file.parentFile?.mkdirs()
        file.writeText(DeviceCodec.encode(items))
    }
}
