'use strict';

// Injected DEMO fixtures. These ids, TTL, and labels are not a wire protocol.

var DEMO_SCHEMA = {
  id: 'openscope-wechat-demo-fixtures',
  version: 'demo-injected-not-wire',
  ttlMs: 120000,
  encryption: 'not-implemented',
  e2ee: false,
  network: false,
};

var MODES = {
  lan_direct: {
    id: 'lan_direct',
    label: '局域网直连 PC',
    detail: '只是模式选择。这个构建没有局域网通道。连接失败不会自动改走云端，也不会改播模拟在线数据。',
  },
  managed_relay: {
    id: 'managed_relay',
    label: '托管付费阿里云中继',
    detail: '只是模式选择。支付返回、服务端已付、权益生效是三件独立的事。这里不会连接中继，也没有价格或支付 SDK。',
  },
  selfhost_relay: {
    id: 'selfhost_relay',
    label: '自建中继',
    detail: '裸公网 IP 不能作为微信小程序的 request 或 socket 合法域名。这里只展示平台限制，不会假装连上。',
  },
};

var MODE_ORDER = ['lan_direct', 'managed_relay', 'selfhost_relay'];

var PROJECTS = [
  {
    id: 'proj-demo',
    name: 'DemoProject',
    modules: [
      {
        id: 'mod-motor',
        name: 'Motor',
        variables: [
          { id: 'var-speed', name: 'speed_rpm', type: 'float', unit: 'rpm', min: 800, max: 2400 },
          { id: 'var-temp', name: 'temp_c', type: 'float', unit: 'C', min: 20, max: 80 },
        ],
      },
      {
        id: 'mod-ctrl',
        name: 'Controller',
        variables: [
          { id: 'var-count', name: 'loop_count', type: 'uint32', unit: 'count', min: 1, max: 500 },
        ],
      },
    ],
  },
];

var CODE_FIXTURES = [
  {
    key: 'demo',
    code: 'OSC-DEMO01',
    fixtureId: 'fix-demo',
    identityId: 'ident-a',
    requestId: 'req-a',
    bindingId: 'bind-pc-1',
    claimantId: 'phone-self',
    revoked: false,
    expired: false,
    claimed: { server: '声称的局域网 PC', pc: 'PC-CLAIMED' },
    verified: { server: '夹具 PC-LAB-1', pc: 'PC-LAB-1' },
  },
  {
    key: 'deny',
    code: 'OSC-DENY01',
    fixtureId: 'fix-deny',
    identityId: 'ident-b',
    requestId: 'req-b',
    bindingId: 'bind-pc-2',
    claimantId: 'phone-self',
    revoked: false,
    expired: false,
    claimed: { server: '声称会被拒绝', pc: 'PC-CLAIMED-DENY' },
    verified: { server: '夹具 PC-LAB-2', pc: 'PC-LAB-2' },
  },
  {
    key: 'expired',
    code: 'OSC-EXPR01',
    fixtureId: 'fix-expired',
    identityId: 'ident-c',
    requestId: 'req-c',
    bindingId: 'bind-pc-3',
    claimantId: 'phone-self',
    revoked: false,
    expired: true,
    claimed: { server: '声称已过期', pc: 'PC-OLD' },
    verified: { server: '夹具 PC-OLD', pc: 'PC-OLD' },
  },
  {
    key: 'revoked',
    code: 'OSC-REVOK1',
    fixtureId: 'fix-revoked',
    identityId: 'ident-d',
    requestId: 'req-d',
    bindingId: 'bind-pc-4',
    claimantId: 'phone-self',
    revoked: true,
    expired: false,
    claimed: { server: '声称已撤销', pc: 'PC-REVOKED' },
    verified: { server: '夹具 PC-REVOKED', pc: 'PC-REVOKED' },
  },
  {
    key: 'other',
    code: 'OSC-OTHR01',
    fixtureId: 'fix-other',
    identityId: 'ident-a',
    requestId: 'req-a',
    bindingId: 'bind-pc-1',
    claimantId: 'phone-other',
    revoked: false,
    expired: false,
    claimed: { server: '声称的其他手机', pc: 'PC-OTHER' },
    verified: { server: '夹具 PC-LAB-1', pc: 'PC-LAB-1' },
  },
];

function cloneFixture(row) {
  return {
    key: row.key,
    fixtureId: row.fixtureId,
    identityId: row.identityId,
    requestId: row.requestId,
    bindingId: row.bindingId,
    claimantId: row.claimantId,
    revoked: row.revoked,
    expired: row.expired,
    claimed: { server: row.claimed.server, pc: row.claimed.pc },
    verified: { server: row.verified.server, pc: row.verified.pc },
  };
}

function fixtureById(fixtureId) {
  var i;
  for (i = 0; i < CODE_FIXTURES.length; i += 1) {
    if (CODE_FIXTURES[i].fixtureId === fixtureId) return cloneFixture(CODE_FIXTURES[i]);
  }
  return null;
}

function fixtureByCode(code) {
  var i;
  for (i = 0; i < CODE_FIXTURES.length; i += 1) {
    if (CODE_FIXTURES[i].code === code) return cloneFixture(CODE_FIXTURES[i]);
  }
  return null;
}

function codeForKey(key) {
  var i;
  for (i = 0; i < CODE_FIXTURES.length; i += 1) {
    if (CODE_FIXTURES[i].key === key) return CODE_FIXTURES[i].code;
  }
  return '';
}

function allVariables() {
  var list = [];
  var p;
  var m;
  var v;
  for (p = 0; p < PROJECTS.length; p += 1) {
    for (m = 0; m < PROJECTS[p].modules.length; m += 1) {
      for (v = 0; v < PROJECTS[p].modules[m].variables.length; v += 1) {
        var variable = PROJECTS[p].modules[m].variables[v];
        list.push({
          id: variable.id,
          name: variable.name,
          type: variable.type,
          unit: variable.unit,
          min: variable.min,
          max: variable.max,
          projectId: PROJECTS[p].id,
          projectName: PROJECTS[p].name,
          moduleId: PROJECTS[p].modules[m].id,
          moduleName: PROJECTS[p].modules[m].name,
        });
      }
    }
  }
  return list;
}

module.exports = {
  DEMO_SCHEMA: DEMO_SCHEMA,
  MODES: MODES,
  MODE_ORDER: MODE_ORDER,
  PROJECTS: PROJECTS,
  fixtureById: fixtureById,
  fixtureByCode: fixtureByCode,
  codeForKey: codeForKey,
  allVariables: allVariables,
};
