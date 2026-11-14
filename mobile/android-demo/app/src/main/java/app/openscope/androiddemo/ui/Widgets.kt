package app.openscope.androiddemo.ui

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.ColumnScope
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.ElevatedCard
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.semantics.contentDescription
import androidx.compose.ui.semantics.semantics
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import app.openscope.androiddemo.domain.DemoCopy

@Composable
fun DemoBanner(extra: String?) {
    Column(
        Modifier
            .fillMaxWidth()
            .background(Color(0xFF3D3422))
            .semantics { contentDescription = "持久演示标记 DEMO 模拟数据" }
            .padding(horizontal = 16.dp, vertical = 8.dp),
    ) {
        Text(
            DemoCopy.BANNER,
            color = Color(0xFFFFF6E4),
            fontWeight = FontWeight.Bold,
            fontSize = 13.sp,
        )
        if (!extra.isNullOrBlank()) {
            Text(extra, color = Color(0xFFE7D7B0), fontSize = 12.sp)
        }
    }
}

@Composable
fun SectionCard(
    title: String,
    modifier: Modifier = Modifier,
    content: @Composable ColumnScope.() -> Unit,
) {
    ElevatedCard(modifier.fillMaxWidth()) {
        Column(
            Modifier.padding(12.dp),
            verticalArrangement = Arrangement.spacedBy(8.dp),
            content = {
                Text(title, style = MaterialTheme.typography.titleMedium)
                content()
            },
        )
    }
}
