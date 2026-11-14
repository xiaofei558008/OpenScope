package app.openscope.androiddemo.domain

import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertNotEquals
import org.junit.Assert.assertNull
import org.junit.Assert.assertTrue
import org.junit.Test

class DemoSessionTest {
    @Test
    fun lanApproveDefaultsToViewerAndDoesNotExecuteControl() {
        val session = DemoSession()
        approve(session, "demo-ok")
        val snap = session.snapshot()
        assertEquals(Approval.ApprovedViewer, snap.storedApproval)
        assertTrue(snap.viewingLive)
        assertEquals(1, snap.redemptionCount)
        assertTrue(snap.grantCreatedOnLastAttempt)
        assertFalse(snap.ttlExtended)
        assertFalse(snap.reconsumed)
        assertFalse(snap.controlExecutable)
        assertEquals(ConnectionMode.LanDirect, snap.mode)
        assertTrue(snap.banner.contains("DEMO"))
        assertTrue(snap.banner.contains("模拟数据"))
    }

    @Test
    fun repeatOfSameApprovedRequestDoesNotCreateAnotherGrant() {
        val session = DemoSession()
        approve(session, "demo-ok")
        val id = session.bindingForTest()!!.requestId
        session.applyPairing()
        val snap = session.snapshot()
        assertEquals("RECOVERED_CURRENT", snap.lastCode)
        assertEquals(Approval.ApprovedViewer, snap.storedApproval)
        assertEquals(1, snap.redemptionCount)
        assertFalse(snap.grantCreatedOnLastAttempt)
        assertFalse(snap.ttlExtended)
        assertFalse(snap.reconsumed)
        assertEquals(1, session.grantCreationsForTest(id))
    }

    @Test
    fun deniedApprovalIsWhatARepeatReturns() {
        val session = DemoSession()
        session.parseInput("demo-deny")
        session.verify()
        session.applyPairing()
        session.pcDecide(PcDecision.Deny)
        session.applyPairing()
        val snap = session.snapshot()
        assertEquals("RECOVERED_CURRENT", snap.lastCode)
        assertEquals(Approval.Denied, snap.storedApproval)
        assertEquals(1, snap.redemptionCount)
        assertFalse(snap.grantCreatedOnLastAttempt)
        assertFalse(snap.viewingLive)
        session.pcDecide(PcDecision.Approve)
        assertEquals(Approval.Denied, session.snapshot().storedApproval)
        assertEquals("DECISION_HELD", session.snapshot().lastCode)
    }

    @Test
    fun cancelAndTimeoutGrantNothing() {
        val cancelled = DemoSession()
        cancelled.parseInput("demo-cancel")
        cancelled.verify()
        cancelled.applyPairing()
        cancelled.pcDecide(PcDecision.Cancel)
        assertEquals(Approval.Cancelled, cancelled.snapshot().storedApproval)
        assertFalse(cancelled.snapshot().viewingLive)

        val timedOut = DemoSession()
        timedOut.parseInput("demo-timeout")
        timedOut.verify()
        timedOut.applyPairing()
        timedOut.pcDecide(PcDecision.Timeout)
        assertEquals(Approval.TimedOut, timedOut.snapshot().storedApproval)
        assertFalse(timedOut.snapshot().grantCreatedOnLastAttempt)
    }

    @Test
    fun claimedAndVerifiedStaySeparateUntilVerify() {
        val session = DemoSession()
        session.parseInput("demo-mismatch")
        val claimed = session.snapshot().preview!!
        assertFalse(claimed.verified)
        assertNull(claimed.verifiedPc)
        assertTrue(claimed.declaredPc.contains("PC-其他"))
        session.verify()
        val verified = session.snapshot().preview!!
        assertTrue(verified.verified)
        assertTrue(verified.declaredPc.contains("PC-其他"))
        assertEquals(DemoScenarios.VERIFIED_PC, verified.verifiedPc)
        session.applyPairing()
        assertEquals("ENDPOINT_CONFIRM_REQUIRED", session.snapshot().lastCode)
        session.acknowledgeVerified()
        session.applyPairing()
        assertEquals(Approval.Pending, session.snapshot().storedApproval)
    }

