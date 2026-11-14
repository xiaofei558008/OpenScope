'use strict';

var engine = require('../../domain/engine.js');
var waveform = require('../../domain/waveform.js');

Page({
  data: { snap: engine.getStore().snapshot() },
  onReady: function () {
    this.initCanvas();
  },
  onShow: function () {
    engine.getStore().onMonitorShow();
    this.refresh();
    this.syncTimer();
    this.initCanvas();
  },
  onHide: function () {
    this.stopTimer();
    engine.getStore().onMonitorHide();
    this.refresh();
  },
  onUnload: function () {
    this.stopTimer();
    engine.getStore().onMonitorHide();
  },
  refresh: function () {
    this.setData({ snap: engine.getStore().snapshot() });
  },
  stopTimer: function () {
    if (this._timer) {
      clearInterval(this._timer);
      this._timer = null;
    }
  },
  syncTimer: function () {
    var snap = engine.getStore().snapshot();
    if (snap.generator === 'running' && !this._timer) {
      var self = this;
      this._timer = setInterval(function () {
        engine.getStore().tickSample();
        self.refresh();
        self.draw();
      }, 320);
      return;
    }
    if (snap.generator !== 'running') this.stopTimer();
  },
  initCanvas: function () {
    var query = wx.createSelectorQuery();
    var self = this;
    query.select('#wave').fields({ node: true, size: true }).exec(function (res) {
      if (!res || !res[0] || !res[0].node) return;
      var canvas = res[0].node;
      var ctx = canvas.getContext('2d');
      var info = wx.getWindowInfo ? wx.getWindowInfo() : wx.getSystemInfoSync();
      var dpr = info.pixelRatio || 2;
      self._w = res[0].width;
      self._h = res[0].height;
      canvas.width = self._w * dpr;
      canvas.height = self._h * dpr;
      ctx.scale(dpr, dpr);
      self._ctx = ctx;
      self.draw();
    });
  },
  draw: function () {
    var ctx = this._ctx;
    if (!ctx || !this._w) return;
    var snap = engine.getStore().snapshot();
    var ring = snap.ring || [];
    var bounds = snap.selectedBounds || { min: 0, max: 1 };
    var segments = waveform.buildDrawSegments(ring, this._w, this._h, bounds.min, bounds.max);
    ctx.clearRect(0, 0, this._w, this._h);
    ctx.fillStyle = '#0c1218';
    ctx.fillRect(0, 0, this._w, this._h);
    ctx.strokeStyle = '#243140';
    ctx.lineWidth = 1;
    ctx.beginPath();
    ctx.moveTo(0, this._h / 2);
    ctx.lineTo(this._w, this._h / 2);
    ctx.stroke();
    ctx.strokeStyle = '#8fd0a8';
    ctx.fillStyle = '#8fd0a8';
    ctx.lineWidth = 2;
    var s;
    for (s = 0; s < segments.length; s += 1) {
      var points = segments[s].points;
      if (!points.length) continue;
      if (points.length === 1) {
        ctx.beginPath();
        ctx.arc(points[0].x, points[0].y, 2.5, 0, Math.PI * 2);
        ctx.fill();
        continue;
      }
      ctx.beginPath();
      ctx.moveTo(points[0].x, points[0].y);
      var i;
      for (i = 1; i < points.length; i += 1) ctx.lineTo(points[i].x, points[i].y);
      ctx.stroke();
    }
  },
  onStart: function () {
    engine.getStore().startDemoStream();
    this.refresh();
    this.syncTimer();
    this.draw();
  },
  onStop: function () {
    this.stopTimer();
    engine.getStore().stopDemoStream();
    this.refresh();
    this.draw();
  },
  onGap: function () {
    engine.getStore().markGap();
    this.refresh();
    this.draw();
  },
});
