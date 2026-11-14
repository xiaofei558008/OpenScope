package app.openscope.androiddemo.domain

/**
 * Process-local demo session.
 *
 * Every connection mode stays inside this object. Nothing here opens a
 * socket, listener, camera, payment client, or device shell.
 */
class DemoSession(
    saved: List<SavedDeviceEntry> = emptyList(),
    private val clock: () -> Long = { 0L },
    private val claimantId: String = DemoCopy.CLAIMANT,
) {
    private val book = PairingBook()
    private val commands = CommandBoard()
    private val rings = SampleRings()

    private var mode = ConnectionMode.LanDirect
    private var pendingMode: ConnectionMode? = null
    private var epoch = 1L
    private var transport = TransportState.Idle
    private var failureNote: String? = null
    private var lanFailed = false
    private var certFailed = false
    private val plaintextFallback = false
    private val silentCloudSwitch = false

    private var preview: PairingPreview? = null
    private var activeRequestId: String? = null
    private var acceptingPc = false
    private var liveEpoch: Long? = null
    private var localPairingNote = "尚未配对。"
    private var lastAttempt: Attempt? = null

    private var savedDevices = saved.toMutableList()
    private var savedSeq = saved.size + 1
    private var selectedSavedId: String? = null
    private var serverRevoked = false

    private var serverOrder = OrderPlaceholder.None
    private var entitlement = EntitlementPlaceholder.Unpurchased
    private var returnObserved = false
    private var entitlementNote = "订单与权益分开。无价格、无配额数字、无下单。"

    private var selfHostAddress = ""
    private var projectEmpty = false
    private var appliedQuery = ""
    private var pendingQuery = ""
    private var searchToken = 0L
    private var searchEpoch = epoch
    private var staleDropped = 0

    private var published: List<List<Sample>> = List(DemoOnlySettings.MAX_CHANNELS) { emptyList() }
    private var frozenChannels: List<List<Sample>>? = null
    private var zoomStep = 0
    private var extraRejected = 0
    private var freshnessHold: Freshness? = null
    private var sourceChangeNote: String? = null
    private var generatorTicks = 0L
    private var uiPublishes = 0L
    private var backgroundPaused = false
    private var wasBackgrounded = false
    private var lifecycleNote: String? = null
    private var fetchCount = 0
    private val openedUrls = emptyList<String>()

    init {
        rings.retarget(epoch, sourceKey())
    }

    fun snapshot(): DemoSnapshot {
        val record = activeRequestId?.let { book.record(it) }
        val viewing = isViewingLive(record)
        val waves = waveView()
        return DemoSnapshot(
            banner = DemoCopy.BANNER,
            settingsNote = DemoOnlySettings.note,
            mode = mode,
            pendingMode = pendingMode,
            epoch = epoch,
            transport = transport,
            failureNote = failureNote,
            silentCloudSwitch = silentCloudSwitch,
            plaintextFallback = plaintextFallback,
            health = health(record, viewing),
            preview = preview,
            storedApproval = record?.approval,
            localPairingNote = localPairingNote,
            roleLabel = roleLabel(record, viewing),
            controlExecutable = false,
            lastCode = lastAttempt?.code,
            lastMessage = lastAttempt?.let { DemoCopy.explain(it.code) },
            redemptionCount = record?.redemptionCount ?: lastAttempt?.redemptionCount ?: 0,
            grantCreatedOnLastAttempt = lastAttempt?.grantCreated == true,
            ttlExtended = false,
            reconsumed = false,
            selfApproved = false,
            pcActionsEnabled = acceptingPc && record?.approval == Approval.Pending,
            saved = savedDevices.toList(),
            selectedSavedId = selectedSavedId,
            savedRestoresAuth = false,
            localDeleteIsServerRevoke = false,
            serverRevoked = serverRevoked,
            project = project(viewing),
            numbers = numbers(viewing),
            waves = waves,
            commands = commands.views(),
            entitlement = EntitlementView(serverOrder, entitlement, returnObserved, entitlementNote),
            selfHostAddress = selfHostAddress,
            adminCredentialFields = false,
            controlQueryCount = commands.controlQueryCount,
            replayCount = commands.replayCount,
            fetchCount = fetchCount,
            openedUrls = openedUrls,
            generatorTicks = generatorTicks,
            uiPublishes = uiPublishes,
            backgroundPaused = backgroundPaused,
            lifecycleNote = lifecycleNote,
            sourceChangeNote = sourceChangeNote,
            viewingLive = viewing,
            resumeEnabled = record?.approval == Approval.ApprovedViewer && !viewing && !serverRevoked,
        )
    }

    fun requestMode(next: ConnectionMode) {
        if (next == mode) {
            pendingMode = null
            return
        }
        pendingMode = next
    }

    fun cancelModeSwitch() {
        pendingMode = null
    }

    fun confirmModeSwitch() {
        val next = pendingMode ?: return
        pendingMode = null
        mode = next
        lanFailed = false
        certFailed = false
        failureNote = null
        transport = TransportState.Idle
        preview = null
        activeRequestId = null
        acceptingPc = false
        serverRevoked = false
        dropLiveView("已切换模式。旧通道样本已清除，需要重新验证。")
        retarget("来源已变更：切换到${next.label}，旧代数样本已清除。")
    }

    fun parseInput(raw: String) {
        val parsed = book.parse(raw, mode, claimantId)
        preview = parsed
        activeRequestId = parsed.requestId.ifEmpty { null }
        acceptingPc = false
        serverRevoked = book.record(parsed.requestId)?.approval == Approval.Revoked
        if (parsed.rejected) {
            lastAttempt = Attempt(
                ok = false,
                code = parsed.rejectCode ?: "UNKNOWN_INPUT",
                approval = null,
                redemptionCount = 0,
                reconsumed = false,
                grantCreated = false,
                ttlExtended = false,
                role = null,
                responseLossRetry = false,
                selfApproved = false,
                fabricatedPending = false,
                disclosesOtherClaimant = false,
            )
            localPairingNote = DemoCopy.explain(lastAttempt!!.code)
        } else {
            lastAttempt = null
            localPairingNote = "已读入声明信息。验证前不能把它当成已确认身份。"
        }
    }

    fun verify() {
        val current = preview ?: return
        preview = book.verify(current)
        localPairingNote = if (preview?.verified == true) {
            "已分列声明与验证结果。验证仍不是配对，也不是控制授权。"
        } else {
            localPairingNote
        }
    }

    fun acknowledgeVerified() {
        val current = preview ?: return
        if (current.verified) preview = current.copy(acknowledged = true)
    }

    fun applyPairing() {
        val current = preview ?: return
        val attempt = book.apply(current, epoch, claimantId)
        note(attempt)
        activeRequestId = current.requestId.ifEmpty { activeRequestId }
        acceptingPc = attempt.code == "APPLIED" && attempt.approval == Approval.Pending
        if (attempt.approval == Approval.ApprovedViewer && attempt.code == "RECOVERED_CURRENT") {
            localPairingNote = "当前审批仍是观察者。尚未恢复本代数的查看，需要单独确认。"
        }
        serverRevoked = attempt.approval == Approval.Revoked
    }

    fun pcDecide(decision: PcDecision) {
        val id = activeRequestId ?: return
        if (!acceptingPc) {
            val existing = book.record(id)
            lastAttempt = Attempt(
                ok = false,
                code = "DECISION_HELD",
                approval = existing?.approval,
                redemptionCount = existing?.redemptionCount ?: 0,
                reconsumed = false,
                grantCreated = false,
                ttlExtended = false,
                role = existing?.role,
                responseLossRetry = false,
                selfApproved = false,
                fabricatedPending = false,
                disclosesOtherClaimant = false,
            )
            localPairingNote = DemoCopy.explain("DECISION_HELD")
            return
        }
        val attempt = book.decide(id, epoch, decision)
        note(attempt)
        if (attempt.code == "PC_APPROVED") {
            acceptingPc = false
            tryStartViewing()
        } else if (attempt.approval != Approval.Pending) {
            acceptingPc = false
            stopSamples()
        }
    }

    fun selfApprove() {
        val id = activeRequestId
        val attempt = if (id == null) {
            Attempt(
                ok = false,
                code = "PAIRING_CONFIRM_REQUIRED",
                approval = null,
                redemptionCount = 0,
                reconsumed = false,
                grantCreated = false,
                ttlExtended = false,
                role = null,
                responseLossRetry = false,
                selfApproved = false,
                fabricatedPending = false,
                disclosesOtherClaimant = false,
            )
        } else {
            book.selfApprove(id)
        }
        note(attempt)
    }

    fun recoverSame() {
        val id = activeRequestId ?: return
        val binding = currentBinding() ?: return
        val before = book.grantCreations(id)
        val attempt = book.recover(id, claimantId, binding)
        note(attempt)
        check(book.grantCreations(id) == before)
        acceptingPc = attempt.ok && attempt.approval == Approval.Pending
        serverRevoked = attempt.approval == Approval.Revoked
        if (!attempt.ok || attempt.approval != Approval.ApprovedViewer) {
            liveEpoch = null
            stopSamples()
        }
        localPairingNote = DemoCopy.explain(attempt.code) + " 查看不会自动恢复。"
    }

    fun resumeViewing() {
        val record = activeRequestId?.let { book.record(it) }
        if (record?.approval != Approval.ApprovedViewer || record.role != Role.Viewer) {
            localPairingNote = "当前审批不是观察者，不能恢复查看。"
            return
        }
        if (serverRevoked) {
            localPairingNote = "服务端吊销仍然有效，不能恢复查看。"
            return
        }
        tryStartViewing()
    }

    fun rememberDevice(label: String) {
        val record = activeRequestId?.let { book.record(it) } ?: return
        if (record.approval != Approval.ApprovedViewer) {
            localPairingNote = "只有 PC 已确认的观察会话可以记住设备。记住不等于保存授权。"
            return
        }
        val entry = SavedDeviceEntry(
            localId = "saved-$savedSeq",
            label = label.ifBlank { "演示设备 $savedSeq" },
            mode = mode,
            serverLabel = record.binding.verifiedServer,
            pcLabel = record.binding.verifiedPc,
            requestId = record.binding.requestId,
        )
        savedSeq += 1
        savedDevices.removeAll { it.requestId == entry.requestId && it.mode == entry.mode }
        savedDevices.add(entry)
        localPairingNote = "已记住设备条目。条目里没有令牌或控制许可。"
    }

    fun selectSaved(localId: String) {
        val entry = savedDevices.find { it.localId == localId } ?: return
        selectedSavedId = localId
        if (entry.mode != mode) {
            mode = entry.mode
            lanFailed = false
            certFailed = false
            failureNote = null
        }
        preview = PairingPreview(
            rawInput = "saved:${entry.localId}",
            scenarioId = "saved-record",
            requestId = entry.requestId,
            mode = entry.mode,
            declaredServer = entry.serverLabel,
            declaredPc = entry.pcLabel,
            verified = false,
            verifiedServer = null,
            verifiedPc = null,
            mismatch = false,
            acknowledged = false,
            expired = false,
            revokedScenario = false,
            rejected = false,
            rejectCode = null,
        )
        activeRequestId = entry.requestId
        acceptingPc = false
        serverRevoked = book.record(entry.requestId)?.approval == Approval.Revoked
        dropLiveView("已点选保存的设备。授权未恢复，需要重新验证。")
        retarget("来源已变更：点选了保存设备「${entry.label}」。旧样本已清除，授权未恢复。")
    }

    fun deleteSavedLocal(localId: String) {
        savedDevices.removeAll { it.localId == localId }
        if (selectedSavedId == localId) selectedSavedId = null
        localPairingNote = "已删除本地设备条目。服务端配对记录没有因此改变。"
    }

    fun revokeServer() {
        val id = activeRequestId ?: return
        val attempt = book.revoke(id)
        note(attempt)
        serverRevoked = true
        acceptingPc = false
        dropLiveView("模拟服务端吊销已生效。旧观察停止，命令不会重放。")
        clearSamples("吊销后已清除该通道样本。")
    }

    fun disconnect() {
        commands.onDisconnected()
        acceptingPc = false
        liveEpoch = null
        transport = TransportState.Down
        rings.enabled = false
        rings.armGap()
        publishUi()
        localPairingNote = "已断开。配对记录保持原审批，本地视为需要重新核验，不会自动再次兑换。"
        failureNote = null
    }

    fun requestReconnect() {
        commands.onDisconnected()
        acceptingPc = false
        dropLiveView("已请求重连。这不是授权恢复。")
        transport = TransportState.Down
        retarget { newEpoch ->
            "来源已变更：重连使用新代数 $newEpoch，旧代数样本已清除。"
        }
    }

    fun injectLanFailure() {
        if (mode != ConnectionMode.LanDirect) return
        lanFailed = true
        certFailed = false
        transport = TransportState.Failed
        failureNote = "局域网不可达（模拟）。模式仍是局域网直连 PC，没有改为官方托管云。"
        stopSamples()
        clearSamples("失败后已清除演示波形，没有回退成另一条成功曲线。")
        check(!silentCloudSwitch)
    }

    fun continueCurrentMode() {
        if (mode == ConnectionMode.LanDirect) lanFailed = false
        if (mode == ConnectionMode.SelfHosted) certFailed = false
        failureNote = null
        if (transport == TransportState.Failed) transport = TransportState.Idle
        localPairingNote = "仍停留在${mode.label}。没有切换到其他模式，也没有明文降级。"
        val record = activeRequestId?.let { book.record(it) }
        if (record?.approval == Approval.ApprovedViewer && !serverRevoked) {
            tryStartViewing()
        }
    }

    fun setSelfHostAddress(text: String) {
        if (PairingBook.looksMalicious(text)) {
            selfHostAddress = ""
            fetchCount += 0
            localPairingNote = "地址占位含不受支持的文本。已拒绝，未打开、未访问、未下载。"
            return
        }
        selfHostAddress = text.trim()
        localPairingNote = "已保存自托管地址占位。不会用它发起连接。"
    }

    fun injectCertFailure() {
        if (mode != ConnectionMode.SelfHosted) return
        certFailed = true
        transport = TransportState.Failed
        failureNote = "证书校验失败（模拟）。未降级为明文，未连接。"
        stopSamples()
        clearSamples("证书失败后已清除演示波形。")
        check(!plaintextFallback)
    }

    fun observePaymentReturn() {
        returnObserved = true
        entitlementNote = "已观察到模拟支付回跳。权益保持「${DemoCopy.entitlement(entitlement)}」，没有开通。"
    }

    fun serverOrder(next: OrderPlaceholder) {
        serverOrder = next
        entitlementNote = "服务端订单占位已更新为 ${DemoCopy.order(next)}。权益未跟着改变。"
    }

    fun serverEntitlement(next: EntitlementPlaceholder) {
        entitlement = next
        entitlementNote = "服务端权益占位已更新为 ${DemoCopy.entitlement(next)}。"
        if (mode == ConnectionMode.OfficialManaged && next != EntitlementPlaceholder.Active) {
            stopSamples()
            clearSamples("官方托管权益占位不允许继续追加样本，缓冲已清除。")
            if (transport == TransportState.Up) transport = TransportState.Idle
            liveEpoch = null
        } else if (mode == ConnectionMode.OfficialManaged && next == EntitlementPlaceholder.Active) {
            val record = activeRequestId?.let { book.record(it) }
            if (record?.approval == Approval.ApprovedViewer && !serverRevoked) tryStartViewing()
        }
    }

    fun beginSearch(query: String): Long {
        searchToken += 1
        searchEpoch = epoch
        pendingQuery = query
        return searchToken
    }

    fun finishSearch(token: Long, epochAtStart: Long) {
        if (token != searchToken || epochAtStart != epoch || epochAtStart != searchEpoch) {
            staleDropped += 1
            return
        }
        appliedQuery = pendingQuery
    }

    fun cancelSearch() {
        searchToken += 1
    }

    fun demonstrateStaleSearch() {
        val token = beginSearch("旧查询")
        finishSearch(token, searchEpoch - 1)
    }

    fun setProjectEmpty(empty: Boolean) {
        projectEmpty = empty
    }

    fun submitControlAttempt() {
        commands.localReject("观察者不可执行控制。动作未送出，没有云回执。")
        publishUi()
    }

    fun injectReceipt(raw: CloudRaw) {
        commands.inject(raw)
        publishUi()
    }

    fun setPcCommandConfirm(id: String, confirm: PcCommandConfirm) {
        commands.setPcConfirm(id, confirm)
        publishUi()
    }

    fun toggleFreeze() {
        frozenChannels = if (frozenChannels == null) {
            published.map { windowOf(it) }
        } else {
            null
        }
        publishUi()
    }

    fun zoomIn() {
        zoomStep = (zoomStep + 1).coerceAtMost(4)
        publishUi()
    }

    fun zoomOut() {
        zoomStep = (zoomStep - 1).coerceAtLeast(0)
        publishUi()
    }

    fun rejectExtraSubscription() {
        extraRejected += 1
    }

    fun holdFreshness(next: Freshness?) {
        freshnessHold = next
    }

    fun onGeneratorTick() {
        generatorTicks += 1
        if (backgroundPaused) return
        rings.tick(clock())
    }

    fun publishUi() {
        published = rings.rings.map { it.snapshot() }
        uiPublishes += 1
    }

    fun onBackground() {
        wasBackgrounded = true
        backgroundPaused = true
        rings.paused = true
        lifecycleNote = "已进入后台，演示采样暂停。命令未重放，原始云态未改写。"
    }

    fun onForeground() {
        if (!wasBackgrounded) return
        wasBackgrounded = false
        val record = activeRequestId?.let { book.record(it) }
        if (isViewingLive(record) && transport == TransportState.Up) {
            rings.armGap()
            backgroundPaused = false
            rings.paused = false
            lifecycleNote = "回到前台已复核来源。曲线先记缺口再继续。命令未重放。"
        } else {
            backgroundPaused = true
            rings.paused = true
            lifecycleNote = "回到前台复核未通过，采样保持暂停。命令未重放。"
        }
        publishUi()
    }

    fun samples(channel: Int): List<Sample> = rings.rings[channel].snapshot()

    fun publishedSamples(channel: Int): List<Sample> = published[channel]

    fun exportSaved(): List<SavedDeviceEntry> = savedDevices.toList()

    fun bindingForTest(): Binding? = currentBinding()

    fun grantCreationsForTest(requestId: String): Int = book.grantCreations(requestId)

    private fun tryStartViewing() {
        val block = blockReason()
        if (block != null) {
            liveEpoch = null
            rings.enabled = false
            if (transport != TransportState.Failed && transport != TransportState.Down) {
                transport = TransportState.Idle
            }
            localPairingNote = block
            publishUi()
            return
        }
        transport = TransportState.Up
        failureNote = null
        liveEpoch = epoch
        rings.enabled = true
        rings.paused = backgroundPaused
        rings.retarget(epoch, sourceKey())
        published = List(DemoOnlySettings.MAX_CHANNELS) { emptyList() }
        frozenChannels = null
        localPairingNote = "本代数已允许观察。控制仍然不可执行。"
        sourceChangeNote = "观察源：${sourceKey()}。旧样本已清掉后再开始演示波形。"
        publishUi()
    }

    private fun blockReason(): String? {
        if (serverRevoked) return "记录已吊销，不能开始观察。"
        if (lanFailed && mode == ConnectionMode.LanDirect) {
            return "局域网失败仍在。保持局域网直连 PC，没有改走官方云。"
        }
        if (certFailed && mode == ConnectionMode.SelfHosted) {
            return "证书失败仍在。未降级明文。"
        }
        if (mode == ConnectionMode.OfficialManaged && entitlement != EntitlementPlaceholder.Active) {
            return "官方托管的权益占位还不是 active。支付回跳不会开通观察。"
        }
        return null
    }

    private fun isViewingLive(record: PairingRecord?): Boolean =
        record?.approval == Approval.ApprovedViewer &&
            record.role == Role.Viewer &&
            liveEpoch == epoch &&
            !serverRevoked &&
            transport == TransportState.Up &&
            !lanFailed &&
            !certFailed &&
            (mode != ConnectionMode.OfficialManaged || entitlement == EntitlementPlaceholder.Active)

    private fun stopSamples() {
        rings.enabled = false
        liveEpoch = null
    }

    private fun dropLiveView(note: String) {
        liveEpoch = null
        rings.enabled = false
        acceptingPc = false
        if (transport == TransportState.Up) transport = TransportState.Down
        localPairingNote = note
    }

    private fun retarget(note: String) {
        retarget { note }
    }

    private fun retarget(note: (Long) -> String) {
        epoch += 1
        rings.retarget(epoch, sourceKey())
        published = List(DemoOnlySettings.MAX_CHANNELS) { emptyList() }
        frozenChannels = null
        sourceChangeNote = note(epoch)
        publishUi()
    }

    private fun clearSamples(note: String) {
        rings.clearAndKeepSource()
        published = List(DemoOnlySettings.MAX_CHANNELS) { emptyList() }
        frozenChannels = null
        sourceChangeNote = note
        publishUi()
    }

    private fun note(attempt: Attempt) {
        lastAttempt = attempt
        localPairingNote = DemoCopy.explain(attempt.code)
    }

    private fun currentBinding(): Binding? {
        val id = activeRequestId ?: return null
        return book.record(id)?.binding
    }

    private fun sourceKey(): String = "${mode.name}#e$epoch"

    private fun health(record: PairingRecord?, viewing: Boolean): HealthLayers {
        val transportText = when (transport) {
            TransportState.Idle -> "空闲。尚未建立本代数观察。"
            TransportState.Up -> "进程内模拟可用。没有真实套接字。"
            TransportState.Down -> "已断开。需要重新核验后才能观察。"
            TransportState.Failed -> "失败。${failureNote.orEmpty()}"
        }
        val pairingText = when {
            preview?.rejected == true -> "输入已拒绝。${preview?.rejectCode}"
            record == null -> "无配对记录。"
            else -> "记录审批：${DemoCopy.approval(record.approval)}。$localPairingNote"
        }
        val channelText = "代数 $epoch · ${mode.label} · 通道占位 ${sourceKey()}"
        val acquisitionText = when {
            viewing && rings.enabled -> "演示采样进行中。订阅不是启动真实采集。"
            else -> "未在追加演示样本。"
        }
        val freshnessText = when (freshness(viewing)) {
            Freshness.NoSample -> "无样本"
            Freshness.Normal -> "正常"
            Freshness.Stale -> "陈旧 stale"
            Freshness.Gap -> "缺口 gap"
            Freshness.Timeout -> "超时"
            Freshness.Paused -> "已暂停"
        }
        return HealthLayers(transportText, pairingText, channelText, acquisitionText, freshnessText)
    }

    private fun freshness(viewing: Boolean): Freshness {
        freshnessHold?.let { return it }
        if (backgroundPaused) return Freshness.Paused
        val tails = published.mapNotNull { it.lastOrNull() }
        if (tails.isEmpty()) return Freshness.NoSample
        if (tails.any { it.gapBefore } || transport == TransportState.Down) return Freshness.Gap
        val newest = tails.maxBy { it.tMillis }
        if (viewing && clock() - newest.tMillis > DemoOnlySettings.STALE_AFTER_MS) return Freshness.Stale
        return Freshness.Normal
    }

    private fun roleLabel(record: PairingRecord?, viewing: Boolean): String = when {
        viewing -> "角色：观察者 viewer。控制不可执行。"
        record?.approval == Approval.ApprovedViewer -> "记录是观察者，本会话尚未恢复查看。控制不可执行。"
        else -> "角色：未获得观察。控制不可执行。"
    }

    private fun project(viewing: Boolean): ProjectView {
        if (!viewing) {
            return ProjectView(
                allowed = false,
                empty = projectEmpty,
                projectName = DemoCatalog.PROJECT,
                rows = emptyList(),
                appliedQuery = appliedQuery,
                staleDropped = staleDropped,
            )
        }
        if (projectEmpty) {
            return ProjectView(true, true, DemoCatalog.PROJECT, emptyList(), appliedQuery, staleDropped)
        }
        return ProjectView(
            allowed = true,
            empty = false,
            projectName = DemoCatalog.PROJECT,
            rows = DemoCatalog.filtered(appliedQuery),
            appliedQuery = appliedQuery,
            staleDropped = staleDropped,
        )
    }

    private fun numbers(viewing: Boolean): List<ValueCard> {
        return DemoCatalog.rows.filter { it.channel != null }.map { row ->
            val channel = row.channel ?: 0
            val sample = published.getOrNull(channel)?.lastOrNull()
            val quality = when {
                sample == null -> "无样本"
                transport == TransportState.Failed -> "失败。没有改成一条成功曲线。"
                transport == TransportState.Down -> "已断开。这是断开前的值，不会继续追加。"
                !viewing -> "当前不可观察。"
                sample.gapBefore -> "缺口"
                freshness(viewing) == Freshness.Stale -> "陈旧"
                row.name.endsWith("zero") -> "良好。数值是 0，不是无样本。"
                else -> "良好"
            }
            ValueCard(
                name = row.name,
                unit = row.unit,
                typeName = row.type,
                source = sourceKey(),
                timeSource = DemoCopy.TIME_SOURCE,
                updatedAt = sample?.let { "演示毫秒 ${it.tMillis}" } ?: "无更新",
                quality = quality,
                displayValue = sample?.let { formatValue(it.value) } ?: "无样本",
            )
        }
    }

    private fun waveView(): WaveView {
        val base = frozenChannels ?: published
        val display = base.map { windowOf(it) }
        return WaveView(
            liveCount = published.sumOf { it.size },
            channels = display,
            frozen = frozenChannels != null,
            zoomStep = zoomStep,
            gapCount = display.sumOf { channel -> channel.count { it.gapBefore } },
            sourceLabel = sourceKey(),
            sourceChangeNote = sourceChangeNote,
            extraRejected = extraRejected,
        )
    }

    private fun windowOf(samples: List<Sample>): List<Sample> {
        if (samples.isEmpty()) return samples
        val denom = 1 shl zoomStep
        val count = (samples.size / denom).coerceAtLeast(minOf(16, samples.size))
        return samples.takeLast(count)
    }

    private fun formatValue(value: Float): String {
        val rounded = (value * 10f).toInt() / 10f
        return if (rounded == 0f) "0" else rounded.toString()
    }
}
