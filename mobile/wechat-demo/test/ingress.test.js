'use strict';

var test = require('node:test');
var assert = require('node:assert/strict');
var engine = require('../miniprogram/domain/engine.js');

function ready(limit) {
  var store = engine.createDemoStore(limit ? { bufferLimit: limit } : undefined);
  assert.equal(store.submitShortCode('OSC-DEMO01').ok, true);
  assert.equal(store.verifyClaim().ok, true);
  assert.equal(store.snapshot().auth, 'verified_preview');
  assert.equal(store.snapshot().role, 'none');
  assert.equal(store.requestPairing().ok, true);
  assert.equal(store.approveAsSelf().ok, false);
  assert.equal(store.applyPcDecision('approved').role, 'viewer');
  assert.equal(store.snapshot().controlEnabled, false);
  assert.equal(store.startDemoStream().ok, true);
  assert.equal(store.snapshot().transport, 'not_connected');
  assert.equal(store.snapshot().liveLink, false);
  return store;
}

test('cancel clears a pending demo request without a grant or a PC session', function () {
  var idle = engine.createDemoStore();
  var none = idle.cancelPairing();
  assert.equal(none.ok, false);
  assert.equal(none.reason, 'nothing_to_cancel');
  assert.equal(none.network, false);
  assert.equal(idle.snapshot().actions.canCancel, false);
  assert.ok(idle.snapshot().actions.cancelBlock.length > 0);

  var store = engine.createDemoStore({ now: function () { return 1000; }, ttlMs: 5000 });
  store.submitShortCode('OSC-DEMO01');
  store.verifyClaim();
  var pending = store.requestPairing();
  assert.equal(pending.redemptionCount, 1);
  assert.equal(pending.grantCount, 0);
  assert.equal(store.snapshot().actions.canCancel, true);
  var cancelled = store.cancelPairing();
  assert.equal(cancelled.ok, true);
  assert.equal(cancelled.reason, 'cancelled_local');
  assert.equal(cancelled.network, false);
  assert.equal(cancelled.auth, 'anonymous');
  assert.equal(cancelled.role, 'none');
  assert.equal(cancelled.controlEnabled, false);
  assert.equal(cancelled.redemptionCount, 1);
  assert.equal(cancelled.grantCount, 0);
  var snap = store.snapshot();
  assert.equal(snap.auth, 'anonymous');
  assert.equal(snap.receipt, null);
  assert.equal(snap.liveLink, false);
  assert.equal(store.approveAsSelf().ok, false);
  assert.equal(store.snapshot().redemptionCount, 1);
  assert.equal(store.snapshot().grantCount, 0);
  assert.equal(store.requestPairing().reason, 'not_verified');
  assert.equal(store.snapshot().redemptionCount, 1);

  var viewer = ready();
  var kept = viewer.cancelPairing();
  assert.equal(kept.ok, false);
  assert.equal(kept.reason, 'viewer_not_a_pc_session');
  assert.equal(viewer.snapshot().role, 'viewer');
  assert.equal(viewer.snapshot().grantCount, 1);
  assert.equal(viewer.snapshot().controlEnabled, false);
  assert.ok(viewer.snapshot().actions.cancelBlock.indexOf('不是在 PC 上结束会话') >= 0);
});

test('ingress rejects stale and non-finite samples without using a slot or plotting zero', function () {
  var store = ready(3);
  function point(generation, source, value, t) {
    return store.acceptSample({
      variableId: 'var-speed',
      value: value,
      generation: generation,
      source: source,
      t: t,
    });
  }
  assert.equal(point(1, 'demo-local', 810, 1).reason, 'established');
  assert.equal(point(1, 'demo-local', 820, 2).stored, true);
  assert.equal(point(1, 'demo-local', 830, 3).stored, true);
  var full = store.snapshot();
  assert.equal(full.bufferLength, 3);
  assert.equal(full.ring[0].t, 1);
  var nan = point(1, 'demo-local', NaN, 9);
  var inf = point(1, 'demo-local', Infinity, 10);
  assert.equal(nan.stored, false);
  assert.equal(nan.reason, 'non_finite');
  assert.equal(inf.stored, false);
  assert.equal(inf.reason, 'non_finite');
  var afterReject = store.snapshot();
  assert.equal(afterReject.bufferLength, 3);
  assert.equal(afterReject.ring[0].t, 1);
  assert.equal(JSON.stringify(afterReject.ring).indexOf('NaN'), -1);
  assert.equal(JSON.stringify(afterReject.readings).indexOf('"display":"0"'), -1);
  assert.equal(afterReject.ring.some(function (entry) { return entry.value === 0; }), false);

  var staleEpoch = point(0, 'demo-local', 840, 11);
  var staleSource = point(1, 'other-demo', 840, 12);
  assert.equal(staleEpoch.reason, 'stale_epoch');
  assert.equal(staleSource.reason, 'stale_source');
  assert.equal(staleEpoch.stored, false);
  assert.equal(staleSource.stored, false);
  assert.equal(store.snapshot().bufferLength, 3);
  assert.equal(store.snapshot().ring[0].t, 1);

  assert.equal(point(1, 'demo-local', 850, 4).stored, true);
  var dropped = store.snapshot();
  assert.equal(dropped.bufferLength, 3);
  assert.equal(dropped.ring[0].t, 2);
  assert.equal(dropped.ring.some(function (entry) { return entry.t === 1; }), false);
  assert.equal(dropped.liveLink, false);
  assert.equal(dropped.transport, 'not_connected');
  assert.equal(Object.prototype.hasOwnProperty.call(dropped.ring[0], 'source'), false);

  var switched = point(2, 'demo-local', 860, 20);
  assert.equal(switched.reason, 'epoch_switch');
  var ring = store.snapshot().ring;
  assert.equal(ring[0].kind, 'gap');
  assert.equal(ring[1].kind, 'point');
  assert.equal(ring[1].value, 860);
  assert.equal(ring[1].generation, 2);
  assert.equal(ring.some(function (entry) { return entry.value === 810 || entry.value === 820; }), false);
  var segments = engine.buildDrawSegments(ring, 100, 40, 800, 2400);
  assert.equal(segments.length, 1);
  assert.equal(segments[0].points[0].value, 860);

  var poisoned = [
    { kind: 'point', t: 1, value: 10 },
    { kind: 'point', t: 2, value: NaN },
    { kind: 'point', t: 3, value: Infinity },
    { kind: 'point', t: 4, value: 12 },
  ];
  var drawn = engine.buildDrawSegments(poisoned, 100, 40, 0, 20);
  assert.equal(drawn.length, 2);
  assert.deepEqual(drawn[0].points.map(function (item) { return item.value; }), [10]);
  assert.deepEqual(drawn[1].points.map(function (item) { return item.value; }), [12]);
  assert.equal(JSON.stringify(drawn).indexOf('"value":0'), -1);

  store.setMode('selfhost_relay');
  var blocked = point(3, 'demo-local', 870, 21);
  assert.equal(blocked.stored, false);
  assert.equal(blocked.reason, 'not_running');
  assert.equal(store.snapshot().mode, 'selfhost_relay');
  assert.equal(store.snapshot().liveLink, false);
  assert.equal(store.getNetworkCallCount(), 0);
});
