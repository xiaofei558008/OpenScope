package app.openscope.androiddemo.domain

/**
 * Local demonstration knobs.
 *
 * These numbers are fixture settings for this demo process. They are not
 * product performance claims, not SRS acceptance budgets, not prices, and
 * not frozen protocol or quota defaults.
 */
object DemoOnlySettings {
    const val RING_CAPACITY = 240
    const val MAX_CHANNELS = 4
    const val GENERATOR_PERIOD_MS = 100L
    const val UI_REFRESH_PERIOD_MS = 250L
    const val STALE_AFTER_MS = 1_500L

    val note: String =
        "演示专用设置：环容量 $RING_CAPACITY、最多 $MAX_CHANNELS 条曲线；" +
            "数据生成节拍 ${GENERATOR_PERIOD_MS} ms，界面刷新 ${UI_REFRESH_PERIOD_MS} ms，两者分开。" +
            "不是产品性能指标，也不是配额、价格或协议默认值。"
}
