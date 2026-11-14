'use strict';

Component({
  properties: {
    current: {
      type: String,
      value: 'mode',
    },
  },
  data: {
    items: [
      { id: 'mode', label: '模式', url: '/pages/mode/mode' },
      { id: 'pair', label: '配对', url: '/pages/pair/pair' },
      { id: 'devices', label: '设备', url: '/pages/devices/devices' },
      { id: 'browser', label: '变量', url: '/pages/browser/browser' },
      { id: 'monitor', label: '波形', url: '/pages/monitor/monitor' },
      { id: 'commands', label: '命令', url: '/pages/commands/commands' },
      { id: 'plan', label: '权益', url: '/pages/plan/plan' },
    ],
  },
});
