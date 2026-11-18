package app.openscope.androiddemo.domain

/**
 * In-process pairing records.
 *
 * Repeating the same verified claimant, request id, and binding returns the
 * current approval, including denied. That repeat does not redeem again,
 * create a grant, or extend a TTL. This is demo input, not a desktop ABI.
 */
class PairingBook {
    private val records = linkedMapOf<String, PairingRecord>()

    fun record(requestId: String): PairingRecord? = records[requestId]

    fun grantCreations(requestId: String): Int =
        if (records[requestId]?.grantEverCreated == true) 1 else 0

    fun parse(raw: String, mode: ConnectionMode, claimantId: String = DemoCopy.CLAIMANT): PairingPreview {
        val text = raw.trim()
        if (text.isEmpty()) {
            return rejected(text, mode, "UNKNOWN_INPUT")
        }
        if (looksMalicious(text)) {
            return rejected(text, mode, "MALICIOUS")
        }
        val item = DemoScenarios.find(text)
            ?: return rejected(text, mode, "UNKNOWN_INPUT")
        return PairingPreview(
            rawInput = text,
            scenarioId = item.id,
            requestId = DemoScenarios.requestId(item.id, mode),
            mode = mode,
            declaredServer = DemoScenarios.declaredServer(mode),
            declaredPc = DemoScenarios.declaredPc(item.mismatch),
            verified = false,
            verifiedServer = null,
            verifiedPc = null,
            mismatch = item.mismatch,
            acknowledged = false,
            expired = item.expired,
            revokedScenario = item.revoked,
            rejected = false,
            rejectCode = null,
        ).let { it.copy(requestId = DemoScenarios.requestId(item.id, mode)) }
            .also {
                // claimant is fixed on the binding at apply time; preview keeps the scenario only.
                check(claimantId.isNotEmpty())
            }
    }

    fun verify(preview: PairingPreview): PairingPreview {
        if (preview.rejected) return preview
        return preview.copy(
            verified = true,
            verifiedServer = DemoScenarios.verifiedServer(preview.mode),
            verifiedPc = DemoScenarios.VERIFIED_PC,
        )
    }

    fun apply(preview: PairingPreview, epoch: Long, claimantId: String = DemoCopy.CLAIMANT): Attempt {
        if (preview.rejected) return terminal(preview.rejectCode ?: "MALICIOUS", null)
        if (!preview.verified || preview.verifiedServer == null || preview.verifiedPc == null) {
            return terminal("NOT_VERIFIED", records[preview.requestId])
        }
        if (preview.mismatch && !preview.acknowledged) {
            return terminal("ENDPOINT_CONFIRM_REQUIRED", records[preview.requestId])
        }
        val binding = Binding(
            mode = preview.mode,
            claimantId = claimantId,
            requestId = preview.requestId,
            verifiedServer = preview.verifiedServer,
            verifiedPc = preview.verifiedPc,
        )
        val existing = records[preview.requestId]
        if (existing != null && existing.binding.fingerprint() != binding.fingerprint()) {
            return terminal("PAIRING_KEY_MISMATCH", existing).copy(approval = null)
        }
        if (preview.expired || existing?.approval == Approval.Expired) {
            val kept = (existing ?: newRecord(binding, Approval.Expired, 0, epoch))
                .copy(approval = Approval.Expired, role = null)
            records[preview.requestId] = kept
            return current(kept, "PAIRING_CODE_EXPIRED", ok = false, fabricatedPending = false)
        }
        if (preview.revokedScenario || existing?.approval == Approval.Revoked) {
            val kept = (existing ?: newRecord(binding, Approval.Revoked, 0, epoch))
                .copy(approval = Approval.Revoked, role = null)
            records[preview.requestId] = kept
            return current(kept, "DEVICE_REVOKED", ok = false, fabricatedPending = false)
        }
        if (existing != null) {
            return current(existing, "RECOVERED_CURRENT", ok = true, responseLossRetry = true)
        }
        val created = newRecord(binding, Approval.Pending, redemptionCount = 1, epoch = epoch)
        records[preview.requestId] = created
        return current(created, "APPLIED", ok = true, responseLossRetry = false)
    }

    fun recover(requestId: String, claimantId: String, binding: Binding): Attempt {
        val existing = records[requestId]
            ?: return terminal("RECOVERY_UNAVAILABLE", null).copy(fabricatedPending = false)
        if (claimantId != existing.binding.claimantId) {
            val code = if (existing.approval == Approval.Pending) {
                "PAIRING_RACE_REJECTED"
            } else {
                "PAIRING_CODE_CONSUMED"
            }
            return terminal(code, existing).copy(
                approval = null,
                role = null,
                disclosesOtherClaimant = false,
            )
        }
        if (binding.fingerprint() != existing.binding.fingerprint()) {
            return terminal("PAIRING_KEY_MISMATCH", existing).copy(approval = null, role = null)
        }
        if (existing.approval == Approval.Revoked) {
            return current(existing, "DEVICE_REVOKED", ok = false, fabricatedPending = false)
        }
        if (existing.approval == Approval.Expired) {
            return current(existing, "PAIRING_CODE_EXPIRED", ok = false, fabricatedPending = false)
        }
        return current(existing, "RECOVERED_CURRENT", ok = true, responseLossRetry = true)
    }

