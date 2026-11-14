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
  onReturn: function (event) {
    engine.getStore().notePaymentReturn(event.currentTarget.dataset.value);
    this.refresh();
  },
  onPaid: function (event) {
    engine.getStore().noteServerPaid(event.currentTarget.dataset.value);
    this.refresh();
  },
  onEntitlement: function (event) {
    engine.getStore().setEntitlementFixture(event.currentTarget.dataset.value);
    this.refresh();
  },
});
