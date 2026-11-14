'use strict';

var fixtures = require('./fixtures.js');
var qr = require('./qr.js');
var waveform = require('./waveform.js');

var COMMAND_STATES = [
  'rejected',
  'pending_pc_confirm',
  'received',
  'accepted',
  'running',
  'succeeded',
  'failed',
  'unknown',
];

var EXECUTABLES = ['RUN_CTL', 'flash', 'shell', 'write', 'arbitrary'];

var BUFFER_LIMIT = 180;

var TRANSPORT_LABEL = {
  not_connected: '未连接',
  failed: '失败（没有回退到模拟在线）',
  restricted: '平台限制（未连接）',
  disconnected: '已断开（不是命令失败，也不会重放）',
};

var AUTH_LABEL = {
  anonymous: '未配对',
  claimed_unverified: '已声称，未验证',
  verified_preview: '已验证预览（尚未授权）',
  pending: '等待 PC 批准',
  viewer: '观察者',
  denied: '已拒绝',
  expired: '已过期',
  revoked: '已撤销',
};

var PC_LABEL = {
  unknown: '未知',
  claimed: '仅声称',
  verified_present: '夹具中的 PC 身份',
  approval_pending: '等待 PC 本机确认',
  approval_denied: 'PC 已拒绝',
};

var TARGET_LABEL = {
  none: '无目标',
  simulated_fixture: '模拟夹具目标（不是在线 MCU）',
};

var FRESH_LABEL = {
  none: '无样本',
  fresh: '新鲜（本地合成）',
  paused: '已暂停',
  gap: '有缺口（不是 0，也不连线）',
  stale_cleared: '已清除过期数据',
};

var GENERATOR_LABEL = {
  stopped: '停止',
  running: '本地合成播放中（不是链路）',
  paused: '已暂停（无后台保活）',
};

var COMMAND_CATALOG = [
  { id: 'rejected', label: '已拒绝', note: '只展示拒绝，不执行' },
  { id: 'pending_pc_confirm', label: '等待 PC 本地确认', note: '手机不能代替 PC 确认' },
  { id: 'received', label: '已接收', note: '展示态' },
  { id: 'accepted', label: '已接受', note: '展示态' },
  { id: 'running', label: '执行中进度', note: '没有真实进度回传' },
  { id: 'succeeded', label: '已成功', note: '本地置信仍是仅展示' },
  { id: 'failed', label: '已失败', note: '这是展示出来的失败。断开连接不会自动写成这个状态' },
  { id: 'unknown', label: '未知', note: '未知命令不会被重放' },
];