    @Test
    fun phoneCannotApproveItself() {
        val session = DemoSession()
        session.parseInput("demo-ok")
        session.verify()
        session.applyPairing()
        session.selfApprove()
        val snap = session.snapshot()
        assertEquals(Approval.Pending, snap.storedApproval)
        assertEquals("PAIRING_CONFIRM_REQUIRED", snap.lastCode)
        assertFalse(snap.selfApproved)
        assertFalse(snap.viewingLive)
    }

    @Test
    fun expiredAndRevokedInputsDoNotFabricatePending() {
        val expired = DemoSession()
        expired.parseInput("demo-expired")
        expired.verify()
        expired.applyPairing()
        expired.applyPairing()
        assertEquals(Approval.Expired, expired.snapshot().storedApproval)
        assertEquals(0, expired.snapshot().redemptionCount)
        assertFalse(expired.snapshot().ttlExtended)
        assertNotEquals(Approval.Pending, expired.snapshot().storedApproval)

        val revoked = DemoSession()
        revoked.parseInput("demo-revoked")
        revoked.verify()
        revoked.applyPairing()
        assertEquals("DEVICE_REVOKED", revoked.snapshot().lastCode)
        revoked.recoverSame()
        assertEquals(Approval.Revoked, revoked.snapshot().storedApproval)
        assertNotEquals(Approval.Pending, revoked.snapshot().storedApproval)
    }

    @Test
    fun maliciousInputIsNotOpened() {
        val session = DemoSession()
        session.parseInput("javascript:alert(1)")
        assertTrue(session.snapshot().preview!!.rejected)
        session.parseInput("javascript%3aalert(1)")
        assertEquals("MALICIOUS", session.snapshot().lastCode)
        session.setSelfHostAddress("http://169.254.169.254/latest")
        val snap = session.snapshot()
        assertEquals("", snap.selfHostAddress)
        assertEquals(0, snap.fetchCount)
        assertTrue(snap.openedUrls.isEmpty())
        assertFalse(snap.plaintextFallback)
        assertFalse(snap.adminCredentialFields)
    }

    @Test
    fun savedDeviceDoesNotRestoreAuthAndLocalDeleteIsNotRevoke() {
        val session = DemoSession()
        approve(session, "demo-ok")
        session.rememberDevice("车间平板")
        val saved = session.exportSaved().single()
        assertFalse(saved.label.contains("token"))
        session.selectSaved(saved.localId)
        val selected = session.snapshot()
        assertFalse(selected.viewingLive)
        assertFalse(selected.savedRestoresAuth)
        assertEquals(false, selected.preview!!.verified)
        session.deleteSavedLocal(saved.localId)
        assertTrue(session.exportSaved().isEmpty())
        assertFalse(session.snapshot().localDeleteIsServerRevoke)
        assertFalse(session.snapshot().serverRevoked)
        assertEquals(Approval.ApprovedViewer, session.snapshot().storedApproval)
    }

    @Test
    fun serverRevokeBlocksResumeAndDoesNotReplay() {
        val session = DemoSession()
        approve(session, "demo-ok")
        session.injectReceipt(CloudRaw.Running)
        session.revokeServer()
        val snap = session.snapshot()
        assertTrue(snap.serverRevoked)
        assertFalse(snap.viewingLive)
        assertEquals(0, snap.replayCount)
        assertEquals(CloudRaw.Running, snap.commands.single().raw)
        session.resumeViewing()
        assertFalse(session.snapshot().viewingLive)
        session.recoverSame()
        assertEquals("DEVICE_REVOKED", session.snapshot().lastCode)
        assertNotEquals(Approval.Pending, session.snapshot().storedApproval)
    }

