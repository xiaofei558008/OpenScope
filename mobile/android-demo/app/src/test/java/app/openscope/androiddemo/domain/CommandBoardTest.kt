package app.openscope.androiddemo.domain

import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertNull
import org.junit.Assert.assertTrue
import org.junit.Test

class CommandBoardTest {
    @Test
    fun sevenRawStatesKeepFiveGroupsAndSublabels() {
        val expected = mapOf(
            CloudRaw.Rejected to ("拒绝" to "拒绝"),
            CloudRaw.Received to ("进行中" to "已收到"),
            CloudRaw.Accepted to ("进行中" to "已接受"),
            CloudRaw.Running to ("进行中" to "执行中"),
            CloudRaw.Succeeded to ("成功" to "成功"),
            CloudRaw.Failed to ("确认失败" to "确认失败"),
            CloudRaw.Unknown to ("未知" to "未知"),
        )
        assertEquals(7, CloudRaw.entries.size)
        assertEquals(7, expected.size)
        val board = CommandBoard()
        CloudRaw.entries.forEach { raw ->
            val view = board.inject(raw).toView()
            val (group, sub) = expected.getValue(raw)
            assertEquals(raw, view.raw)
            assertTrue(view.rawLabel.contains(DemoCopy.raw(raw)))
            assertTrue(view.groupLabel.contains(group))
            assertEquals(sub, view.sublabel)
            assertTrue(view.localLabel.contains("已提交"))
            assertFalse(view.executed)
            assertEquals(PcCommandConfirm.Waiting, view.pcConfirm)
        }
        assertEquals(0, board.controlQueryCount)
        assertEquals(0, board.replayCount)
    }

    @Test
    fun disconnectMarksLocalUnknownWithoutRewritingRawOrReplaying() {
        val board = CommandBoard()
        val running = board.inject(CloudRaw.Running)
        board.setPcConfirm(running.id, PcCommandConfirm.Confirmed)
        board.onDisconnected()
        val view = board.views().single { it.id == running.id }
        assertEquals(CloudRaw.Running, view.raw)
        assertTrue(view.groupLabel.contains("未知"))
        assertTrue(view.sublabel.contains("running"))
        assertTrue(view.sublabel.contains("已收到").not())
        assertTrue(view.localLabel.contains("断线后未知"))
        assertEquals(PcCommandConfirm.Confirmed, view.pcConfirm)
        assertEquals(0, board.replayCount)
        assertEquals(0, board.controlQueryCount)
    }

    @Test
    fun localValidationNeverCreatesACloudRawState() {
        val board = CommandBoard()
        val view = board.localReject("未送出").toView()
        assertNull(view.raw)
        assertTrue(view.groupLabel.contains("本地态"))
        assertFalse(view.executed)
        board.onDisconnected()
        assertNull(board.views().single().raw)
    }
}
