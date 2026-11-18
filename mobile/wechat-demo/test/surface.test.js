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

test('nav buttons are page-level and docs do not claim tourist CLI open works', function () {
  var readme = fs.readFileSync(path.join(root, 'README.md'), 'utf8');
  var ignore = fs.readFileSync(path.join(root, '.gitignore'), 'utf8');
  assert.ok(readme.indexOf('not a working DevTools open path') >= 0);
  assert.ok(readme.indexOf('project.private.config.json') >= 0);
  assert.ok(ignore.indexOf('project.private.config.json') >= 0);
  assert.equal(/wx[0-9a-f]{16}/i.test(readme), false);

  var nav = fs.readFileSync(path.join(mini, 'templates/section-nav.wxml'), 'utf8');
  var ids = ['nav-mode', 'nav-pair', 'nav-devices', 'nav-browser', 'nav-monitor', 'nav-commands', 'nav-plan'];
  var i;
  for (i = 0; i < ids.length; i += 1) {
    assert.ok(nav.indexOf('id="' + ids[i] + '"') >= 0);
  }
  assert.ok(nav.indexOf('<button') >= 0);
  assert.equal(nav.indexOf('<navigator'), -1);

  var pages = ['mode', 'pair', 'devices', 'browser', 'monitor', 'commands', 'plan'];
  for (i = 0; i < pages.length; i += 1) {
    var wxml = fs.readFileSync(path.join(mini, 'pages', pages[i], pages[i] + '.wxml'), 'utf8');
    var js = fs.readFileSync(path.join(mini, 'pages', pages[i], pages[i] + '.js'), 'utf8');
    assert.ok(wxml.indexOf('template is="section-nav"') >= 0);
    assert.equal(wxml.indexOf('<navigator'), -1);
    assert.ok(js.indexOf('onNav: nav.onNav') >= 0);
  }
  var pair = fs.readFileSync(path.join(mini, 'pages/pair/pair.wxml'), 'utf8');
  assert.ok(pair.indexOf('id="pair-next"') >= 0);
  assert.ok(pair.indexOf('id="pair-verify"') >= 0);
  assert.ok(pair.indexOf('id="pair-cancel"') >= 0);
  assert.ok(pair.indexOf('id="pair-cancel-block"') >= 0);
  assert.ok(pair.indexOf('id="pair-verify-block"') >= 0);
  assert.ok(pair.indexOf('disabled="{{!snap.actions.canRequest}}"') >= 0);
});