    @Test
    fun lanFailureStaysOnLanAndClearsSyntheticSamples() {
        val session = DemoSession()
        approve(session, "demo-ok")
        session.onGeneratorTick()
        session.publishUi()
        assertTrue(session.publishedSamples(0).isNotEmpty())
        session.injectLanFailure()
        val snap = session.snapshot()
        assertEquals(ConnectionMode.LanDirect, snap.mode)
        assertFalse(snap.silentCloudSwitch)
        assertEquals(TransportState.Failed, snap.transport)
        assertTrue(snap.failureNote!!.contains("没有改为官方托管云"))
        assertTrue(session.publishedSamples(0).isEmpty())
        assertFalse(snap.viewingLive)
        session.continueCurrentMode()
        assertEquals(ConnectionMode.LanDirect, session.snapshot().mode)
        assertTrue(session.snapshot().viewingLive)
    }

    @Test
    fun certificateFailureDoesNotFallBackToPlaintext() {
        val session = DemoSession()
        session.requestMode(ConnectionMode.SelfHosted)
        session.confirmModeSwitch()
        session.injectCertFailure()
        val snap = session.snapshot()
        assertEquals(ConnectionMode.SelfHosted, snap.mode)
        assertEquals(TransportState.Failed, snap.transport)
        assertFalse(snap.plaintextFallback)
        assertFalse(snap.adminCredentialFields)
        assertFalse(snap.silentCloudSwitch)
    }

    @Test
    fun modeSwitchRequiresConfirmAndDropsOldSamples() {
        val session = DemoSession()
        approve(session, "demo-ok")
        session.onGeneratorTick()
        session.publishUi()
        session.requestMode(ConnectionMode.OfficialManaged)
        session.cancelModeSwitch()
        assertEquals(ConnectionMode.LanDirect, session.snapshot().mode)
        session.requestMode(ConnectionMode.OfficialManaged)
        session.confirmModeSwitch()
        val snap = session.snapshot()
        assertEquals(ConnectionMode.OfficialManaged, snap.mode)
        assertNull(snap.preview)
        assertTrue(session.samples(0).isEmpty())
        assertFalse(snap.viewingLive)
        assertTrue(snap.epoch > 1)
    }

    @Test
    fun officialEntitlementIgnoresPaymentReturn() {
        val session = DemoSession()
        session.requestMode(ConnectionMode.OfficialManaged)
        session.confirmModeSwitch()
        approve(session, "demo-ok")
        assertFalse(session.snapshot().viewingLive)
        assertEquals(EntitlementPlaceholder.Unpurchased, session.snapshot().entitlement.entitlement)
        session.observePaymentReturn()
        assertTrue(session.snapshot().entitlement.returnObserved)
        assertEquals(EntitlementPlaceholder.Unpurchased, session.snapshot().entitlement.entitlement)
        assertFalse(session.snapshot().viewingLive)
        session.serverOrder(OrderPlaceholder.Active)
        assertEquals(EntitlementPlaceholder.Unpurchased, session.snapshot().entitlement.entitlement)
        session.serverEntitlement(EntitlementPlaceholder.Active)
        assertTrue(session.snapshot().viewingLive)
        session.onGeneratorTick()
        session.publishUi()
        assertTrue(session.publishedSamples(0).isNotEmpty())
        session.serverEntitlement(EntitlementPlaceholder.Exhausted)
        assertTrue(session.publishedSamples(0).isEmpty())
        assertFalse(session.snapshot().viewingLive)
    }

    @Test
    fun lanViewingSurvivesEntitlementPlaceholder() {
        val session = DemoSession()
        approve(session, "demo-ok")
        session.onGeneratorTick()
        session.publishUi()
        val size = session.publishedSamples(0).size
        session.serverEntitlement(EntitlementPlaceholder.Exhausted)
        assertTrue(session.snapshot().viewingLive)
        assertEquals(size, session.publishedSamples(0).size)
    }

