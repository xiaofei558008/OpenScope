'use strict';

var test = require('node:test');
var assert = require('node:assert/strict');
var engine = require('../miniprogram/domain/engine.js');

function pairViewer(store) {
  var submitted = store.submitShortCode('OSC-DEMO01');
  assert.equal(submitted.ok, true);
  assert.equal(submitted.grantsControl, false);
  assert.equal(store.verifyClaim().ok, true);
  assert.equal(store.snapshot().role, 'none');
  assert.equal(store.snapshot().controlEnabled, false);
  var pending = store.requestPairing();
  assert.equal(pending.ok, true);
  assert.equal(store.snapshot().auth, 'pending');
  var approved = store.applyPcDecision('approved');
  assert.equal(approved.ok, true);
  assert.equal(store.snapshot().role, 'viewer');
  assert.equal(store.snapshot().controlEnabled, false);
  return store;
}

test('pairing stays pending, can be denied, and otherwise defaults to viewer', function () {
  var pendingStore = engine.createDemoStore({ now: function () { return 1000; }, ttlMs: 5000 });
  assert.equal(pendingStore.submitFixtureKey('demo').ok, true);
  assert.equal(pendingStore.snapshot().auth, 'claimed_unverified');
  assert.equal(pendingStore.snapshot().claimedServer, '声称的局域网 PC');
  assert.equal(pendingStore.snapshot().verifiedServer, '—');
  assert.equal(pendingStore.verifyClaim().ok, true);
  assert.equal(pendingStore.snapshot().auth, 'verified_preview');
  assert.equal(pendingStore.snapshot().verifiedPc, 'PC-LAB-1');
  assert.notEqual(pendingStore.snapshot().claimedPc, pendingStore.snapshot().verifiedPc);
  assert.equal(pendingStore.snapshot().role, 'none');
  assert.equal(pendingStore.requestPairing().ok, true);
  assert.equal(pendingStore.snapshot().auth, 'pending');
  assert.equal(pendingStore.snapshot().role, 'none');
  assert.equal(pendingStore.snapshot().controlEnabled, false);

  var denied = engine.createDemoStore({ now: function () { return 1000; }, ttlMs: 5000 });
  denied.submitFixtureKey('deny');
  denied.verifyClaim();
  denied.requestPairing();
  var decision = denied.applyPcDecision('denied');
  assert.equal(decision.ok, true);
  assert.equal(denied.snapshot().auth, 'denied');
  assert.equal(denied.snapshot().role, 'none');
  assert.equal(denied.snapshot().grantCount, 0);
  assert.equal(denied.snapshot().canBrowse, false);

  var viewer = pairViewer(engine.createDemoStore({ now: function () { return 1000; }, ttlMs: 5000 }));
  assert.equal(viewer.snapshot().auth, 'viewer');
  assert.equal(viewer.snapshot().role, 'viewer');
  assert.equal(viewer.snapshot().roleLabel, '观察者（默认）');
  assert.equal(viewer.snapshot().controlEnabled, false);
  assert.equal(viewer.setControl().ok, false);
  assert.equal(viewer.snapshot().controlEnabled, false);
  assert.equal(viewer.snapshot().grantCount, 1);
});

test('the phone cannot approve its own pairing request', function () {
  var store = engine.createDemoStore({ now: function () { return 1000; }, ttlMs: 5000 });
  store.submitShortCode('OSC-DEMO01');
  store.verifyClaim();
  store.requestPairing();
  var self = store.approveAsSelf();
  assert.equal(self.ok, false);
  assert.equal(self.reason, 'self_approval_forbidden');
  assert.equal(self.executed, false);
  assert.equal(store.snapshot().auth, 'pending');
  assert.equal(store.snapshot().role, 'none');
  assert.equal(store.snapshot().grantCount, 0);
  assert.equal(store.snapshot().controlEnabled, false);
});

