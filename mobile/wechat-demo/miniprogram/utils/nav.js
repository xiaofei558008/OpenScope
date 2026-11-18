'use strict';

function routeOf(url) {
  return String(url || '').replace(/^\//, '');
}

function shouldRedirect(currentRoute, url) {
  var target = routeOf(url);
  if (!target) return false;
  return currentRoute !== target;
}

function onNav(event) {
  var dataset = event && event.currentTarget ? event.currentTarget.dataset : null;
  var url = dataset ? dataset.url : '';
  var pages = typeof getCurrentPages === 'function' ? getCurrentPages() : [];
  var current = pages.length ? pages[pages.length - 1].route : '';
  if (!shouldRedirect(current, url)) return;
  wx.redirectTo({ url: url });
}

module.exports = {
  routeOf: routeOf,
  shouldRedirect: shouldRedirect,
  onNav: onNav,
};
