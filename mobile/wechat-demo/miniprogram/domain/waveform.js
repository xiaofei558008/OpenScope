'use strict';

// Pure drawing helper. A gap is the absence of a sample: it never becomes 0
// and it never joins the samples on either side into one line.

function buildDrawSegments(entries, width, height, yMin, yMax) {
  var segments = [];
  var current = [];
  var list = entries || [];
  var total = list.length;
  var span = yMax === yMin ? 1 : (yMax - yMin);
  var usableH = height - 8;
  if (usableH < 1) usableH = 1;

  function flush() {
    if (current.length) segments.push({ points: current });
    current = [];
  }

  var i;
  for (i = 0; i < total; i += 1) {
    var entry = list[i];
    if (!entry || entry.kind === 'gap' || typeof entry.value !== 'number') {
      flush();
      continue;
    }
    var x = total === 1 ? width / 2 : (i / (total - 1)) * width;
    var yNorm = (entry.value - yMin) / span;
    var y = height - 4 - (yNorm * usableH);
    current.push({ x: x, y: y, value: entry.value, t: entry.t });
  }
  flush();
  return segments;
}

module.exports = {
  buildDrawSegments: buildDrawSegments,
};