test('hide clears the stale generation and show starts a fresh simulated session', function () {
  var store = pairViewer(engine.createDemoStore());
  assert.equal(store.startDemoStream().ok, true);
  assert.equal(store.tickSample().ok, true);
  assert.equal(store.tickSample().ok, true);
  var before = store.snapshot();
  var oldGeneration = before.ring[0].generation;
  assert.equal(before.freshness, 'fresh');
  var hidden = store.onMonitorHide();
  var paused = store.snapshot();
  assert.equal(hidden.keepalive, false);
  assert.equal(paused.keepalive, false);
  assert.equal(paused.generator, 'paused');
  assert.equal(paused.ring.length, 0);
  assert.equal(paused.freshness, 'stale_cleared');
  assert.ok(paused.generation > before.generation);
  assert.equal(store.tickSample().ok, false);

  store.onMonitorShow();
  var shown = store.snapshot();
  assert.equal(shown.ring.length, 0);
  assert.equal(shown.keepalive, false);
  assert.equal(shown.generator, 'running');
  assert.equal(shown.freshness, 'stale_cleared');
  assert.equal(shown.liveLink, false);
  assert.ok(shown.generation > paused.generation);
  assert.equal(store.tickSample().ok, true);
  var fresh = store.snapshot();
  assert.ok(fresh.ring.length > 0);
  assert.equal(fresh.ring.every(function (point) { return point.generation !== oldGeneration; }), true);
  assert.equal(fresh.freshness, 'fresh');
});

test('a gap is not rendered as zero or as a connected line', function () {
  var store = pairViewer(engine.createDemoStore());
  store.startDemoStream();
  store.tickSample();
  store.tickSample();
  var gap = store.markGap();
  assert.equal(gap.ok, true);
  var duringGap = store.snapshot();
  assert.equal(duringGap.freshness, 'gap');
  assert.equal(duringGap.readings.every(function (row) { return row.display === '—'; }), true);
  assert.equal(JSON.stringify(duringGap.readings).indexOf('"display":"0"'), -1);
  store.tickSample();
  var ring = store.snapshot().ring;
  assert.equal(ring.some(function (entry) { return entry.kind === 'gap'; }), true);
  assert.equal(ring.some(function (entry) { return entry.kind === 'gap' && Object.prototype.hasOwnProperty.call(entry, 'value'); }), false);
  var bounds = store.snapshot().selectedBounds;
  var segments = engine.buildDrawSegments(ring, 100, 50, bounds.min, bounds.max);
  assert.equal(segments.length, 2);
  var firstValues = segments[0].points.map(function (point) { return point.value; });
  var secondValues = segments[1].points.map(function (point) { return point.value; });
  assert.equal(firstValues.indexOf(secondValues[0]), -1);
  var flat = firstValues.concat(secondValues);
  assert.equal(flat.indexOf(0), -1);
  assert.equal(JSON.stringify(segments).indexOf('"value":0'), -1);
  assert.deepEqual(flat, ring.filter(function (entry) { return entry.kind === 'point'; }).map(function (entry) { return entry.value; }));
});

test('sample rings stay inside the bounded limit', function () {
  var store = pairViewer(engine.createDemoStore({ bufferLimit: 5 }));
  store.startDemoStream();
  var i;
  for (i = 0; i < 12; i += 1) store.tickSample();
  store.markGap();
  store.tickSample();
  var snap = store.snapshot();
  assert.equal(snap.bufferLimit, 5);
  assert.ok(snap.bufferLength <= 5);
  assert.equal(snap.ring.length <= 5, true);
  assert.equal(snap.ring[0].kind === 'point' || snap.ring[0].kind === 'gap', true);
  var oldestPoint = null;
  var n;
  for (n = 0; n < snap.ring.length; n += 1) {
    if (snap.ring[n].kind === 'point') {
      oldestPoint = snap.ring[n];
      break;
    }
  }
  assert.ok(oldestPoint.t > 1);
  assert.equal(snap.ring.some(function (entry) { return entry.kind === 'point' && entry.t === 1; }), false);
});

test('payment return, server paid, and entitlement are independent', function () {
  var store = engine.createDemoStore();
  var returned = store.notePaymentReturn('returned');
  assert.equal(returned.paymentReturn, 'returned');
  assert.equal(returned.serverPaid, 'unknown');
  assert.equal(returned.entitlement, 'inactive');
  var paid = store.noteServerPaid('paid');
  assert.equal(paid.paymentReturn, 'returned');
  assert.equal(paid.serverPaid, 'paid');
  assert.equal(paid.entitlement, 'inactive');
  var entitled = store.setEntitlementFixture('active');
  assert.equal(entitled.paymentReturn, 'returned');
  assert.equal(entitled.serverPaid, 'paid');
  assert.equal(entitled.entitlement, 'active');
  var snap = store.snapshot();
  assert.equal(Object.prototype.hasOwnProperty.call(snap.payment, 'price'), false);
  assert.equal(Object.prototype.hasOwnProperty.call(snap.payment, 'orderId'), false);
  assert.equal(snap.transport, 'not_connected');
  assert.equal(snap.liveLink, false);
  assert.equal(store.attemptConnect({}).ok, false);
  assert.equal(store.snapshot().mode, 'lan_direct');
  assert.equal(store.snapshot().transport, 'failed');
  assert.equal(store.snapshot().entitlement ? store.snapshot().payment.entitlement : snap.payment.entitlement, 'active');
  assert.equal(store.snapshot().payment.entitlement, 'active');
  assert.equal(store.snapshot().usingMockFallback, false);
});

