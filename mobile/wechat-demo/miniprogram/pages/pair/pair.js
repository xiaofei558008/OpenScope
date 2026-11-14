'use strict';

var engine = require('../../domain/engine.js');

Page({
  data: {
    snap: engine.getStore().snapshot(),
    draft: '',
  },
  onShow: function () {
    this.refresh();
  },
  refresh: function () {
    this.setData({ snap: engine.getStore().snapshot() });
  },
  onDraft: function (event) {
    this.setData({ draft: event.detail.value });
  },
  onSubmitCode: function () {
    engine.getStore().submitShortCode(this.data.draft || '');
    this.setData({ draft: '' });
    this.refresh();
  },
  onFixture: function (event) {
    engine.getStore().submitFixtureKey(event.currentTarget.dataset.key);
    this.setData({ draft: '' });
    this.refresh();
  },
  onVerify: function () {
    engine.getStore().verifyClaim();
    this.refresh();
  },
  onRequest: function () {
    engine.getStore().requestPairing();
    this.refresh();
  },
  onSelf: function () {
    engine.getStore().approveAsSelf();
    this.refresh();
  },
  onPc: function (event) {
    engine.getStore().applyPcDecision(event.currentTarget.dataset.decision);
    this.refresh();
  },
  onRestore: function () {
    var receipt = engine.getStore().snapshot().receipt || {};
    engine.getStore().restore(receipt);
    this.refresh();
  },
  onExpire: function () {
    engine.getStore().simulatePastTtl();
    engine.getStore().restore(engine.getStore().snapshot().receipt || {});
    this.refresh();
  },
});
