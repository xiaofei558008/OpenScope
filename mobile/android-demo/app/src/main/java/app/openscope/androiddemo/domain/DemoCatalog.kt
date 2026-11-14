package app.openscope.androiddemo.domain

object DemoCatalog {
    val rows: List<VarRow> = listOf(
        VarRow("g_app.synth_speed", "动力总成", "km/h", "f32", 0),
        VarRow("g_app.synth_rpm", "动力总成", "rpm", "f32", 1),
        VarRow("g_app.synth_temp", "热管理", "°C", "f32", 2),
        VarRow("g_app.synth_zero", "标定摘要", "1", "f32", 3),
        VarRow("g_app.synth_note", "标定摘要", "", "text", null),
    )

    const val PROJECT = "演示工程 Alpha"

    fun filtered(query: String): List<VarRow> {
        val q = query.trim()
        if (q.isEmpty()) return rows
        return rows.filter {
            it.name.contains(q, ignoreCase = true) || it.module.contains(q, ignoreCase = true)
        }
    }
}
