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
  onShowState: function (event) {
    engine.getStore().demonstrateCommand(event.currentTarget.dataset.state);
    this.refresh();
  },
  onExec: function (event) {
    engine.getStore().submitExecutable(event.currentTarget.dataset.name);
    this.refresh();
  },
  onReplay: function () {
    engine.getStore().replayCommand();
    this.refresh();
  },
  onDisconnect: function () {
    engine.getStore().disconnect();
    this.refresh();
  },
  onControl: function () {
    engine.getStore().setControl(true);
    this.refresh();
  },
});