    @Test
    fun disconnectKeepsRawCommandAndDoesNotReplay() {
        val session = DemoSession()
        approve(session, "demo-ok")
        session.injectReceipt(CloudRaw.Running)
        session.setPcCommandConfirm("cmd-1", PcCommandConfirm.Confirmed)
        val before = session.snapshot().commands.single()
        assertEquals(CloudRaw.Running, before.raw)
        assertTrue(before.groupLabel.contains("进行中"))
        assertTrue(before.pcLabel.contains("已确认"))
        session.disconnect()
        val after = session.snapshot().commands.single()
        assertEquals(CloudRaw.Running, after.raw)
        assertTrue(after.groupLabel.contains("未知"))
        assertTrue(after.localLabel.contains("断线后未知"))
        assertTrue(after.sublabel.contains("running"))
        assertFalse(after.executed)
        assertEquals(0, session.snapshot().replayCount)
        assertEquals(0, session.snapshot().controlQueryCount)
        assertFalse(session.snapshot().viewingLive)
    }

    @Test
    fun terminalRawIsNotRewrittenAndLocalRejectIsNotACloudState() {
        val session = DemoSession()
        session.injectReceipt(CloudRaw.Succeeded)
        session.disconnect()
        val done = session.snapshot().commands.single()
        assertEquals(CloudRaw.Succeeded, done.raw)
        assertTrue(done.groupLabel.contains("成功"))
        assertTrue(done.localLabel.contains("已提交"))

        session.submitControlAttempt()
        val local = session.snapshot().commands.last()
        assertNull(local.raw)
        assertTrue(local.groupLabel.contains("本地态"))
        assertFalse(local.executed)
    }

    @Test
    fun ringIsBoundedAndUiPublishIsSeparateFromGeneration() {
        val session = DemoSession()
        approve(session, "demo-ok")
        val publishesAfterApprove = session.snapshot().uiPublishes
        repeat(300) { session.onGeneratorTick() }
        assertEquals(DemoOnlySettings.RING_CAPACITY, session.samples(0).size)
        assertEquals(61L, session.samples(0).first().seq)
        assertEquals(publishesAfterApprove, session.snapshot().uiPublishes)
        assertTrue(session.publishedSamples(0).isEmpty())
        session.publishUi()
        assertEquals(DemoOnlySettings.RING_CAPACITY, session.publishedSamples(0).size)
        assertTrue(session.snapshot().uiPublishes > publishesAfterApprove)
    }

    @Test
    fun gapsEpochChangesAndFreezeDoNotPauseGeneration() {
        val session = DemoSession()
        approve(session, "demo-ok")
        repeat(48) { session.onGeneratorTick() }
        assertTrue(session.samples(1).last().gapBefore)
        assertFalse(session.samples(0).last().gapBefore)
        session.publishUi()
        session.toggleFreeze()
        val frozenSeq = session.snapshot().waves.channels[0].last().seq
        session.onGeneratorTick()
        session.publishUi()
        assertEquals(frozenSeq, session.snapshot().waves.channels[0].last().seq)
        assertTrue(session.snapshot().waves.frozen)
        assertFalse(session.snapshot().backgroundPaused)
        assertTrue(session.snapshot().waves.liveCount > session.snapshot().waves.channels.sumOf { it.size })
        val oldEpoch = session.snapshot().epoch
        session.requestReconnect()
        assertTrue(session.samples(0).isEmpty())
        assertTrue(session.snapshot().epoch > oldEpoch)
        assertTrue(session.snapshot().sourceChangeNote!!.contains("旧代数"))
        assertFalse(session.snapshot().viewingLive)
    }

    @Test
    fun backgroundPausesAndForegroundRevalidatesWithoutReplay() {
        val session = DemoSession()
        approve(session, "demo-ok")
        session.injectReceipt(CloudRaw.Accepted)
        session.onGeneratorTick()
        session.onBackground()
        val size = session.samples(0).size
        session.onGeneratorTick()
        assertEquals(size, session.samples(0).size)
        assertEquals(CloudRaw.Accepted, session.snapshot().commands.single().raw)
        assertTrue(session.snapshot().commands.single().localLabel.contains("已提交"))
        session.onForeground()
        session.onGeneratorTick()
        assertTrue(session.samples(0).last().gapBefore)
        assertEquals(0, session.snapshot().replayCount)
        assertTrue(session.snapshot().lifecycleNote!!.contains("缺口"))
    }

