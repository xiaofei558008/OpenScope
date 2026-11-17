package app.openscope.androiddemo.domain

import kotlin.math.sin

class ChannelRing(val capacity: Int) {
    private val data = ArrayDeque<Sample>()
    private var gapArmed = false

    fun append(sample: Sample) {
        val flagged = if (gapArmed || sample.gapBefore) {
            sample.copy(gapBefore = true)
        } else {
            sample
        }
        gapArmed = false
        data.addLast(flagged)
        while (data.size > capacity) data.removeFirst()
    }

    fun armGap() {
        gapArmed = true
    }

    fun clear() {
        data.clear()
        gapArmed = false
    }

    fun snapshot(): List<Sample> = data.toList()

    fun size(): Int = data.size
}

class SampleRings(
    channels: Int = DemoOnlySettings.MAX_CHANNELS,
    capacity: Int = DemoOnlySettings.RING_CAPACITY,
) {
    val rings: List<ChannelRing> = List(channels) { ChannelRing(capacity) }
    var seq: Long = 0L
        private set
    var epoch: Long = 1L
        private set
    var sourceKey: String = ""
        private set
    var enabled: Boolean = false
    var paused: Boolean = false

    fun retarget(epoch: Long, sourceKey: String) {
        this.epoch = epoch
        this.sourceKey = sourceKey
        seq = 0L
        rings.forEach {
            it.clear()
            it.armGap()
        }
    }

    fun offer(sample: Sample): IngressReason? {
        val reason = SampleIngress.decide(epoch, sourceKey, rings.size, sample)
        if (reason != null) return reason
        rings[sample.channel].append(sample)
        return null
    }

    fun offerBatch(samples: List<Sample>): IngressTally {
        var accepted = 0
        val rejected = mutableMapOf<IngressReason, Int>()
        for (sample in samples) {
            val reason = offer(sample)
            if (reason == null) {
                accepted += 1
            } else {
                rejected[reason] = (rejected[reason] ?: 0) + 1
            }
        }
        return IngressTally(accepted, rejected)
    }

    fun clearAndKeepSource() {
        rings.forEach { it.clear() }
        seq = 0L
    }

    fun armGap() {
        rings.forEach { it.armGap() }
    }

    fun tick(now: Long): Boolean {
        if (!enabled || paused) return false
        seq += 1
        rings.forEachIndexed { channel, ring ->
            if (channel == 1 && seq % 48L == 0L) ring.armGap()
            val decision = offer(
                Sample(
                    seq = seq,
                    tMillis = now,
                    channel = channel,
                    value = synth(channel, seq, epoch),
                    gapBefore = false,
                    epoch = epoch,
                    sourceKey = sourceKey,
                ),
            )
            check(decision == null)
        }
        return true
    }

    companion object {
        fun synth(channel: Int, seq: Long, epoch: Long): Float {
            val salt = epoch.toFloat()
            val t = seq.toFloat()
            return when (channel) {
                0 -> 60f + 40f * sin(t / 8f) + salt
                1 -> if ((seq / 10L) % 2L == 0L) 800f else 2400f
                2 -> 40f + (seq % 50L) + salt
                else -> 0f
            }
        }
    }
}
