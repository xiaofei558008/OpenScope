package app.openscope.androiddemo.domain

object DemoCopy {
    const val BANNER = "DEMO · 模拟数据 · 进程内演示 · 不连接真实设备"
    const val CLAIMANT = "demo-handset"
    const val TIME_SOURCE = "演示时钟 demo-clock"

    fun explain(code: String): String = when (code) {
        "APPLIED" -> "已提交配对申请，正在等待 PC 确认。尚未授予观察。"
        "RECOVERED_CURRENT" -> "已读回当前审批。未二次兑换，未新发授权，未延长有效期。"
        "ENDPOINT_CONFIRM_REQUIRED" -> "声明和已验证身份不一致。需要明确按已验证身份申请。"
        "NOT_VERIFIED" -> "来源尚未验证，不能申请。"
        "PAIRING_CONFIRM_REQUIRED" -> "手机不能自行批准。审批仍保持原样。"
        "PAIRING_CODE_EXPIRED" -> "该演示输入已过期。未延长有效期，也没有编造新的待审批。"
        "DEVICE_REVOKED" -> "该记录已吊销，不能恢复成待审批或观察。"
        "LATE_CONFIRM_IGNORED" -> "迟到的确认不能推翻已经结束的审批。"
        "STALE_EPOCH" -> "旧代数上的确认不能改变当前会话。"
        "DECISION_HELD" -> "当前会话未在等待确认。记录保持原样，没有再次兑换。"
        "PAIRING_RACE_REJECTED" -> "不是同一已验证申请。未透露其他申请的状态。"
        "PAIRING_CODE_CONSUMED" -> "该申请已经结束。其他申请方读不到审批内容。"
        "PAIRING_KEY_MISMATCH" -> "绑定内容不同，不能当作同一次申请恢复。"
        "RECOVERY_UNAVAILABLE" -> "没有可恢复的申请，不会编造待审批。"
        "MALICIOUS" -> "已拒绝该输入。未打开、未访问、未下载。"
        "UNKNOWN_INPUT" -> "不是本演示认识的场景。已拒绝，未打开、未访问。"
        "PC_APPROVED" -> "PC 确认通过。默认角色是观察者 viewer。控制另算，且本演示不可执行。"
        "PC_DENIED" -> "PC 已拒绝。没有观察授权。"
        "PC_CANCELLED" -> "PC 已取消。没有观察授权。"
        "PC_TIMED_OUT" -> "PC 确认超时。没有观察授权，也没有改写为成功。"
        else -> "演示语义：$code"
    }

    fun raw(raw: CloudRaw): String = when (raw) {
        CloudRaw.Received -> "received"
        CloudRaw.Accepted -> "accepted"
        CloudRaw.Running -> "running"
        CloudRaw.Succeeded -> "succeeded"
        CloudRaw.Failed -> "failed"
        CloudRaw.Rejected -> "rejected"
        CloudRaw.Unknown -> "unknown"
    }

    fun group(raw: CloudRaw): String = when (raw) {
        CloudRaw.Rejected -> "拒绝"
        CloudRaw.Received, CloudRaw.Accepted, CloudRaw.Running -> "进行中"
        CloudRaw.Succeeded -> "成功"
        CloudRaw.Failed -> "确认失败"
        CloudRaw.Unknown -> "未知"
    }

    fun sub(raw: CloudRaw): String = when (raw) {
        CloudRaw.Received -> "已收到"
        CloudRaw.Accepted -> "已接受"
        CloudRaw.Running -> "执行中"
        CloudRaw.Succeeded -> "成功"
        CloudRaw.Failed -> "确认失败"
        CloudRaw.Rejected -> "拒绝"
        CloudRaw.Unknown -> "未知"
    }

    fun approval(approval: Approval): String = when (approval) {
        Approval.Pending -> "待 PC 确认 pending"
        Approval.ApprovedViewer -> "已通过 · 观察者 viewer"
        Approval.Denied -> "已拒绝 denied"
        Approval.Cancelled -> "已取消 cancelled"
        Approval.TimedOut -> "确认超时 timed_out"
        Approval.Expired -> "已过期 expired"
        Approval.Revoked -> "已吊销 revoked"
    }

    fun order(order: OrderPlaceholder): String = when (order) {
        OrderPlaceholder.None -> "无服务端订单"
        OrderPlaceholder.PaymentPending -> "服务端订单：待支付（占位）"
        OrderPlaceholder.PaymentFailed -> "服务端订单：支付失败（占位）"
        OrderPlaceholder.Closed -> "服务端订单：已关闭（占位）"
        OrderPlaceholder.Refund -> "服务端订单：退款（占位）"
        OrderPlaceholder.Active -> "服务端订单：active（占位，不等于权益）"
    }

    fun entitlement(state: EntitlementPlaceholder): String = when (state) {
        EntitlementPlaceholder.Unpurchased -> "权益：未购买"
        EntitlementPlaceholder.Active -> "权益：active（仅模拟服务端）"
        EntitlementPlaceholder.Expired -> "权益：expired"
        EntitlementPlaceholder.Exhausted -> "权益：exhausted"
        EntitlementPlaceholder.Revoked -> "权益：revoked"
    }
}
