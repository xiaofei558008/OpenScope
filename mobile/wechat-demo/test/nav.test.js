'use strict';

var test = require('node:test');
var assert = require('node:assert/strict');
var nav = require('../miniprogram/utils/nav.js');

test('section buttons redirect only to a different page', function () {
  assert.equal(nav.shouldRedirect('pages/mode/mode', '/pages/mode/mode'), false);
  assert.equal(nav.shouldRedirect('pages/mode/mode', '/pages/pair/pair'), true);
  assert.equal(nav.shouldRedirect('pages/pair/pair', ''), false);
  assert.equal(nav.shouldRedirect('', '/pages/monitor/monitor'), true);
  assert.equal(nav.routeOf('/pages/commands/commands'), 'pages/commands/commands');
});