test('a failed mode does not fall back or switch from LAN to relay', function () {
  var store = pairViewer(engine.createDemoStore());
  store.startDemoStream();
  store.tickSample();
  assert.equal(store.setMode('lan_direct').ok, true);
  var failed = store.attemptConnect({ host: 'https://evil.invalid/pair' });
  var snap = store.snapshot();
  assert.equal(failed.ok, false);
  assert.equal(failed.reason, 'no_network_path');
  assert.equal(failed.fellBack, false);
  assert.equal(failed.network, false);
  assert.equal(snap.mode, 'lan_direct');
  assert.equal(snap.transport, 'failed');
  assert.equal(snap.generator, 'stopped');
  assert.equal(snap.ring.length, 0);
  assert.equal(snap.usingMockFallback, false);
  assert.equal(snap.fellBack, false);
  assert.equal(snap.liveLink, false);
  assert.equal(snap.networkCalls, 0);
  assert.equal(JSON.stringify(snap).indexOf('evil.invalid'), -1);
  assert.equal(store.getNetworkCallCount(), 0);
  assert.equal(typeof store.autoFallback, 'undefined');
  var refused = store.startDemoStream();
  assert.equal(refused.ok, false);
  assert.equal(refused.reason, 'refusing_mock_fallback');
  assert.equal(store.snapshot().mode, 'lan_direct');
  assert.equal(store.snapshot().generator, 'stopped');

  var relay = pairViewer(engine.createDemoStore());
  relay.setMode('managed_relay');
  var relayFail = relay.attemptConnect({});
  assert.equal(relayFail.reason, 'no_network_path');
  assert.equal(relay.snapshot().mode, 'managed_relay');
  assert.equal(relay.startDemoStream().reason, 'mode_blocks_generator');
  assert.equal(relay.snapshot().mode, 'managed_relay');
  assert.equal(relay.snapshot().ring.length, 0);
});

test('unknown command is not replayed and disconnect is not a confirmed failure', function () {
  var store = engine.createDemoStore();
  var shown = store.demonstrateCommand('unknown');
  assert.equal(shown.rawState, 'unknown');
  assert.equal(shown.localConfidence, 'displayed_only');
  assert.equal(shown.executed, false);
  assert.equal(shown.confirmed, false);
  var replay = store.replayCommand();
  assert.equal(replay.ok, false);
  assert.equal(replay.reason, 'not_replayed');
  assert.equal(replay.replayed, false);
  assert.equal(replay.rawState, 'unknown');
  assert.equal(replay.attemptCount, 0);
  var disconnected = store.disconnect();
  assert.equal(disconnected.rawState, 'unknown');
  assert.equal(disconnected.treatedAsFailure, false);
  assert.equal(disconnected.replayed, false);
  var snap = store.snapshot();
  assert.equal(snap.command.rawState, 'unknown');
  assert.equal(snap.command.disconnectTreatedAsFailure, false);
  assert.equal(snap.transport, 'disconnected');
  assert.notEqual(snap.command.rawState, 'failed');

  store.demonstrateCommand('succeeded');
  store.disconnect();
  assert.equal(store.snapshot().command.rawState, 'succeeded');
  assert.equal(store.snapshot().command.localConfidence, 'displayed_only');
  assert.equal(store.replayCommand().rawState, 'succeeded');
});

test('malicious demo QR text never becomes a request and is not logged raw', function () {
  var samples = [
    'https://evil.invalid/pair',
    'http://203.0.113.9/socket',
    'wss://relay.invalid/ws',
    'weixin://dl/business/?t=abc',
    'javascript:alert(1)',
    'OSC-DEMO01/../admin',
    'OSC-DEMO01?token=abcdef',
    '/pages/pair/pair',
    'C:\\Windows\\secret',
    'Bearer abcdefghijklmnop',
    'osc-demo01',
    'OSC-NOTHEX',
    'OSC-UNKNOWN',
  ];
  var store = engine.createDemoStore();
  var i;
  for (i = 0; i < samples.length; i += 1) {
    var parsed = engine.parseDemoCode(samples[i]);
    assert.equal(parsed.ok, false);
    assert.equal(parsed.network, false);
    assert.equal(parsed.followedUrl, false);
    assert.equal(parsed.rawLogged, false);
    assert.equal(parsed.grantsControl, false);
    assert.equal(parsed.longTermSecret, false);
    assert.equal(JSON.stringify(parsed).indexOf(samples[i]), -1);
    var submitted = store.submitShortCode(samples[i]);
    assert.equal(submitted.ok, false);
    assert.equal(submitted.network, false);
    assert.equal(JSON.stringify(store.snapshot()).indexOf(samples[i]), -1);
  }
  assert.equal(store.getNetworkCallCount(), 0);
  assert.equal(store.snapshot().logs.length, 0);
  assert.equal(store.snapshot().auth, 'anonymous');
  assert.deepEqual(engine.parseDemoCode(''), { ok: false, reason: 'empty', network: false, followedUrl: false, rawLogged: false, grantsControl: false, longTermSecret: false });
});