function createDemoStore(options) {
  options = options || {};
  var ttlMs = typeof options.ttlMs === 'number' ? options.ttlMs : fixtures.DEMO_SCHEMA.ttlMs;
  var nowFn = options.now || function () { return Date.now(); };
  var selfClaimant = options.claimantId || 'phone-self';
  var bufferLimit = options.bufferLimit || BUFFER_LIMIT;
  var state = freshState();

  function freshState() {
    return {
      mode: 'lan_direct',
      transport: 'not_connected',
      auth: 'anonymous',
      pc: 'unknown',
      target: 'none',
      freshness: 'none',
      generator: 'stopped',
      role: 'none',
      controlEnabled: false,
      keepalive: false,
      usingMockFallback: false,
      fellBack: false,
      networkCalls: 0,
      liveLink: false,
      generation: 1,
      rings: {},
      lastValues: {},
      gapCount: 0,
      sampleSeq: 0,
      streamDesired: false,
      hidden: false,
      redemptionCount: 0,
      grantCount: 0,
      expiresAt: null,
      forceExpired: false,
      receipt: null,
      fixture: null,
      claimed: null,
      verified: null,
      revoked: false,
      devices: [],
      deviceSeq: 1,
      activeDeviceId: null,
      selectedProjectId: null,
      selectedModuleId: null,
      selectedVariableId: null,
      command: {
        rawState: 'none',
        localConfidence: 'none',
        executed: false,
        confirmed: false,
        attemptCount: 0,
        disconnectTreatedAsFailure: false,
        replayed: false,
        rejectedClass: '',
      },
      payment: {
        paymentReturn: 'none',
        serverPaid: 'unknown',
        entitlement: 'inactive',
      },
      platformRestriction: '',
      notice: '这是本地夹具 DEMO。没有网络通道。',
      logs: [],
    };
  }

  function now() {
    return nowFn();
  }

  function isExpired() {
    if (state.forceExpired) return true;
    if (state.expiresAt == null) return false;
    return now() >= state.expiresAt;
  }

  function note(text) {
    state.notice = text;
  }

  function clearRings() {
    state.rings = {};
    state.lastValues = {};
    state.gapCount = 0;
  }

  function stopGenerator() {
    state.streamDesired = false;
    state.generator = 'stopped';
    state.keepalive = false;
    clearRings();
    state.freshness = 'stale_cleared';
    state.target = 'none';
  }

  function canBrowse() {
    return state.role === 'viewer' && state.auth === 'viewer' && !state.revoked;
  }

  function canGenerate() {
    return state.mode === 'lan_direct' &&
      state.transport === 'not_connected' &&
      state.role === 'viewer' &&
      state.auth === 'viewer' &&
      !state.revoked &&
      !state.hidden &&
      state.networkCalls === 0 &&
      state.liveLink === false;
  }

  function pushRing(ring, entry) {
    ring.push(entry);
    while (ring.length > bufferLimit) ring.shift();
  }

  function ensureSelection() {
    if (state.selectedVariableId) return;
    var variables = fixtures.allVariables();
    if (!variables.length) return;
    state.selectedProjectId = variables[0].projectId;
    state.selectedModuleId = variables[0].moduleId;
    state.selectedVariableId = variables[0].id;
  }

  function clearSelection() {
    state.selectedProjectId = null;
    state.selectedModuleId = null;
    state.selectedVariableId = null;
  }

  function selectedVariable() {
    var variables = fixtures.allVariables();
    var i;
    for (i = 0; i < variables.length; i += 1) {
      if (variables[i].id === state.selectedVariableId) return variables[i];
    }
    return null;
  }

  function findDevice(id) {
    var i;
    for (i = 0; i < state.devices.length; i += 1) {
      if (state.devices[i].id === id) return state.devices[i];
    }
    return null;
  }

  function displayFor(id) {
    if (state.freshness !== 'fresh') return '—';
    var value = state.lastValues[id];
    if (typeof value !== 'number') return '—';
    return String(value);
  }

  function projectTree() {
    if (!canBrowse()) return [];
    var projects = [];
    var p;
    for (p = 0; p < fixtures.PROJECTS.length; p += 1) {
      var project = fixtures.PROJECTS[p];
      var modules = [];
      var m;
      for (m = 0; m < project.modules.length; m += 1) {
        var module = project.modules[m];
        var variables = [];
        var v;
        for (v = 0; v < module.variables.length; v += 1) {
          var variable = module.variables[v];
          variables.push({
            id: variable.id,
            name: variable.name,
            type: variable.type,
            unit: variable.unit,
            selected: variable.id === state.selectedVariableId,
          });
        }
        modules.push({
          id: module.id,
          name: module.name,
          selected: module.id === state.selectedModuleId,
          variables: variables,
        });
      }
      projects.push({
        id: project.id,
        name: project.name,
        selected: project.id === state.selectedProjectId,
        modules: modules,
      });
    }
    return projects;
  }

  function readingRows() {
    if (!canBrowse()) return [];
    var variables = fixtures.allVariables();
    var rows = [];
    var i;
    for (i = 0; i < variables.length; i += 1) {
      var variable = variables[i];
      rows.push({
        id: variable.id,
        name: variable.name,
        type: variable.type,
        unit: variable.unit,
        moduleName: variable.moduleName,
        projectName: variable.projectName,
        selected: variable.id === state.selectedVariableId,
        display: displayFor(variable.id),
      });
    }
    return rows;
  }

  function selectedRing() {
    if (!state.selectedVariableId || !state.rings[state.selectedVariableId]) return [];
    return state.rings[state.selectedVariableId].map(cloneEntry);
  }

  function cloneEntry(entry) {
    if (!entry || entry.kind === 'gap') {
      return { kind: 'gap', generation: entry ? entry.generation : state.generation };
    }
    return {
      kind: 'point',
      t: entry.t,
      value: entry.value,
      generation: entry.generation,
      variableId: entry.variableId,
    };
  }

  function resetPairing() {
    state.fixture = null;
    state.claimed = null;
    state.verified = null;
    state.receipt = null;
    state.redemptionCount = 0;
    state.grantCount = 0;
    state.expiresAt = null;
    state.forceExpired = false;
    state.auth = 'anonymous';
    state.role = 'none';
    state.controlEnabled = false;
    state.revoked = false;
    state.streamDesired = false;
    state.generator = 'stopped';
    state.keepalive = false;
    clearRings();
    clearSelection();
    state.freshness = 'none';
    state.target = 'none';
    state.activeDeviceId = null;
  }

  function submitShortCode(text) {
    var parsed = qr.parseDemoCode(text);
    if (!parsed.ok) {
      note('短码被拒绝：' + parsed.reason + '。没有发起网络请求，也没有记录原始内容。');
      return {
        ok: false,
        reason: parsed.reason,
        network: false,
        followedUrl: false,
        rawLogged: false,
        grantsControl: false,
        longTermSecret: false,
      };
    }
    var fixture = fixtures.fixtureById(parsed.fixtureId);
    if (!fixture) {
      note('未知短码，已关闭。');
      return {
        ok: false,
        reason: 'unknown_code',
        network: false,
        followedUrl: false,
        rawLogged: false,
        grantsControl: false,
        longTermSecret: false,
      };
    }
    resetPairing();
    state.fixture = fixture;
    state.claimed = { server: fixture.claimed.server, pc: fixture.claimed.pc };
    state.auth = 'claimed_unverified';
    state.pc = 'claimed';
    note('已识别演示短码（已脱敏）。短码不是控制权限，也没有长期密钥。声称信息还没验证。');
    return {
      ok: true,
      reason: 'recognized_fixture',
      network: false,
      followedUrl: false,
      rawLogged: false,
      grantsControl: false,
      longTermSecret: false,
      redacted: 'OSC-******',
    };
  }

  function submitFixtureKey(key) {
    var code = fixtures.codeForKey(key);
    if (!code) {
      return {
        ok: false,
        reason: 'unknown_fixture',
        network: false,
        followedUrl: false,
        rawLogged: false,
        grantsControl: false,
      };
    }
    return submitShortCode(code);
  }

  function verifyClaim() {
    if (!state.fixture || state.auth !== 'claimed_unverified') {
      note('没有待验证的声称。');
      return { ok: false, reason: 'nothing_claimed', network: false, grantsControl: false };
    }
    var fixture = state.fixture;
    if (fixture.expired) {
      state.auth = 'expired';
      state.verified = null;
      state.pc = 'unknown';
      state.role = 'none';
      state.controlEnabled = false;
      note('这份夹具已过期。不会延长 TTL，也不会授权。');
      return { ok: false, reason: 'expired', network: false, grantsControl: false };
    }
    if (fixture.revoked) {
      state.auth = 'revoked';
      state.revoked = true;
      state.verified = { server: fixture.verified.server, pc: fixture.verified.pc };
      state.pc = 'verified_present';
      state.role = 'none';
      state.controlEnabled = false;
      stopGenerator();
      note('身份已撤销。无访问。');
      return { ok: false, reason: 'revoked', network: false, grantsControl: false };
    }
    if (fixture.claimantId !== selfClaimant) {
      state.auth = 'denied';
      state.verified = null;
      state.role = 'none';
      state.controlEnabled = false;
      state.pc = 'approval_denied';
      note('声称者与本机夹具身份不同，已拒绝。');
      return { ok: false, reason: 'different_claimant', network: false, grantsControl: false };
    }
    state.auth = 'verified_preview';
    state.verified = { server: fixture.verified.server, pc: fixture.verified.pc };
    state.pc = 'verified_present';
    state.role = 'none';
    state.controlEnabled = false;
    note('验证预览完成。这还不是授权，默认也不是控制方。');
    return { ok: true, reason: 'verified_preview', network: false, grantsControl: false, role: 'none' };
  }

  function requestPairing() {
    if (state.revoked || (state.fixture && state.fixture.revoked)) {
      state.auth = 'revoked';
      state.role = 'none';
      state.controlEnabled = false;
      note('已撤销，不能发起配对。');
      return { ok: false, reason: 'revoked', network: false, grantCount: state.grantCount };
    }
    if (state.fixture && state.fixture.expired) {
      state.auth = 'expired';
      note('已过期，不能发起配对。');
      return { ok: false, reason: 'expired', network: false, redemptionCount: state.redemptionCount, expiresAt: state.expiresAt };
    }
    if (state.auth === 'pending') {
      if (isExpired()) {
        state.auth = 'expired';
        if (state.receipt) state.receipt = Object.assign({}, state.receipt, { auth: 'expired' });
        note('等待已超过演示 TTL。没有第二次兑换，也没有延长 TTL。');
        return { ok: false, reason: 'expired', network: false, redemptionCount: state.redemptionCount, expiresAt: state.expiresAt, grantCount: state.grantCount };
      }
      note('仍在等待 PC。不会再次兑换。');
      return { ok: false, reason: 'already_pending', network: false, redemptionCount: state.redemptionCount, expiresAt: state.expiresAt, grantCount: state.grantCount };
    }
    if (state.auth === 'denied' && state.receipt) {
      note('先前的拒绝还在。请恢复同一请求，而不是再次兑换。');
      return { ok: false, reason: 'use_restore_not_rerequest', network: false, redemptionCount: state.redemptionCount, expiresAt: state.expiresAt, grantCount: state.grantCount };
    }
    if (state.auth !== 'verified_preview' || !state.fixture || !state.verified) {
      note('要先完成验证预览，才能请求 PC 批准。');
      return { ok: false, reason: 'not_verified', network: false };
    }
    if (state.fixture.claimantId !== selfClaimant) {
      state.auth = 'denied';
      state.role = 'none';
      note('不同声称者，已拒绝。');
      return { ok: false, reason: 'different_claimant', network: false };
    }
    state.redemptionCount += 1;
    state.expiresAt = now() + ttlMs;
    state.auth = 'pending';
    state.pc = 'approval_pending';
    state.role = 'none';
    state.controlEnabled = false;
    state.grantCount = 0;
    state.receipt = {
      identityId: state.fixture.identityId,
      requestId: state.fixture.requestId,
      bindingId: state.fixture.bindingId,
      claimantId: selfClaimant,
      auth: 'pending',
    };
    note('已请求配对，等待 PC 批准。本机不能自己批准。默认不会变成控制方。');
    return {
      ok: true,
      reason: 'pending',
      network: false,
      role: 'none',
      redemptionCount: state.redemptionCount,
      expiresAt: state.expiresAt,
      grantCount: state.grantCount,
    };
  }

  function approveAsSelf() {
    note('本机自行批准已被拒绝。配对只能由模拟的对端 PC 决定。');
    return {
      ok: false,
      reason: 'self_approval_forbidden',
      executed: false,
      network: false,
      auth: state.auth,
      role: state.role,
      grantCount: state.grantCount,
    };
  }

  function applyPcDecision(decision) {
    if (decision !== 'approved' && decision !== 'denied') {
      return { ok: false, reason: 'unknown_decision', executed: false, network: false };
    }
    if (state.auth !== 'pending' || !state.receipt) {
      note('没有等待中的配对请求。');
      return { ok: false, reason: 'not_pending', executed: false, network: false, grantCount: state.grantCount };
    }
    if (isExpired()) {
      state.auth = 'expired';
      state.receipt = Object.assign({}, state.receipt, { auth: 'expired' });
      state.role = 'none';
      state.controlEnabled = false;
      note('请求已过期。PC 批准不会补发授权，也不会延长 TTL。');
      return { ok: false, reason: 'expired', executed: false, network: false, grantCount: state.grantCount, redemptionCount: state.redemptionCount, expiresAt: state.expiresAt };
    }
    if (decision === 'denied') {
      state.auth = 'denied';
      state.receipt = Object.assign({}, state.receipt, { auth: 'denied' });
      state.pc = 'approval_denied';
      state.role = 'none';
      state.controlEnabled = false;
      note('模拟对端 PC 已拒绝。没有授权。');
      return { ok: true, reason: 'denied', executed: false, network: false, role: 'none', grantCount: state.grantCount };
    }
    if (state.revoked) {
      state.auth = 'revoked';
      state.role = 'none';
      state.controlEnabled = false;
      note('已撤销，不能批准。');
      return { ok: false, reason: 'revoked', executed: false, network: false };
    }
    state.auth = 'viewer';
    state.role = 'viewer';
    state.controlEnabled = false;
    state.grantCount += 1;
    state.receipt = Object.assign({}, state.receipt, { auth: 'viewer' });
    state.pc = 'verified_present';
    ensureSelection();
    note('模拟对端 PC 已批准。角色是观察者。控制保持关闭。传输仍然未连接。');
    return { ok: true, reason: 'viewer', executed: false, network: false, role: 'viewer', controlEnabled: false, grantCount: state.grantCount };
  }

  function restore(input) {
    input = input || {};
    if (!state.receipt) {
      note('没有可恢复的配对请求。');
      return { ok: false, reason: 'nothing_to_restore', network: false };
    }
    var same = input.identityId === state.receipt.identityId &&
      input.requestId === state.receipt.requestId &&
      input.bindingId === state.receipt.bindingId;
    if (!same) {
      note('身份、请求或绑定不一致，已拒绝。原请求未改。');
      return {
        ok: false,
        reason: 'binding_mismatch',
        network: false,
        auth: state.auth,
        redemptionCount: state.redemptionCount,
        grantCount: state.grantCount,
        expiresAt: state.expiresAt,
      };
    }
    if (input.claimantId !== state.receipt.claimantId) {
      note('不同声称者不能恢复这条请求。原请求保持不变。');
      return {
        ok: false,
        reason: 'different_claimant',
        network: false,
        auth: 'denied',
        redemptionCount: state.redemptionCount,
        grantCount: state.grantCount,
        expiresAt: state.expiresAt,
      };
    }
    if (state.revoked || (state.fixture && state.fixture.revoked)) {
      state.auth = 'revoked';
      state.role = 'none';
      state.controlEnabled = false;
      stopGenerator();
      note('绑定已撤销，恢复被拒绝。');
      return { ok: false, reason: 'revoked', network: false, grantCount: state.grantCount, expiresAt: state.expiresAt };
    }
    if (isExpired()) {
      var kept = state.expiresAt;
      state.auth = 'expired';
      state.role = 'none';
      state.controlEnabled = false;
      state.receipt = Object.assign({}, state.receipt, { auth: 'expired' });
      stopGenerator();
      note('请求已过期。TTL 没有延长。');
      return { ok: false, reason: 'expired', network: false, expiresAt: kept, grantCount: state.grantCount, redemptionCount: state.redemptionCount };
    }
    if (state.receipt.auth !== 'pending' && state.receipt.auth !== 'denied') {
      note('只有等待中或已拒绝的同一请求可以恢复，且不会再次授权。');
      return { ok: false, reason: 'not_restorable', network: false, grantCount: state.grantCount, redemptionCount: state.redemptionCount, expiresAt: state.expiresAt };
    }
    state.auth = state.receipt.auth;
    state.role = 'none';
    state.controlEnabled = false;
    state.pc = state.auth === 'pending' ? 'approval_pending' : 'approval_denied';
    note(state.auth === 'pending' ? '已恢复等待中的同一请求。没有第二次兑换，没有新授权，TTL 未延长。' : '已恢复先前的拒绝。没有新授权，TTL 未延长。');
    return {
      ok: true,
      reason: 'restored',
      network: false,
      auth: state.auth,
      redemptionCount: state.redemptionCount,
      grantCount: state.grantCount,
      expiresAt: state.expiresAt,
    };
  }

  function rememberDevice() {
    if (state.role !== 'viewer' || state.revoked || !state.fixture) {
      note('只有未撤销的观察者会话可以记住模拟设备。');
      return { ok: false, reason: 'not_viewer' };
    }
    var id = 'dev-' + String(state.deviceSeq);
    state.deviceSeq += 1;
    var device = {
      id: id,
      label: state.verified ? state.verified.pc : '模拟设备',
      bindingId: state.fixture.bindingId,
      identityId: state.fixture.identityId,
      role: 'viewer',
      revoked: false,
    };
    state.devices.push(device);
    state.activeDeviceId = id;
    note('已记住模拟设备。角色仍是观察者。');
    return { ok: true, id: id };
  }

  function revokeDevice(id) {
    var device = findDevice(id);
    if (!device) return { ok: false, reason: 'unknown_device', access: false };
    device.revoked = true;
    state.revoked = true;
    state.auth = 'revoked';
    state.role = 'none';
    state.controlEnabled = false;
    state.activeDeviceId = id;
    state.generation += 1;
    stopGenerator();
    clearSelection();
    note('模拟设备已撤销。目录、读数和波形都不再提供。');
    return { ok: true, revoked: true, access: false, role: 'none' };
  }

  function selectDevice(id) {
    var device = findDevice(id);
    if (!device) return { ok: false, reason: 'unknown_device', access: false };
    state.activeDeviceId = id;
    if (device.revoked) {
      state.revoked = true;
      state.auth = 'revoked';
      state.role = 'none';
      state.controlEnabled = false;
      stopGenerator();
      clearSelection();
      note('该设备已撤销，不能恢复访问。');
      return { ok: false, reason: 'revoked', access: false, role: 'none' };
    }
    state.revoked = false;
    state.auth = 'viewer';
    state.role = 'viewer';
    state.controlEnabled = false;
    state.pc = 'verified_present';
    ensureSelection();
    note('已选中记住的观察者设备。控制仍然关闭。这不是新的兑换。');
    return { ok: true, role: 'viewer', controlEnabled: false, access: true, grantCount: state.grantCount, redemptionCount: state.redemptionCount };
  }

  function setMode(mode) {
    if (!fixtures.MODES[mode]) {
      return { ok: false, reason: 'unknown_mode', mode: state.mode, fellBack: false };
    }
    state.mode = mode;
    if (mode !== 'lan_direct' && (state.generator === 'running' || state.streamDesired)) {
      state.streamDesired = false;
      state.generator = 'stopped';
      state.keepalive = false;
      clearRings();
      state.freshness = 'stale_cleared';
      state.target = 'none';
    }
    note(fixtures.MODES[mode].label + '。模式不会自动切换，也没有因此发起连接。');
    return { ok: true, mode: state.mode, transport: state.transport, fellBack: false, network: false };
  }

  function classifyEndpoint(input) {
    var host = input && typeof input.host === 'string' ? input.host.trim() : '';
    var isBareIp = /^(\d{1,3}\.){3}\d{1,3}$/.test(host);
    return { isBareIp: isBareIp };
  }

  function attemptConnect(input) {
    var endpoint = classifyEndpoint(input);
    state.usingMockFallback = false;
    state.fellBack = false;
    state.liveLink = false;
    stopGenerator();
    if (state.mode === 'selfhost_relay') {
      state.transport = 'restricted';
      state.platformRestriction = endpoint.isBareIp
        ? '检测到裸 IP 形态地址，已丢弃且未拨号。微信小程序不能把裸公网 IP 当作 request 或 socket 合法域名。本 DEMO 没有连接。'
        : '自建中继在这个 DEMO 里不会连接。生产环境里，裸公网 IP 也不是合法的微信 request 或 socket 域名。';
      note(state.platformRestriction);
      return {
        ok: false,
        reason: 'wechat_platform_restriction',
        network: false,
        fellBack: false,
        pretendConnected: false,
        mode: state.mode,
        transport: state.transport,
      };
    }
    state.transport = 'failed';
    state.platformRestriction = '';
    note('连接失败。此构建没有网络通道。失败不会回退到模拟在线数据，也不会自动切换模式。');
    return {
      ok: false,
      reason: 'no_network_path',
      network: false,
      fellBack: false,
      pretendConnected: false,
      mode: state.mode,
      transport: state.transport,
    };
  }

  function acknowledgeIdle() {
    if (state.transport === 'failed' || state.transport === 'disconnected' || state.transport === 'restricted') {
      state.transport = 'not_connected';
    }
    state.usingMockFallback = false;
    state.fellBack = false;
    state.liveLink = false;
    state.streamDesired = false;
    state.generator = 'stopped';
    state.keepalive = false;
    clearRings();
    state.freshness = 'stale_cleared';
    state.target = 'none';
    note('传输回到“未连接”。没有重放命令，没有恢复链路，也没有开始合成数据。');
    return { ok: true, transport: state.transport, fellBack: false, generator: 'stopped', network: false };
  }

  function disconnect() {
    var raw = state.command.rawState;
    state.transport = 'disconnected';
    state.liveLink = false;
    state.streamDesired = false;
    state.generator = 'stopped';
    state.keepalive = false;
    clearRings();
    state.freshness = 'stale_cleared';
    state.target = 'none';
    state.command.disconnectTreatedAsFailure = false;
    state.command.replayed = false;
    state.command.rawState = raw;
    note('传输已断开。命令原始状态保持不变，断开没有被当成确认失败，也没有重放。');
    return {
      ok: true,
      transport: 'disconnected',
      rawState: state.command.rawState,
      treatedAsFailure: false,
      replayed: false,
      network: false,
    };
  }

  function startDemoStream() {
    if (state.mode === 'selfhost_relay') {
      note('自建中继模式不会用本地曲线假装已经连上。');
      return { ok: false, reason: 'wechat_platform_restriction', fellBack: false, network: false, mode: state.mode };
    }
    if (state.mode !== 'lan_direct') {
      note('当前模式没有本地合成播放。不会自动改回局域网模式。');
      return { ok: false, reason: 'mode_blocks_generator', fellBack: false, network: false, mode: state.mode };
    }
    if (state.transport !== 'not_connected') {
      note('传输不是“未连接”。拒绝把失败、限制或断开伪装成模拟在线数据。');
      return { ok: false, reason: 'refusing_mock_fallback', fellBack: false, network: false, transport: state.transport };
    }
    if (state.role !== 'viewer' || state.auth !== 'viewer' || state.revoked) {
      note('需要未撤销的观察者会话，才会播放本地合成样本。');
      return { ok: false, reason: 'viewer_required', fellBack: false, network: false };
    }
    if (state.hidden) {
      note('页面不可见时不会播放，也没有后台保活。');
      return { ok: false, reason: 'background_blocked', fellBack: false, network: false };
    }
    state.streamDesired = true;
    state.generator = 'running';
    state.keepalive = false;
    state.liveLink = false;
    state.usingMockFallback = false;
    state.freshness = 'stale_cleared';
    ensureSelection();
    note('开始新的本地合成会话。来源是 DEMO/SIMULATED DATA。传输仍是未连接。');
    return { ok: true, reason: 'local_generator', transport: state.transport, liveLink: false, network: false };
  }

  function stopDemoStream() {
    state.streamDesired = false;
    state.generator = 'stopped';
    state.keepalive = false;
    clearRings();
    state.freshness = 'stale_cleared';
    state.target = 'none';
    note('已停止本地合成，并清除样本。');
    return { ok: true, generator: 'stopped' };
  }

  function synth(variable, seq) {
    var steps = 20;
    var t = (seq - 1) % steps;
    var span = variable.max - variable.min;
    var value = Math.round(variable.min + (span * t) / (steps - 1));
    if (value < variable.min) value = variable.min;
    if (value > variable.max) value = variable.max;
    return value;
  }

  function tickSample() {
    if (state.keepalive) return { ok: false, reason: 'keepalive_forbidden' };
    if (state.generator !== 'running' || !canGenerate()) return { ok: false, reason: 'not_running' };
    state.sampleSeq += 1;
    var variables = fixtures.allVariables();
    var i;
    for (i = 0; i < variables.length; i += 1) {
      var variable = variables[i];
      var value = synth(variable, state.sampleSeq);
      state.lastValues[variable.id] = value;
      if (!state.rings[variable.id]) state.rings[variable.id] = [];
      pushRing(state.rings[variable.id], {
        kind: 'point',
        t: state.sampleSeq,
        value: value,
        generation: state.generation,
        variableId: variable.id,
      });
    }
    state.freshness = 'fresh';
    state.target = 'simulated_fixture';
    state.liveLink = false;
    return { ok: true, seq: state.sampleSeq, network: false };
  }

  function markGap() {
    if (state.generator !== 'running' || !canGenerate()) {
      return { ok: false, reason: 'not_running' };
    }
    var variables = fixtures.allVariables();
    var i;
    for (i = 0; i < variables.length; i += 1) {
      var id = variables[i].id;
      if (!state.rings[id]) state.rings[id] = [];
      pushRing(state.rings[id], { kind: 'gap', generation: state.generation });
    }
    state.gapCount += 1;
    state.freshness = 'gap';
    note('已插入采样缺口。缺口不是 0，波形也不会把两侧连成一条线。');
    return { ok: true, gapCount: state.gapCount };
  }

  function onMonitorHide() {
    var wasHidden = state.hidden;
    state.hidden = true;
    state.keepalive = false;
    state.generator = state.streamDesired ? 'paused' : 'stopped';
    if (!wasHidden) {
      state.generation += 1;
      clearRings();
      state.freshness = 'stale_cleared';
      state.target = 'none';
    }
    return { ok: true, keepalive: state.keepalive, generation: state.generation, generator: state.generator };
  }

  function onMonitorShow() {
    state.hidden = false;
    state.keepalive = false;
    state.generation += 1;
    clearRings();
    state.target = 'none';
    if (state.streamDesired && canGenerate()) {
      state.generator = 'running';
      state.freshness = 'stale_cleared';
    } else {
      state.generator = 'stopped';
      state.freshness = 'stale_cleared';
    }
    note('监视页重新可见：旧样本已清除，这是新的本地会话，没有后台保活。');
    return { ok: true, generation: state.generation, keepalive: false, generator: state.generator };
  }

  function demonstrateCommand(raw) {
    if (COMMAND_STATES.indexOf(raw) < 0) raw = 'unknown';
    state.command.rawState = raw;
    state.command.localConfidence = 'displayed_only';
    state.command.executed = false;
    state.command.confirmed = false;
    state.command.replayed = false;
    note('正在展示命令状态“' + raw + '”。本地置信只是“仅展示”。没有执行。');
    return {
      ok: true,
      rawState: state.command.rawState,
      localConfidence: state.command.localConfidence,
      executed: false,
      confirmed: false,
      network: false,
    };
  }

  function submitExecutable(name) {
    var rejectedClass = 'arbitrary';
    if (EXECUTABLES.indexOf(name) >= 0) rejectedClass = name;
    state.command.rawState = 'rejected';
    state.command.localConfidence = 'displayed_only';
    state.command.executed = false;
    state.command.confirmed = false;
    state.command.replayed = false;
    state.command.rejectedClass = rejectedClass;
    note('控制命令被拒绝，没有执行。');
    return { ok: false, executed: false, rawState: 'rejected', rejectedClass: rejectedClass, network: false };
  }

  function replayCommand() {
    state.command.replayed = false;
    note('没有重放命令。未知状态也不会被重新发送。');
    return {
      ok: false,
      reason: 'not_replayed',
      executed: false,
      replayed: false,
      rawState: state.command.rawState,
      attemptCount: state.command.attemptCount,
      network: false,
    };
  }

  function setControl() {
    state.controlEnabled = false;
    note('控制通道保持关闭。默认只有观察者。');
    return { ok: false, reason: 'control_disabled', controlEnabled: false, executed: false };
  }

  function assignPayment(field, allowed, value) {
    if (allowed.indexOf(value) < 0) return { ok: false, reason: 'unknown' };
    state.payment[field] = value;
    return { ok: true };
  }

  function notePaymentReturn(value) {
    var result = assignPayment('paymentReturn', ['returned', 'cancelled', 'none'], value);
    if (!result.ok) return result;
    note('只记录了支付返回夹具。它不会把服务端改成已付，也不会打开权益。');
    return Object.assign({ ok: true }, copyPayment());
  }

  function noteServerPaid(value) {
    var result = assignPayment('serverPaid', ['paid', 'unpaid', 'unknown'], value);
    if (!result.ok) return result;
    note('只记录了服务端已付夹具。它不会从支付返回推导，也不会打开权益。');
    return Object.assign({ ok: true }, copyPayment());
  }

  function setEntitlementFixture(value) {
    var result = assignPayment('entitlement', ['active', 'inactive', 'unknown'], value);
    if (!result.ok) return result;
    note('只记录了权益夹具。它不会连接，也不会打开控制。');
    return Object.assign({ ok: true }, copyPayment());
  }

  function copyPayment() {
    return {
      paymentReturn: state.payment.paymentReturn,
      serverPaid: state.payment.serverPaid,
      entitlement: state.payment.entitlement,
    };
  }

  function selectProject(id) {
    if (!canBrowse()) return { ok: false, reason: 'no_access' };
    var p;
    for (p = 0; p < fixtures.PROJECTS.length; p += 1) {
      if (fixtures.PROJECTS[p].id === id) {
        state.selectedProjectId = id;
        state.selectedModuleId = fixtures.PROJECTS[p].modules[0].id;
        state.selectedVariableId = fixtures.PROJECTS[p].modules[0].variables[0].id;
        return { ok: true };
      }
    }
    return { ok: false, reason: 'unknown' };
  }

  function selectModule(id) {
    if (!canBrowse()) return { ok: false, reason: 'no_access' };
    var p;
    var m;
    for (p = 0; p < fixtures.PROJECTS.length; p += 1) {
      for (m = 0; m < fixtures.PROJECTS[p].modules.length; m += 1) {
        if (fixtures.PROJECTS[p].modules[m].id === id) {
          state.selectedProjectId = fixtures.PROJECTS[p].id;
          state.selectedModuleId = id;
          state.selectedVariableId = fixtures.PROJECTS[p].modules[m].variables[0].id;
          return { ok: true };
        }
      }
    }
    return { ok: false, reason: 'unknown' };
  }

  function selectVariable(id) {
    if (!canBrowse()) return { ok: false, reason: 'no_access' };
    var variables = fixtures.allVariables();
    var i;
    for (i = 0; i < variables.length; i += 1) {
      if (variables[i].id === id) {
        state.selectedProjectId = variables[i].projectId;
        state.selectedModuleId = variables[i].moduleId;
        state.selectedVariableId = variables[i].id;
        return { ok: true };
      }
    }
    return { ok: false, reason: 'unknown' };
  }

  function simulatePastTtl() {
    var kept = state.expiresAt;
    state.forceExpired = true;
    note('演示时钟已越过注入的 TTL。到期时间数值没有被延长。');
    return { ok: true, expiresAt: kept, forceExpired: true };
  }

  function modeCards() {
    var cards = [];
    var i;
    for (i = 0; i < fixtures.MODE_ORDER.length; i += 1) {
      var id = fixtures.MODE_ORDER[i];
      var mode = fixtures.MODES[id];
      cards.push({
        id: id,
        label: mode.label,
        detail: mode.detail,
        selected: state.mode === id,
      });
    }
    return cards;
  }

  function snapshot() {
    var ring = selectedRing();
    var variable = selectedVariable();
    var receipt = null;
    if (state.receipt) {
      receipt = {
        identityId: state.receipt.identityId,
        requestId: state.receipt.requestId,
        bindingId: state.receipt.bindingId,
        claimantId: state.receipt.claimantId,
        auth: state.receipt.auth,
      };
    }
    return {
      banner: 'DEMO · 模拟数据 · 非生产 · 无网络',
      sourceLabel: 'DEMO/SIMULATED DATA',
      schemaId: fixtures.DEMO_SCHEMA.id,
      schemaVersion: fixtures.DEMO_SCHEMA.version,
      ttlMs: ttlMs,
      ttlLabel: String(ttlMs) + ' ms（演示注入，不是冻结协议）',
      encryption: 'not-implemented',
      e2ee: false,
      encryptionLabel: '未实现加密。不要把 TLS 当成端到端加密。本 DEMO 没有传输。',
      liveLink: false,
      networkCalls: state.networkCalls,
      usingMockFallback: state.usingMockFallback,
      fellBack: state.fellBack,
      keepalive: state.keepalive,
      mode: state.mode,
      modes: modeCards(),
      transport: state.transport,
      transportLabel: TRANSPORT_LABEL[state.transport],
      auth: state.auth,
      authLabel: AUTH_LABEL[state.auth],
      pc: state.pc,
      pcLabel: PC_LABEL[state.pc],
      target: state.target,
      targetLabel: TARGET_LABEL[state.target],
      freshness: state.freshness,
      freshnessLabel: FRESH_LABEL[state.freshness],
      generator: state.generator,
      generatorLabel: GENERATOR_LABEL[state.generator],
      role: state.role,
      roleLabel: state.role === 'viewer' ? '观察者（默认）' : '无',
      controlEnabled: false,
      controlReason: '默认观察者。控制通道单独存在且保持禁用。不会写变量，不会 RUN_CTL，不会烧录，不会打开 Shell。',
      claimedServer: state.claimed ? state.claimed.server : '—',
      claimedPc: state.claimed ? state.claimed.pc : '—',
      verifiedServer: state.verified ? state.verified.server : '—',
      verifiedPc: state.verified ? state.verified.pc : '—',
      redemptionCount: state.redemptionCount,
      grantCount: state.grantCount,
      expiresAt: state.expiresAt,
      receipt: receipt,
      canBrowse: canBrowse(),
      accessDeniedReason: canBrowse() ? '' : '没有观察者访问。拒绝、过期、撤销或尚未批准时不提供目录和读数。',
      projects: projectTree(),
      readings: readingRows(),
      ring: ring,
      bufferLength: ring.length,
      bufferLimit: bufferLimit,
      generation: state.generation,
      gapCount: state.gapCount,
      streamDesired: state.streamDesired,
      selectedBounds: variable ? { min: variable.min, max: variable.max, name: variable.name, unit: variable.unit } : { min: 0, max: 1, name: '—', unit: '' },
      devices: state.devices.map(function (device) {
        return {
          id: device.id,
          label: device.label,
          role: device.role,
          revoked: device.revoked,
          bindingId: device.bindingId,
          identityId: device.identityId,
          active: device.id === state.activeDeviceId,
        };
      }),
      command: {
        rawState: state.command.rawState,
        localConfidence: state.command.localConfidence,
        executed: state.command.executed,
        confirmed: state.command.confirmed,
        attemptCount: state.command.attemptCount,
        disconnectTreatedAsFailure: state.command.disconnectTreatedAsFailure,
        replayed: state.command.replayed,
        rejectedClass: state.command.rejectedClass,
      },
      commandCatalog: COMMAND_CATALOG,
      payment: copyPayment(),
      platformRestriction: state.platformRestriction,
      notice: state.notice,
      scanNotice: '模拟扫码：只接受手动输入或演示夹具。不会打开相机，也不会做真实扫码。',
      noNetworkNotice: '本构建没有任何网络通道。真实连接失败不会改播模拟数据。',
      hidden: state.hidden,
      logs: state.logs.slice(),
    };
  }

  return {
    snapshot: snapshot,
    submitShortCode: submitShortCode,
    submitFixtureKey: submitFixtureKey,
    verifyClaim: verifyClaim,
    requestPairing: requestPairing,
    approveAsSelf: approveAsSelf,
    applyPcDecision: applyPcDecision,
    restore: restore,
    rememberDevice: rememberDevice,
    revokeDevice: revokeDevice,
    selectDevice: selectDevice,
    setMode: setMode,
    attemptConnect: attemptConnect,
    acknowledgeIdle: acknowledgeIdle,
    disconnect: disconnect,
    startDemoStream: startDemoStream,
    stopDemoStream: stopDemoStream,
    tickSample: tickSample,
    markGap: markGap,
    onMonitorHide: onMonitorHide,
    onMonitorShow: onMonitorShow,
    demonstrateCommand: demonstrateCommand,
    submitExecutable: submitExecutable,
    replayCommand: replayCommand,
    setControl: setControl,
    notePaymentReturn: notePaymentReturn,
    noteServerPaid: noteServerPaid,
    setEntitlementFixture: setEntitlementFixture,
    selectProject: selectProject,
    selectModule: selectModule,
    selectVariable: selectVariable,
    simulatePastTtl: simulatePastTtl,
    getNetworkCallCount: function () { return state.networkCalls; },
  };
}

var singleton = null;

function getStore() {
  if (!singleton) singleton = createDemoStore();
  return singleton;
}

module.exports = {
  createDemoStore: createDemoStore,
  getStore: getStore,
  parseDemoCode: qr.parseDemoCode,
  buildDrawSegments: waveform.buildDrawSegments,
  DEMO_SCHEMA: fixtures.DEMO_SCHEMA,
  COMMAND_STATES: COMMAND_STATES,
  BUFFER_LIMIT: BUFFER_LIMIT,
};
