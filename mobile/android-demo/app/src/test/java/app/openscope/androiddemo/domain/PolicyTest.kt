package app.openscope.androiddemo.domain

import java.io.File
import org.junit.Assert.assertFalse
import org.junit.Assert.assertTrue
import org.junit.Test

class PolicyTest {
    @Test
    fun manifestDeclaresNoRuntimePermissions() {
        val manifest = File(moduleRoot(), "src/main/AndroidManifest.xml").readText()
        assertFalse(manifest.contains("<uses-permission"))
        assertFalse(manifest.contains("android.permission.INTERNET"))
        assertFalse(manifest.contains("android.permission.CAMERA"))
        assertFalse(manifest.contains("uses-feature"))
        assertTrue(manifest.contains("allowBackup=\"false\""))
    }

    @Test
    fun sourcesDoNotReferenceNetworkCameraPaymentOrShell() {
        val banned = listOf(
            "java.net",
            "HttpURLConnection",
            "OkHttp",
            "okhttp",
            "android.hardware.camera",
            "CameraManager",
            "BillingClient",
            "com.android.billingclient",
            "Runtime.getRuntime",
            "ProcessBuilder",
            "startActivity",
            "ACTION_VIEW",
            "control_query(",
        )
        val files = File(moduleRoot(), "src/main").walkTopDown().filter { it.extension == "kt" || it.extension == "xml" }
        val hits = files.flatMap { file ->
            val text = file.readText()
            banned.filter { token -> text.contains(token) }.map { token -> "${file.name}: $token" }
        }.toList()
        assertTrue(hits.joinToString(), hits.isEmpty())
    }

    @Test
    fun savedDeviceCodecHasNoCredentialField() {
        val entry = SavedDeviceEntry(
            localId = "saved-1",
            label = "设备\t甲",
            mode = ConnectionMode.LanDirect,
            serverLabel = "已验证：局域网直连 PC，无云中继",
            pcLabel = "已验证：PC-1",
            requestId = "demo-req-demo-ok-LanDirect",
        )
        val encoded = DeviceCodec.encode(listOf(entry))
        assertFalse(encoded.contains("token"))
        assertFalse(encoded.contains("grant"))
        assertFalse(encoded.contains("password"))
        assertEqualsRoundTrip(entry, DeviceCodec.decode(encoded).single())
        val withExtra = encoded + "\tsecret-token"
        val decoded = DeviceCodec.decode(withExtra).single()
        assertEqualsRoundTrip(entry, decoded)
    }

    private fun assertEqualsRoundTrip(expected: SavedDeviceEntry, actual: SavedDeviceEntry) {
        assertTrue(expected == actual)
    }

    private fun moduleRoot(): File {
        val start = System.getProperty("user.dir") ?: error("user.dir missing")
        var dir = File(start)
        repeat(6) {
            if (File(dir, "src/main/AndroidManifest.xml").exists()) return dir
            dir = dir.parentFile ?: error("manifest not found")
        }
        error("manifest not found from $start")
    }
}
