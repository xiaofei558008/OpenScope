package app.openscope.androiddemo.domain

import org.junit.Assert.assertEquals
import org.junit.Assert.assertFalse
import org.junit.Assert.assertNull
import org.junit.Test

class PairingBookTest {
    @Test
    fun otherClaimantDoesNotLearnTheApproval() {
        val book = PairingBook()
        val verified = book.verify(book.parse("demo-ok", ConnectionMode.LanDirect))
        book.apply(verified, epoch = 1)
        val binding = book.record(verified.requestId)!!.binding
        val pending = book.recover(binding.requestId, "other-handset", binding)
        assertEquals("PAIRING_RACE_REJECTED", pending.code)
        assertNull(pending.approval)
        assertFalse(pending.disclosesOtherClaimant)
        book.decide(binding.requestId, epoch = 1, PcDecision.Approve)
        val finished = book.recover(binding.requestId, "other-handset", binding)
        assertEquals("PAIRING_CODE_CONSUMED", finished.code)
        assertNull(finished.approval)
        assertFalse(finished.disclosesOtherClaimant)
    }

    @Test
    fun changedBindingIsNotRecovery() {
        val book = PairingBook()
        val verified = book.verify(book.parse("demo-ok", ConnectionMode.OfficialManaged))
        book.apply(verified, epoch = 3)
        val binding = book.record(verified.requestId)!!.binding
        val changed = binding.copy(verifiedPc = "已验证：PC-2")
        val attempt = book.recover(binding.requestId, binding.claimantId, changed)
        assertEquals("PAIRING_KEY_MISMATCH", attempt.code)
        assertEquals(1, book.record(verified.requestId)!!.redemptionCount)
        assertFalse(attempt.ttlExtended)
        assertFalse(attempt.reconsumed)
    }

    @Test
    fun lateApproveCannotOverturnDenial() {
        val book = PairingBook()
        val verified = book.verify(book.parse("demo-ok", ConnectionMode.LanDirect))
        book.apply(verified, epoch = 1)
        book.decide(verified.requestId, 1, PcDecision.Deny)
        val late = book.decide(verified.requestId, 1, PcDecision.Approve)
        assertEquals("LATE_CONFIRM_IGNORED", late.code)
        assertEquals(Approval.Denied, book.record(verified.requestId)!!.approval)
        assertEquals(0, book.grantCreations(verified.requestId))
    }

    @Test
    fun selfApproveLeavesPendingUntouched() {
        val book = PairingBook()
        val verified = book.verify(book.parse("demo-ok", ConnectionMode.SelfHosted))
        book.apply(verified, epoch = 2)
        val attempt = book.selfApprove(verified.requestId)
        assertEquals("PAIRING_CONFIRM_REQUIRED", attempt.code)
        assertFalse(attempt.selfApproved)
        assertEquals(Approval.Pending, book.record(verified.requestId)!!.approval)
        assertFalse(book.record(verified.requestId)!!.grantEverCreated)
    }
}
