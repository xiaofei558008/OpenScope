'use strict';

var engine = require('../../domain/engine.js');

Page({
  data: { snap: engine.getStore().snapshot() },
  onShow: function () {
    this.refresh();
  },
  refresh: function () {
    this.setData({ snap: engine.getStore().snapshot() });
  },
  onRemember: function () {
    engine.getStore().rememberDevice();
    this.refresh();
  },
  onSelect: function (event) {
    engine.getStore().selectDevice(event.currentTarget.dataset.id);
    this.refresh();
  },
  onRevoke: function (event) {
    engine.getStore().revokeDevice(event.currentTarget.dataset.id);
    this.refresh();
  },
});
