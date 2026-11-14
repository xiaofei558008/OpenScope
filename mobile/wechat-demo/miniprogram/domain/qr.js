'use strict';

var fixtures = require('./fixtures.js');

function refused(reason) {
  return {
    ok: false,
    reason: reason,
    network: false,
    followedUrl: false,
    rawLogged: false,
    grantsControl: false,
    longTermSecret: false,
  };
}

function scheme(name) {
  return name + '://';
}

function looksLikeLocator(text) {
  var lower = text.toLowerCase();
  var schemes = [
    scheme('http'),
    scheme('https'),
    scheme('ws'),
    scheme('wss'),
    scheme('weixin'),
  ];
  var i;
  for (i = 0; i < schemes.length; i += 1) {
    if (lower.indexOf(schemes[i]) === 0) return true;
  }
  if (lower.indexOf('javascript:') === 0) return true;
  if (text.indexOf('://') >= 0) return true;
  if (text.indexOf('/') >= 0) return true;
  if (text.indexOf('\\') >= 0) return true;
  if (text.indexOf('?') >= 0) return true;
  if (text.indexOf('#') >= 0) return true;
  if (text.indexOf('..') >= 0) return true;
  if (text.indexOf('@') >= 0) return true;
  return false;
}

function parseDemoCode(input) {
  if (typeof input !== 'string') return refused('unknown_format');
  var text = input.trim();
  if (!text) return refused('empty');
  if (looksLikeLocator(text)) return refused('url_or_path_refused');
  if (text.length > 10) return refused('secret_refused');
  if (/secret|token|password|bearer|key/i.test(text)) return refused('secret_refused');
  if (!/^OSC-[A-Z0-9]{6}$/.test(text)) return refused('unknown_format');
  var fixture = fixtures.fixtureByCode(text);
  if (!fixture) return refused('unknown_code');
  return {
    ok: true,
    reason: 'recognized_fixture',
    network: false,
    followedUrl: false,
    rawLogged: false,
    grantsControl: false,
    longTermSecret: false,
    redacted: 'OSC-******',
    fixtureId: fixture.fixtureId,
  };
}

module.exports = {
  parseDemoCode: parseDemoCode,
};
