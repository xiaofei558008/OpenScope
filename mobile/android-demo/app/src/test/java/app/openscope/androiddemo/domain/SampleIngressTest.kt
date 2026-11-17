package app.openscope.androiddemo.domain

import kotlin.random.Random
import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertNull
import org.junit.Assert.assertTrue
import org.junit.Test

class SampleIngressTest {
    @Test
    fun staleEpochOrSourceNeverEntersTheVisibleRing() {
        val rings = open(epoch = 4L, source = "src-live", capacity = 8)
        rings.offer(sample(epoch = 4L, source = "src-live", seq = 1, value = 1f))
        val before = rings.rings[0].snapshot()

        assertEquals(IngressReason.StaleEpoch, rings.offer(sample(epoch = 3L, source = "src-live", seq = 2, value = 9f)))
        assertEquals(IngressReason.SourceMismatch, rings.offer(sample(epoch = 4L, source = "src-old", seq = 3, value = 8f)))
        assertEquals(IngressReason.ChannelOutOfRange, rings.offer(sample(epoch = 4L, source = "src-live", seq = 4, value = 7f, channel = 9)))

        assertEquals(before, rings.rings[0].snapshot())
        assertTrue(rings.rings.drop(1).all { it.snapshot().isEmpty() })
    }

    @Test
    fun mixedBatchKeepsOnlyLatestAcceptedSamplesAndStaysBounded() {
        val capacity = 4
        val rings = open(epoch = 2L, source = "src-live", capacity = capacity)
        val batch = mutableListOf<Sample>()
        repeat(3) { batch += sample(epoch = 1L, source = "src-live", seq = it.toLong(), value = 100f + it) }
        repeat(2) { batch += sample(epoch = 2L, source = "other", seq = 10L + it, value = 50f) }
        batch += sample(epoch = 2L, source = "src-live", seq = 20, value = Float.NaN)
        batch += sample(epoch = 2L, source = "src-live", seq = 21, value = Float.POSITIVE_INFINITY)
        batch += sample(epoch = 2L, source = "src-live", seq = 22, value = Float.NEGATIVE_INFINITY)
        repeat(6) { index ->
            batch += sample(epoch = 2L, source = "src-live", seq = 30L + index, value = index.toFloat(), channel = 0)
        }
        val tally = rings.offerBatch(batch)
        val visible = rings.rings[0].snapshot()

        assertEquals(6, tally.accepted)
        assertEquals(3, tally.rejected[IngressReason.StaleEpoch])
        assertEquals(2, tally.rejected[IngressReason.SourceMismatch])
        assertEquals(3, tally.rejected[IngressReason.NonFiniteValue])
        assertEquals(capacity, visible.size)
        assertEquals(listOf(2f, 3f, 4f, 5f), visible.map { it.value })
        visible.forEach {
            assertEquals(2L, it.epoch)
            assertEquals("src-live", it.sourceKey)
            assertTrue(it.value.isFinite())
        }
        assertTrue(rings.rings.all { it.size() <= capacity })
    }

    @Test
    fun sourceChangeClearsStaleSamplesAndGapsTheNextPoint() {
        val rings = open(epoch = 1L, source = "src-a", capacity = 8)
        rings.offer(sample(epoch = 1L, source = "src-a", seq = 7, value = 3f))
        rings.retarget(2L, "src-b")
        assertTrue(rings.rings.all { it.snapshot().isEmpty() })

        val late = rings.offerBatch(
            listOf(
                sample(epoch = 1L, source = "src-a", seq = 8, value = 99f),
                sample(epoch = 2L, source = "src-a", seq = 9, value = 98f),
            ),
        )
        assertEquals(0, late.accepted)
        assertTrue(rings.rings[0].snapshot().isEmpty())

        assertNull(rings.offer(sample(epoch = 2L, source = "src-b", seq = 1, value = 4f)))
        val first = rings.rings[0].snapshot().single()
        assertTrue(first.gapBefore)
        assertEquals(2L, first.epoch)
        assertEquals("src-b", first.sourceKey)
        assertEquals(4f, first.value)
    }

    @Test
    fun nonFiniteValuesDoNotCrashScalingAndAreNotShownAsZero() {
        val poisoned = listOf(
            sample(epoch = 1L, source = "s", seq = 1, value = 2f),
            sample(epoch = 1L, source = "s", seq = 2, value = Float.NaN),
            sample(epoch = 1L, source = "s", seq = 3, value = Float.POSITIVE_INFINITY),
            sample(epoch = 1L, source = "s", seq = 4, value = 6f),
        )
        val drawable = SampleIngress.drawable(poisoned)
        val span = SampleIngress.span(poisoned)
        assertEquals(listOf(2f, 6f), drawable.map { it.value })
        assertTrue(drawable[1].gapBefore)
        assertEquals(2f to 6f, span)
        assertEquals("无效", formatDemoValue(Float.NaN))
        assertEquals("无效", formatDemoValue(Float.NEGATIVE_INFINITY))
        assertEquals("0", formatDemoValue(0f))
        assertNull(SampleIngress.span(emptyList()))
    }

