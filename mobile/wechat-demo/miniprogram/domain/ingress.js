'use strict';

// Demo-local sample gate. generation is the existing epoch field.
// source is only an argument to this gate, not a new stored sample field.

function finiteValue(value) {
  return typeof value === 'number' && value === value && value !== Infinity && value !== -Infinity;
}

function judgeSample(cursor, sample) {
  sample = sample || {};
  if (!finiteValue(sample.value)) return { action: 'reject', reason: 'non_finite' };
  if (!finiteValue(sample.generation)) return { action: 'reject', reason: 'bad_epoch' };
  if (typeof sample.source !== 'string' || sample.source.length === 0) {
    return { action: 'reject', reason: 'bad_source' };
  }
  if (!cursor || cursor.epoch == null) return { action: 'accept', reason: 'established' };
  if (sample.generation < cursor.epoch) return { action: 'reject', reason: 'stale_epoch' };
  if (sample.generation === cursor.epoch && sample.source !== cursor.source) {
    return { action: 'reject', reason: 'stale_source' };
  }
  if (sample.generation !== cursor.epoch) return { action: 'switch', reason: 'epoch_switch' };
  return { action: 'accept', reason: 'current' };
}

module.exports = {
  finiteValue: finiteValue,
  judgeSample: judgeSample,
};