    fun decide(requestId: String, epoch: Long, decision: PcDecision): Attempt {
        val existing = records[requestId] ?: return terminal("RECOVERY_UNAVAILABLE", null)
        if (epoch != existing.epochBound) {
            return current(existing, "STALE_EPOCH", ok = false)
        }
        if (existing.approval != Approval.Pending) {
            return current(existing, "LATE_CONFIRM_IGNORED", ok = false)
        }
        val updated = when (decision) {
            PcDecision.Approve -> existing.copy(
                approval = Approval.ApprovedViewer,
                role = Role.Viewer,
                grantEverCreated = true,
            )
            PcDecision.Deny -> existing.copy(approval = Approval.Denied, role = null)
            PcDecision.Cancel -> existing.copy(approval = Approval.Cancelled, role = null)
            PcDecision.Timeout -> existing.copy(approval = Approval.TimedOut, role = null)
        }
        records[requestId] = updated
        val code = when (decision) {
            PcDecision.Approve -> "PC_APPROVED"
            PcDecision.Deny -> "PC_DENIED"
            PcDecision.Cancel -> "PC_CANCELLED"
            PcDecision.Timeout -> "PC_TIMED_OUT"
        }
        return current(
            updated,
            code,
            ok = decision == PcDecision.Approve,
            grantCreated = decision == PcDecision.Approve,
        )
    }

    fun selfApprove(requestId: String): Attempt {
        val existing = records[requestId]
        return terminal("PAIRING_CONFIRM_REQUIRED", existing).copy(selfApproved = false)
    }

    fun revoke(requestId: String): Attempt {
        val existing = records[requestId] ?: return terminal("RECOVERY_UNAVAILABLE", null)
        val updated = existing.copy(approval = Approval.Revoked, role = null)
        records[requestId] = updated
        return current(updated, "DEVICE_REVOKED", ok = false, fabricatedPending = false)
    }

    private fun newRecord(
        binding: Binding,
        approval: Approval,
        redemptionCount: Int,
        epoch: Long,
    ) = PairingRecord(
        binding = binding,
        approval = approval,
        redemptionCount = redemptionCount,
        grantEverCreated = false,
        role = null,
        epochBound = epoch,
    )

    private fun current(
        record: PairingRecord,
        code: String,
        ok: Boolean,
        responseLossRetry: Boolean = false,
        grantCreated: Boolean = false,
        fabricatedPending: Boolean = false,
    ) = Attempt(
        ok = ok,
        code = code,
        approval = record.approval,
        redemptionCount = record.redemptionCount,
        reconsumed = false,
        grantCreated = grantCreated,
        ttlExtended = false,
        role = record.role,
        responseLossRetry = responseLossRetry,
        selfApproved = false,
        fabricatedPending = fabricatedPending,
        disclosesOtherClaimant = false,
    )

    private fun terminal(code: String, record: PairingRecord?) = Attempt(
        ok = false,
        code = code,
        approval = record?.approval,
        redemptionCount = record?.redemptionCount ?: 0,
        reconsumed = false,
        grantCreated = false,
        ttlExtended = false,
        role = record?.role,
        responseLossRetry = false,
        selfApproved = false,
        fabricatedPending = false,
        disclosesOtherClaimant = false,
    )

    private fun rejected(raw: String, mode: ConnectionMode, code: String) = PairingPreview(
        rawInput = raw,
        scenarioId = "",
        requestId = "",
        mode = mode,
        declaredServer = "声明：未采用（输入已拒绝）",
        declaredPc = "声明：未采用（输入已拒绝）",
        verified = false,
        verifiedServer = null,
        verifiedPc = null,
        mismatch = false,
        acknowledged = false,
        expired = false,
        revokedScenario = false,
        rejected = true,
        rejectCode = code,
    )

    companion object {
        fun looksMalicious(raw: String): Boolean {
            val lowered = raw.lowercase()
            val decoded = lowered.replace("%3a", ":").replace("%2f", "/")
            val probes = listOf(lowered, decoded)
            val bad = listOf(
                "javascript:",
                "file:",
                "data:",
                "intent:",
                "http://",
                "https://",
                "169.254.169.254",
            )
            return probes.any { probe -> bad.any { token -> probe.contains(token) } }
        }
    }
}