    @Test
    fun longTimestampsStayInTheModelWithoutANonFiniteForm() {
        val rings = open(epoch = 5L, source = "src-live", capacity = 4)
        assertNull(rings.offer(sample(epoch = 5L, source = "src-live", seq = 1, value = 1f, tMillis = Long.MIN_VALUE)))
        assertNull(rings.offer(sample(epoch = 5L, source = "src-live", seq = 2, value = 2f, tMillis = Long.MAX_VALUE)))
        val stored = rings.rings[0].snapshot()
        assertEquals(listOf(Long.MIN_VALUE, Long.MAX_VALUE), stored.map { it.tMillis })
        assertEquals(1f to 2f, SampleIngress.span(stored))
    }

    @Test
    fun stressKeepsEveryVisiblePointOnTheCurrentEpochAndSource() {
        val rings = open(epoch = 9L, source = "src-live", capacity = 16)
        val random = Random(20261117)
        var accepted = 0
        repeat(2_000) { index ->
            val sample = when (index % 7) {
                0 -> sample(epoch = 8L, source = "src-live", seq = index.toLong(), value = random.nextFloat())
                1 -> sample(epoch = 9L, source = "src-other", seq = index.toLong(), value = random.nextFloat())
                2 -> sample(epoch = 9L, source = "src-live", seq = index.toLong(), value = Float.NaN)
                3 -> sample(epoch = 9L, source = "src-live", seq = index.toLong(), value = Float.POSITIVE_INFINITY, channel = 1)
                4 -> sample(epoch = 9L, source = "src-live", seq = index.toLong(), value = random.nextFloat(), channel = 99)
                else -> sample(
                    epoch = 9L,
                    source = "src-live",
                    seq = index.toLong(),
                    value = random.nextFloat(),
                    channel = index % DemoOnlySettings.MAX_CHANNELS,
                    tMillis = if (index % 2 == 0) Long.MIN_VALUE else index.toLong(),
                )
            }
            if (rings.offer(sample) == null) accepted += 1
            assertTrue(rings.rings.all { it.size() <= 16 })
            rings.rings.forEach { ring ->
                ring.snapshot().forEach {
                    assertEquals(9L, it.epoch)
                    assertEquals("src-live", it.sourceKey)
                    assertTrue(it.value.isFinite())
                    assertTrue(it.channel in 0 until DemoOnlySettings.MAX_CHANNELS)
                }
            }
        }
        assertTrue(accepted > 100)
        val other = open(epoch = 1L, source = "untouched", capacity = 16)
        other.offer(sample(epoch = 1L, source = "untouched", seq = 1, value = 42f))
        assertEquals(listOf(42f), other.rings[0].snapshot().map { it.value })
        assertTrue(rings.rings[0].snapshot().none { it.value == 42f && it.sourceKey == "untouched" })
    }

    @Test
    fun oversizeBatchCannotGrowPastDemoCapacity() {
        val capacity = DemoOnlySettings.RING_CAPACITY
        val rings = open(epoch = 1L, source = "src-live", capacity = capacity)
        val count = capacity * 8
        val batch = List(count) { index ->
            sample(epoch = 1L, source = "src-live", seq = index.toLong(), value = index.toFloat(), channel = index % 4)
        }
        val tally = rings.offerBatch(batch)
        assertEquals(batch.size, tally.accepted)
        assertEquals(0, tally.rejectedCount)
        rings.rings.forEach { ring ->
            assertEquals(capacity, ring.size())
        }
        assertEquals((count - 4).toFloat(), rings.rings[0].snapshot().last().value)
    }

    @Test
    fun sessionSourceChangeDoesNotSwitchModeOrAuthorizeControl() {
        val session = DemoSession()
        session.parseInput("demo-ok")
        session.verify()
        session.applyPairing()
        session.pcDecide(PcDecision.Approve)
        session.injectReceipt(CloudRaw.Running)
        val mode = session.snapshot().mode
        session.onGeneratorTick()
        session.requestReconnect()
        assertTrue(session.samples(0).isEmpty())
        session.recoverSame()
        session.resumeViewing()
        session.onGeneratorTick()
        val snap = session.snapshot()
        assertEquals(mode, snap.mode)
        assertFalse(snap.controlExecutable)
        assertFalse(snap.silentCloudSwitch)
        assertEquals(CloudRaw.Running, snap.commands.single().raw)
        assertEquals(0, snap.replayCount)
        assertEquals(0, snap.controlQueryCount)
        assertTrue(session.samples(0).single().gapBefore)
        assertEquals(snap.epoch, session.samples(0).single().epoch)
    }

    private fun open(epoch: Long, source: String, capacity: Int): SampleRings {
        return SampleRings(channels = DemoOnlySettings.MAX_CHANNELS, capacity = capacity).also {
            it.retarget(epoch, source)
        }
    }

    private fun sample(
        epoch: Long,
        source: String,
        seq: Long,
        value: Float,
        channel: Int = 0,
        tMillis: Long = seq,
    ) = Sample(
        seq = seq,
        tMillis = tMillis,
        channel = channel,
        value = value,
        gapBefore = false,
        epoch = epoch,
        sourceKey = source,
    )
}