test('revoking a remembered device removes access', function () {
  var store = pairViewer(engine.createDemoStore());
  var remembered = store.rememberDevice();
  assert.equal(remembered.ok, true);
  store.startDemoStream();
  store.tickSample();
  assert.equal(store.snapshot().canBrowse, true);
  assert.ok(store.snapshot().projects.length > 0);
  var revoked = store.revokeDevice(remembered.id);
  assert.equal(revoked.access, false);
  var snap = store.snapshot();
  assert.equal(snap.auth, 'revoked');
  assert.equal(snap.role, 'none');
  assert.equal(snap.canBrowse, false);
  assert.equal(snap.projects.length, 0);
  assert.equal(snap.readings.length, 0);
  assert.equal(snap.ring.length, 0);
  assert.equal(snap.controlEnabled, false);
  assert.equal(store.selectDevice(remembered.id).access, false);
  assert.equal(store.snapshot().canBrowse, false);
  assert.equal(store.startDemoStream().ok, false);
});

test('the same identity, request, and binding restores pending or denied without a second redemption', function () {
  var clock = { t: 1000 };
  var store = engine.createDemoStore({
    now: function () { return clock.t; },
    ttlMs: 5000,
  });
  store.submitShortCode('OSC-DEMO01');
  store.verifyClaim();
  var first = store.requestPairing();
  assert.equal(first.redemptionCount, 1);
  assert.equal(first.grantCount, 0);
  var expiresAt = first.expiresAt;
  var receipt = store.snapshot().receipt;
  clock.t = 2000;
  var restored = store.restore(receipt);
  assert.equal(restored.ok, true);
  assert.equal(restored.auth, 'pending');
  assert.equal(restored.redemptionCount, 1);
  assert.equal(restored.grantCount, 0);
  assert.equal(restored.expiresAt, expiresAt);
  assert.equal(store.requestPairing().reason, 'already_pending');
  assert.equal(store.snapshot().redemptionCount, 1);
  assert.equal(store.snapshot().expiresAt, expiresAt);

  store.applyPcDecision('denied');
  var deniedAgain = store.restore(store.snapshot().receipt);
  assert.equal(deniedAgain.ok, true);
  assert.equal(deniedAgain.auth, 'denied');
  assert.equal(deniedAgain.grantCount, 0);
  assert.equal(deniedAgain.redemptionCount, 1);
  assert.equal(deniedAgain.expiresAt, expiresAt);
  assert.equal(store.snapshot().role, 'none');
  assert.equal(store.requestPairing().reason, 'use_restore_not_rerequest');
  assert.equal(store.snapshot().redemptionCount, 1);

  clock.t = expiresAt + 1;
  var expired = store.restore({
    identityId: receipt.identityId,
    requestId: receipt.requestId,
    bindingId: receipt.bindingId,
    claimantId: receipt.claimantId,
  });
  assert.equal(expired.ok, false);
  assert.equal(expired.reason, 'expired');
  assert.equal(expired.expiresAt, expiresAt);
  assert.equal(store.snapshot().grantCount, 0);
  assert.equal(store.snapshot().auth, 'expired');
});

