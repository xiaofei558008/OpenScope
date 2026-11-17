package app.openscope.androiddemo.domain

/**
 * Demo-only gate in front of the visible ring.
 *
 * Identity here is the pair already stored on [Sample]: epoch and sourceKey.
 * There is no separate wire generation, channel credential, or timestamp type.
 * [Sample.tMillis] is a Long, so this model cannot represent a non-finite timestamp.
 */
enum class IngressReason {
    StaleEpoch,
    SourceMismatch,
    ChannelOutOfRange,
    NonFiniteValue,
}

data class IngressTally(
    val accepted: Int = 0,
    val rejected: Map<IngressReason, Int> = emptyMap(),
) {
    val rejectedCount: Int get() = rejected.values.sum()
}

object SampleIngress {
    fun decide(epoch: Long, sourceKey: String, channelCount: Int, sample: Sample): IngressReason? {
        if (sample.epoch != epoch) return IngressReason.StaleEpoch
        if (sample.sourceKey != sourceKey) return IngressReason.SourceMismatch
        if (sample.channel !in 0 until channelCount) return IngressReason.ChannelOutOfRange
        if (!sample.value.isFinite()) return IngressReason.NonFiniteValue
        return null
    }

    /**
     * Points the chart can scale. Non-finite values are omitted and the next
     * finite point is drawn as a gap, so min/max stay finite.
     */
    fun drawable(samples: List<Sample>): List<Sample> {
        val out = ArrayList<Sample>(samples.size)
        var breakNext = false
        for (sample in samples) {
            if (!sample.value.isFinite()) {
                breakNext = true
                continue
            }
            out.add(if (breakNext) sample.copy(gapBefore = true) else sample)
            breakNext = false
        }
        return out
    }

    fun span(samples: List<Sample>): Pair<Float, Float>? {
        val finite = drawable(samples)
        if (finite.isEmpty()) return null
        return finite.minOf { it.value } to finite.maxOf { it.value }
    }
}

fun formatDemoValue(value: Float): String {
    if (!value.isFinite()) return "无效"
    val rounded = (value * 10f).toInt() / 10f
    return if (rounded == 0f) "0" else rounded.toString()
}
