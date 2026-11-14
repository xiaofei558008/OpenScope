package app.openscope.androiddemo.domain

/**
 * In-process demo vocabulary.
 *
 * Names such as received/accepted or PAIRING_* below are semantic labels for
 * this demo and its unit tests. They are not a frozen wire schema, QR scheme,
 * desktop ABI, or permission to call a product endpoint.
 */
enum class ConnectionMode(val label: String, val detail: String) {
    LanDirect(
        "局域网直连 PC",
        "同网直接连接 PC 的名称。本演示不打开套接字，失败不会改走官方云。",
    ),
    OfficialManaged(
        "官方托管云",
        "官方托管付费中转的名称。本演示无端点、无价格、无扣费。",
    ),
    SelfHosted(
        "自托管",
        "自有服务器地址只作占位。本演示不连接、不执行部署脚本、不收集管理凭据。",
    ),
}

enum class TransportState { Idle, Up, Down, Failed }

enum class Freshness { NoSample, Normal, Stale, Gap, Timeout, Paused }

enum class Approval {
    Pending,
    ApprovedViewer,
    Denied,
    Cancelled,
    TimedOut,
    Expired,
    Revoked,
}

enum class Role { Viewer }

enum class PcDecision { Approve, Deny, Cancel, Timeout }

enum class CloudRaw {
    Received,
    Accepted,
    Running,
    Succeeded,
    Failed,
    Rejected,
    Unknown,
}

enum class LocalCommandConfidence { NotSent, Submitted, DisconnectedUnknown }

enum class PcCommandConfirm { None, Waiting, Confirmed, Refused, Cancelled }

enum class OrderPlaceholder { None, PaymentPending, PaymentFailed, Closed, Refund, Active }

enum class EntitlementPlaceholder { Unpurchased, Active, Expired, Exhausted, Revoked }

data class Binding(
    val mode: ConnectionMode,
    val claimantId: String,
    val requestId: String,
    val verifiedServer: String,
    val verifiedPc: String,
) {
    fun fingerprint(): String =
        listOf(mode.name, claimantId, requestId, verifiedServer, verifiedPc).joinToString("|")
}

data class PairingRecord(
    val binding: Binding,
    val approval: Approval,
    val redemptionCount: Int,
    val grantEverCreated: Boolean,
    val role: Role?,
    val epochBound: Long,
)

data class PairingPreview(
    val rawInput: String,
    val scenarioId: String,
    val requestId: String,
    val mode: ConnectionMode,
    val declaredServer: String,
    val declaredPc: String,
    val verified: Boolean,
    val verifiedServer: String?,
    val verifiedPc: String?,
    val mismatch: Boolean,
    val acknowledged: Boolean,
    val expired: Boolean,
    val revokedScenario: Boolean,
    val rejected: Boolean,
    val rejectCode: String?,
)

/**
 * Result of one demo pairing attempt.
 * [grantCreated] describes this attempt only. Recovery keeps it false.
 * [ttlExtended] is always false: this demo does not store a TTL number.
 */
data class Attempt(
    val ok: Boolean,
    val code: String,
    val approval: Approval?,
    val redemptionCount: Int,
    val reconsumed: Boolean,
    val grantCreated: Boolean,
    val ttlExtended: Boolean,
    val role: Role?,
    val responseLossRetry: Boolean,
    val selfApproved: Boolean,
    val fabricatedPending: Boolean,
    val disclosesOtherClaimant: Boolean,
)

data class SavedDeviceEntry(
    val localId: String,
    val label: String,
    val mode: ConnectionMode,
    val serverLabel: String,
    val pcLabel: String,
    val requestId: String,
)

data class Sample(
    val seq: Long,
    val tMillis: Long,
    val channel: Int,
    val value: Float,
    val gapBefore: Boolean,
    val epoch: Long,
    val sourceKey: String,
)

data class HealthLayers(
    val transport: String,
    val pairing: String,
    val channel: String,
    val acquisition: String,
    val freshness: String,
)

data class VarRow(
    val name: String,
    val module: String,
    val unit: String,
    val type: String,
    val channel: Int?,
)

data class ProjectView(
    val allowed: Boolean,
    val empty: Boolean,
    val projectName: String,
    val rows: List<VarRow>,
    val appliedQuery: String,
    val staleDropped: Int,
)

data class ValueCard(
    val name: String,
    val unit: String,
    val typeName: String,
    val source: String,
    val timeSource: String,
    val updatedAt: String,
    val quality: String,
    val displayValue: String,
)

data class WaveView(
    val liveCount: Int,
    val channels: List<List<Sample>>,
    val frozen: Boolean,
    val zoomStep: Int,
    val gapCount: Int,
    val sourceLabel: String,
    val sourceChangeNote: String?,
    val extraRejected: Int,
)

data class CommandView(
    val id: String,
    val raw: CloudRaw?,
    val rawLabel: String,
    val groupLabel: String,
    val sublabel: String,
    val localLabel: String,
    val pcConfirm: PcCommandConfirm,
    val pcLabel: String,
    val executed: Boolean,
)

data class EntitlementView(
    val serverOrder: OrderPlaceholder,
    val entitlement: EntitlementPlaceholder,
    val returnObserved: Boolean,
    val note: String,
)

data class DemoSnapshot(
    val banner: String,
    val settingsNote: String,
    val mode: ConnectionMode,
    val pendingMode: ConnectionMode?,
    val epoch: Long,
    val transport: TransportState,
    val failureNote: String?,
    val silentCloudSwitch: Boolean,
    val plaintextFallback: Boolean,
    val health: HealthLayers,
    val preview: PairingPreview?,
    val storedApproval: Approval?,
    val localPairingNote: String,
    val roleLabel: String,
    val controlExecutable: Boolean,
    val lastCode: String?,
    val lastMessage: String?,
    val redemptionCount: Int,
    val grantCreatedOnLastAttempt: Boolean,
    val ttlExtended: Boolean,
    val reconsumed: Boolean,
    val selfApproved: Boolean,
    val pcActionsEnabled: Boolean,
    val saved: List<SavedDeviceEntry>,
    val selectedSavedId: String?,
    val savedRestoresAuth: Boolean,
    val localDeleteIsServerRevoke: Boolean,
    val serverRevoked: Boolean,
    val project: ProjectView,
    val numbers: List<ValueCard>,
    val waves: WaveView,
    val commands: List<CommandView>,
    val entitlement: EntitlementView,
    val selfHostAddress: String,
    val adminCredentialFields: Boolean,
    val controlQueryCount: Int,
    val replayCount: Int,
    val fetchCount: Int,
    val openedUrls: List<String>,
    val generatorTicks: Long,
    val uiPublishes: Long,
    val backgroundPaused: Boolean,
    val lifecycleNote: String?,
    val sourceChangeNote: String?,
    val viewingLive: Boolean,
    val resumeEnabled: Boolean,
)