    @Test
    fun foregroundWithoutALiveSessionStaysPaused() {
        val session = DemoSession()
        session.onBackground()
        session.onForeground()
        assertTrue(session.snapshot().backgroundPaused)
        assertTrue(session.snapshot().lifecycleNote!!.contains("未通过"))
        session.onGeneratorTick()
        assertEquals(0, session.samples(0).size)
    }

    @Test
    fun reconnectRecoveryDoesNotMintAGrantAndOldEpochDecisionIsIgnored() {
        val session = DemoSession()
        session.parseInput("demo-ok")
        session.verify()
        session.applyPairing()
        val epoch = session.snapshot().epoch
        session.requestReconnect()
        assertTrue(session.snapshot().epoch > epoch)
        session.pcDecide(PcDecision.Approve)
        assertEquals("DECISION_HELD", session.snapshot().lastCode)
        assertEquals(Approval.Pending, session.snapshot().storedApproval)
        session.recoverSame()
        assertEquals("RECOVERED_CURRENT", session.snapshot().lastCode)
        assertEquals(1, session.snapshot().redemptionCount)
        session.pcDecide(PcDecision.Approve)
        assertEquals("STALE_EPOCH", session.snapshot().lastCode)
        assertEquals(Approval.Pending, session.snapshot().storedApproval)
        assertEquals(0, session.grantCreationsForTest(session.bindingForTest()!!.requestId))
    }

    @Test
    fun sameVariableNameDoesNotKeepOldEpochSamples() {
        val session = DemoSession()
        approve(session, "demo-ok")
        session.onGeneratorTick()
        val first = session.samples(0).single().value
        session.requestReconnect()
        session.recoverSame()
        session.resumeViewing()
        session.onGeneratorTick()
        val second = session.samples(0).single()
        assertEquals(1, session.samples(0).size)
        assertNotEquals(first, second.value)
        assertEquals(session.snapshot().epoch, second.epoch)
    }

    @Test
    fun zeroSampleIsDistinctFromNoSample() {
        val session = DemoSession()
        approve(session, "demo-ok")
        val before = session.snapshot().numbers.first { it.name.endsWith("zero") }
        assertEquals("无样本", before.displayValue)
        session.onGeneratorTick()
        session.publishUi()
        val after = session.snapshot().numbers.first { it.name.endsWith("zero") }
        assertEquals("0", after.displayValue)
        assertTrue(after.quality.contains("不是无样本"))
    }

    @Test
    fun staleSearchCannotOverwriteTheCurrentQuery() {
        val session = DemoSession()
        approve(session, "demo-ok")
        val token = session.beginSearch("speed")
        session.finishSearch(token, session.snapshot().epoch)
        assertTrue(session.snapshot().project.rows.any { it.name.contains("speed") })
        session.demonstrateStaleSearch()
        assertTrue(session.snapshot().project.staleDropped >= 1)
        assertEquals("speed", session.snapshot().project.appliedQuery)
        session.setProjectEmpty(true)
        assertTrue(session.snapshot().project.empty)
        assertTrue(session.snapshot().project.rows.isEmpty())
    }

    @Test
    fun extraChannelIsRejected() {
        val session = DemoSession()
        session.rejectExtraSubscription()
        assertEquals(1, session.snapshot().waves.extraRejected)
        assertEquals(DemoOnlySettings.MAX_CHANNELS, session.snapshot().waves.channels.size)
    }

    @Test
    fun demoPeriodsAreSeparateFixtureSettings() {
        assertTrue(DemoOnlySettings.GENERATOR_PERIOD_MS != DemoOnlySettings.UI_REFRESH_PERIOD_MS)
        assertTrue(DemoOnlySettings.note.contains("不是产品性能指标"))
    }

    private fun approve(session: DemoSession, scenario: String) {
        session.parseInput(scenario)
        session.verify()
        session.applyPairing()
        session.pcDecide(PcDecision.Approve)
    }
}
