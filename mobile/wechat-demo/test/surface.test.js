'use strict';

var test = require('node:test');
var assert = require('node:assert/strict');
var fs = require('node:fs');
var path = require('node:path');

var root = path.resolve(__dirname, '..');
var mini = path.join(root, 'miniprogram');

function walk(dir, acc) {
  var names = fs.readdirSync(dir);
  var i;
  for (i = 0; i < names.length; i += 1) {
    var full = path.join(dir, names[i]);
    var stat = fs.statSync(full);
    if (stat.isDirectory()) walk(full, acc);
    else acc.push(full);
  }
  return acc;
}

test('tourist appid is set and the mini-program source has no network path', function () {
  var config = JSON.parse(fs.readFileSync(path.join(root, 'project.config.json'), 'utf8'));
  assert.equal(config.appid, 'touristappid');
  assert.equal(config.compileType, 'miniprogram');
  assert.equal(config.setting.urlCheck, true);
  assert.equal(config.setting.preloadBackgroundData, false);
  assert.equal(config.setting.nodeModules, false);

  var files = walk(mini, []);
  files.push(path.join(root, 'project.config.json'));
  var banned = [
    /wx\.request\b/,
    /wx\.connectSocket\b/,
    /wx\.downloadFile\b/,
    /wx\.uploadFile\b/,
    /wx\.requestPayment\b/,
    /wx\.scanCode\b/,
    /wx\.createCameraContext\b/,
    /wx\.login\b/,
    /getUserProfile\b/,
    /\bfetch\s*\(/,
    /XMLHttpRequest/,
    /\bWebSocket\b/,
    /require\(['"]https?['"]\)/,
    /require\(['"]net['"]\)/,
    /require\(['"]ws['"]\)/,
    /https?:\/\//,
    /wss?:\/\//,
  ];
  var i;
  var b;
  for (i = 0; i < files.length; i += 1) {
    var text = fs.readFileSync(files[i], 'utf8');
    for (b = 0; b < banned.length; b += 1) {
      assert.equal(banned[b].test(text), false, path.relative(root, files[i]) + ' matched ' + banned[b]);
    }
  }

  var app = JSON.parse(fs.readFileSync(path.join(mini, 'app.json'), 'utf8'));
  assert.ok(app.pages.indexOf('pages/mode/mode') === 0);
  assert.ok(app.pages.indexOf('pages/pair/pair') >= 0);
  assert.ok(app.pages.indexOf('pages/devices/devices') >= 0);
  assert.ok(app.pages.indexOf('pages/browser/browser') >= 0);
  assert.ok(app.pages.indexOf('pages/monitor/monitor') >= 0);
  assert.ok(app.pages.indexOf('pages/commands/commands') >= 0);
  assert.ok(app.pages.indexOf('pages/plan/plan') >= 0);

  var monitor = fs.readFileSync(path.join(mini, 'pages/monitor/monitor.js'), 'utf8');
  assert.equal(monitor.indexOf('setInterval') >= 0, true);
  assert.equal(monitor.indexOf('clearInterval') >= 0, true);
  assert.equal(monitor.indexOf('onHide') >= 0, true);
  assert.equal(fs.readFileSync(path.join(mini, 'app.js'), 'utf8').indexOf('setInterval'), -1);
});
