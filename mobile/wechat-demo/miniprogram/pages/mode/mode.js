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
  onPickMode: function (event) {
    engine.getStore().setMode(event.currentTarget.dataset.mode);
    this.refresh();
  },
  onConnect: function () {
    var host = this.data.draft || '';
    engine.getStore().attemptConnect({ host: host });
    this.setData({ draft: '' });
    this.refresh();
  },
  onIdle: function () {
    engine.getStore().acknowledgeIdle();
    this.refresh();
  },
});
