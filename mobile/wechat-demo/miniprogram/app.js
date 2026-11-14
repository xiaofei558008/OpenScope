'use strict';

var engine = require('./domain/engine.js');

App({
  onLaunch: function () {
    this.store = engine.getStore();
    this._foregroundPasses = 0;
  },
  onShow: function () {
    this._foregroundPasses += 1;
    if (this._foregroundPasses > 1) {
      // Drop anything sampled before the app was hidden. The monitor page
      // starts a fresh local session only when that page itself is shown.
      engine.getStore().onMonitorHide();
    }
  },
  onHide: function () {
    engine.getStore().onMonitorHide();
  },
});
