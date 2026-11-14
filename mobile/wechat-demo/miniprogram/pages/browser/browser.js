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
  onProject: function (event) {
    engine.getStore().selectProject(event.currentTarget.dataset.id);
    this.refresh();
  },
  onModule: function (event) {
    engine.getStore().selectModule(event.currentTarget.dataset.id);
    this.refresh();
  },
  onVariable: function (event) {
    engine.getStore().selectVariable(event.currentTarget.dataset.id);
    this.refresh();
  },
});