test('a different claimant, an expired code, or a revoked code is denied', function () {
  var clock = { t: 1000 };
  var pending = engine.createDemoStore({ now: function () { return clock.t; }, ttlMs: 5000 });
  pending.submitShortCode('OSC-DEMO01');
  pending.verifyClaim();
  pending.requestPairing();
  var before = pending.snapshot();
  var hostile = pending.restore({
    identityId: before.receipt.identityId,
    requestId: before.receipt.requestId,
    bindingId: before.receipt.bindingId,
    claimantId: 'phone-other',
  });
  assert.equal(hostile.ok, false);
  assert.equal(hostile.reason, 'different_claimant');
  assert.equal(pending.snapshot().auth, 'pending');
  assert.equal(pending.snapshot().redemptionCount, 1);
  assert.equal(pending.snapshot().grantCount, 0);
  assert.equal(pending.snapshot().expiresAt, before.expiresAt);

  var other = engine.createDemoStore();
  assert.equal(other.submitFixtureKey('other').ok, true);
  assert.equal(other.verifyClaim().reason, 'different_claimant');
  assert.equal(other.snapshot().auth, 'denied');
  assert.equal(other.requestPairing().ok, false);
  assert.equal(other.snapshot().canBrowse, false);

  var expired = engine.createDemoStore();
  expired.submitFixtureKey('expired');
  assert.equal(expired.verifyClaim().reason, 'expired');
  assert.equal(expired.snapshot().auth, 'expired');
  assert.equal(expired.snapshot().grantCount, 0);

  var revoked = engine.createDemoStore();
  revoked.submitFixtureKey('revoked');
  assert.equal(revoked.verifyClaim().reason, 'revoked');
  assert.equal(revoked.snapshot().auth, 'revoked');
  assert.equal(revoked.snapshot().canBrowse, false);
  assert.equal(revoked.snapshot().projects.length, 0);
});

test('self-host bare public IP is a platform restriction and not a connection', function () {
  var store = pairViewer(engine.createDemoStore());
  store.setMode('selfhost_relay');
  var result = store.attemptConnect({ host: '203.0.113.10' });
  var snap = store.snapshot();
  assert.equal(result.ok, false);
  assert.equal(result.reason, 'wechat_platform_restriction');
  assert.equal(result.pretendConnected, false);
  assert.equal(result.fellBack, false);
  assert.equal(result.network, false);
  assert.equal(snap.transport, 'restricted');
  assert.equal(snap.liveLink, false);
  assert.equal(snap.mode, 'selfhost_relay');
  assert.equal(snap.generator, 'stopped');
  assert.equal(snap.ring.length, 0);
  assert.ok(snap.platformRestriction.length > 0);
  assert.equal(JSON.stringify(snap).indexOf('203.0.113.10'), -1);
  assert.equal(store.getNetworkCallCount(), 0);
  assert.equal(store.startDemoStream().reason, 'wechat_platform_restriction');
  assert.equal(snap.e2ee, false);
  assert.equal(store.snapshot().encryption, 'not-implemented');
  assert.equal(store.snapshot().schemaVersion, 'demo-injected-not-wire');
});

test('executable controls are rejected and never run', function () {
  var store = pairViewer(engine.createDemoStore());
  var names = ['RUN_CTL', 'flash', 'shell', 'write', 'arbitrary'];
  var i;
  for (i = 0; i < names.length; i += 1) {
    var result = store.submitExecutable(names[i]);
    assert.equal(result.executed, false);
    assert.equal(result.rawState, 'rejected');
    assert.equal(result.network, false);
  }
  var payload = 'rm -rf /tmp/openscope-demo-payload';
  var blocked = store.submitExecutable(payload);
  assert.equal(blocked.executed, false);
  assert.equal(blocked.rejectedClass, 'arbitrary');
  assert.equal(JSON.stringify(store.snapshot()).indexOf(payload), -1);
  assert.equal(store.replayCommand().executed, false);
  assert.equal(store.snapshot().command.rawState, 'rejected');
  assert.equal(store.setControl().executed, false);
});

test('missing readings render as a dash and demo schema stays injected', function () {
  var store = pairViewer(engine.createDemoStore());
  var before = store.snapshot();
  assert.equal(before.readings.length > 0, true);
  assert.equal(before.readings.every(function (row) { return row.display === '—'; }), true);
  assert.equal(before.sourceLabel, 'DEMO/SIMULATED DATA');
  assert.equal(before.schemaVersion, 'demo-injected-not-wire');
  assert.equal(before.encryption, 'not-implemented');
  assert.equal(before.e2ee, false);
  assert.equal(before.liveLink, false);
  store.acknowledgeIdle();
  assert.equal(store.snapshot().generator, 'stopped');
  assert.equal(store.snapshot().ring.length, 0);
  assert.equal(store.startDemoStream().ok, true);
  assert.equal(store.snapshot().transport, 'not_connected');
  assert.equal(store.tickSample().ok, true);
  var live = store.snapshot();
  assert.equal(live.transport, 'not_connected');
  assert.equal(live.liveLink, false);
  assert.equal(live.readings.every(function (row) { return row.display !== '—' && row.display !== '0'; }), true);
});
