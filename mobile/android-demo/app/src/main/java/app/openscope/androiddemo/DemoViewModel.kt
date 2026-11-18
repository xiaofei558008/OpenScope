package app.openscope.androiddemo

import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.viewModelScope
import androidx.lifecycle.viewmodel.CreationExtras
import app.openscope.androiddemo.domain.CloudRaw
import app.openscope.androiddemo.domain.ConnectionMode
import app.openscope.androiddemo.domain.DemoOnlySettings
import app.openscope.androiddemo.domain.DemoSession
import app.openscope.androiddemo.domain.DemoSnapshot
import app.openscope.androiddemo.domain.EntitlementPlaceholder
import app.openscope.androiddemo.domain.FileDeviceStore
import app.openscope.androiddemo.domain.Freshness
import app.openscope.androiddemo.domain.OrderPlaceholder
import app.openscope.androiddemo.domain.PcCommandConfirm
import app.openscope.androiddemo.domain.PcDecision
import app.openscope.androiddemo.domain.SavedDeviceStore
import java.io.File
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.isActive
import kotlinx.coroutines.launch

class DemoViewModel(store: SavedDeviceStore) : ViewModel() {
    private val session = DemoSession(saved = store.load(), clock = { System.currentTimeMillis() })
    private val memory = store
    private val _state = MutableStateFlow(session.snapshot())
    val state: StateFlow<DemoSnapshot> = _state.asStateFlow()

    init {
        viewModelScope.launch {
            while (isActive) {
                session.onGeneratorTick()
                delay(DemoOnlySettings.GENERATOR_PERIOD_MS)
            }
        }
        viewModelScope.launch {
            while (isActive) {
                session.publishUi()
                _state.value = session.snapshot()
                delay(DemoOnlySettings.UI_REFRESH_PERIOD_MS)
            }
        }
    }

    fun requestMode(mode: ConnectionMode) = act { requestMode(mode) }
    fun confirmModeSwitch() = act { confirmModeSwitch() }
    fun cancelModeSwitch() = act { cancelModeSwitch() }
    fun parseInput(raw: String) = act { parseInput(raw) }
    fun verify() = act { verify() }
    fun acknowledgeVerified() = act { acknowledgeVerified() }
    fun applyPairing() = act { applyPairing() }
    fun pcDecide(decision: PcDecision) = act { pcDecide(decision) }
    fun selfApprove() = act { selfApprove() }
    fun recoverSame() = act { recoverSame() }
    fun resumeViewing() = act { resumeViewing() }
    fun rememberDevice(label: String) = act { rememberDevice(label) }
    fun selectSaved(localId: String) = act { selectSaved(localId) }
    fun deleteSavedLocal(localId: String) = act { deleteSavedLocal(localId) }
    fun revokeServer() = act { revokeServer() }
    fun disconnect() = act { disconnect() }
    fun requestReconnect() = act { requestReconnect() }
    fun injectLanFailure() = act { injectLanFailure() }
    fun continueCurrentMode() = act { continueCurrentMode() }
    fun setSelfHostAddress(text: String) = act { setSelfHostAddress(text) }
    fun injectCertFailure() = act { injectCertFailure() }
    fun observePaymentReturn() = act { observePaymentReturn() }
    fun serverOrder(next: OrderPlaceholder) = act { serverOrder(next) }
    fun serverEntitlement(next: EntitlementPlaceholder) = act { serverEntitlement(next) }
    fun searchNow(query: String) = act {
        val token = beginSearch(query)
        finishSearch(token, snapshot().epoch)
    }
    fun cancelSearch() = act { cancelSearch() }
    fun demonstrateStaleSearch() = act { demonstrateStaleSearch() }
    fun setProjectEmpty(empty: Boolean) = act { setProjectEmpty(empty) }
    fun submitControlAttempt() = act { submitControlAttempt() }
    fun injectReceipt(raw: CloudRaw) = act { injectReceipt(raw) }
    fun setPcCommandConfirm(id: String, confirm: PcCommandConfirm) = act { setPcCommandConfirm(id, confirm) }
    fun toggleFreeze() = act { toggleFreeze() }
    fun zoomIn() = act { zoomIn() }
    fun zoomOut() = act { zoomOut() }
    fun rejectExtraSubscription() = act { rejectExtraSubscription() }
    fun holdFreshness(next: Freshness?) = act { holdFreshness(next) }
    fun onBackground() = act { onBackground() }
    fun onForeground() = act { onForeground() }

    private fun act(block: DemoSession.() -> Unit) {
        session.block()
        memory.save(session.exportSaved())
        _state.value = session.snapshot()
    }

    companion object {
        fun factory(file: File): ViewModelProvider.Factory = object : ViewModelProvider.Factory {
            override fun <T : ViewModel> create(modelClass: Class<T>, extras: CreationExtras): T {
                @Suppress("UNCHECKED_CAST")
                return DemoViewModel(FileDeviceStore(file)) as T
            }
        }
    }
}
