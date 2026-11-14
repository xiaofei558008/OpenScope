package app.openscope.androiddemo.domain

object DemoScenarios {
    data class Item(
        val id: String,
        val blurb: String,
        val expired: Boolean = false,
        val revoked: Boolean = false,
        val mismatch: Boolean = false,
    )

    val all: List<Item> = listOf(
        Item("demo-ok", "声明与验证一致，可申请"),
        Item("demo-mismatch", "声明的 PC 与验证结果不同", mismatch = true),
        Item("demo-expired", "过期输入，不能建立待审批", expired = true),
        Item("demo-deny", "身份一致。请再用 PC 拒绝按钮演示拒绝"),
        Item("demo-cancel", "身份一致。请再用 PC 取消按钮"),
        Item("demo-timeout", "身份一致。请再用 PC 超时按钮"),
        Item("demo-revoked", "吊销记录，不能恢复为待审批", revoked = true),
    )

    fun find(id: String): Item? = all.find { it.id == id }

    fun declaredServer(mode: ConnectionMode): String = when (mode) {
        ConnectionMode.LanDirect -> "声明：局域网内的 PC，无云服务器"
        ConnectionMode.OfficialManaged -> "声明：官方托管云（无真实端点）"
        ConnectionMode.SelfHosted -> "声明：自托管服务（无真实连接）"
    }

    fun verifiedServer(mode: ConnectionMode): String = when (mode) {
        ConnectionMode.LanDirect -> "已验证：局域网直连 PC，无云中继"
        ConnectionMode.OfficialManaged -> "已验证：官方托管云身份（进程内，非端点）"
        ConnectionMode.SelfHosted -> "已验证：自托管身份（进程内，未拨号）"
    }

    fun declaredPc(mismatch: Boolean): String =
        if (mismatch) "声明：PC-其他" else "声明：PC-1"

    const val VERIFIED_PC = "已验证：PC-1"

    fun requestId(scenarioId: String, mode: ConnectionMode): String =
        "demo-req-$scenarioId-${mode.name}"
}
