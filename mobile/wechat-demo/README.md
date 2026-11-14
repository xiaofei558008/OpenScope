# OpenScope WeChat mini-program DEMO shell

This directory is an isolated, importable WeChat mini-program shell. It is a local fixture and a simulated state machine. It is not a desktop OpenScope build, not an Android client, and not a production remote-control product.

The repository was inspected before this directory was added. There was no existing mini-program, WeChat project, or `mobile/` tree, and nothing named `wechat-demo`. Desktop code, installers, plugins, and docs were left in place.

`cc046627e3b9ed08253ca2ff54929a06ac3cd8c7` is only the git revision this demo was branched from. It is not a frozen integration contract. A local `project_tab` revision `d67c139` was not available here and was not reconciled. Schema names, TTL, and ids inside `miniprogram/domain/` are injected demo values. They are not a frozen wire protocol. Do not wait on this demo for a real cloud protocol.

## What you can open

1. Install WeChat DevTools on a machine that has it. This cloud workspace did not have DevTools, a phone, or a mini-program simulator.
2. Import the project directory that contains `project.config.json`: `mobile/wechat-demo`.
3. Leave the AppID as `touristappid`. If the import dialog asks, choose the test / tourist account. This demo does not need a private AppID, and it does not register one.
4. Compile. The first page is connection-mode selection. A DEMO / simulated-data banner stays on every page. It cannot be dismissed.

There is no service to start. Do not point it at a PC, a relay, or a payment provider. The connect action always fails closed and does not switch mode or play “online” data as a fallback.

## Pages

- Mode: LAN direct to a PC, managed paid relay, or self-host relay. These are choices only. Self-host shows the WeChat platform restriction: a bare public IP is not a legal request or socket domain. The typed endpoint is discarded and is not stored.
- Pair: simulated scan. Type a short code or use a fixture button. The page states that the camera is not opened. Claimed server/PC and verified preview are separate. Pairing can be pending, denied, expired, or viewer. The phone cannot approve itself. A labeled peer-PC fixture button is the only way to reach viewer. The same verified identity + request + binding can restore pending or denied without a second redemption, a new grant, or a TTL extension.
- Devices: remembered simulated devices and revoke. Revoke removes catalog and sample access.
- Browser: project / module / variable tree from local fixtures, only while the role is viewer and the device is not revoked.
- Monitor: bounded synthetic numbers and a canvas waveform. The ring holds 180 entries. The page refreshes about every 320 ms only while it is visible and the local generator is running. Hide pauses that timer and clears stale samples. Show starts a new generation. Gaps break the line and are not drawn as zero. The transport status stays “not connected” during local playback.
- Commands: display-only states (rejected, pending PC local confirmation, received, accepted, running, succeeded, failed, unknown). Raw state and local confidence are separate. Disconnect does not rewrite the raw state into failure and does not replay. Writes, RUN_CTL, flash, Shell, and arbitrary commands are rejected and not executed. Control stays off. The default granted role is viewer.
- Plan: payment-return, server-paid, and entitlement fixtures. They do not imply each other. There is no price, order, payment SDK, or secret.

## Tests

From the repository root, with Node 20 or newer:

```bash
node --test mobile/wechat-demo/test/*.test.js
```

Pass the test files. On Node v22.23.4, `node --test mobile/wechat-demo/test` treats that directory as a module and fails with `MODULE_NOT_FOUND`; that is a CLI invocation failure, not a failed fixture test. The tests use `node:test` and `node:assert`. They do not open a socket, bind a port, or call WeChat. See `TEST_REPORT.md` for the command output from this workspace. WeChat DevTools was not installed here, so the simulator and a device were not run, and there is no screenshot.

## Limitations

- No network path. Source under `miniprogram/` does not call `wx.request`, sockets, download/upload, login, payment, or scan APIs. A failed connect does not fall back to mock online data, and LAN does not auto-switch to a relay.
- Encryption is not implemented. TLS is not claimed, and TLS would not be end-to-end encryption even if a later build added HTTPS.
- Short codes are not control permission and are not long-term secrets. Unknown QR text fails closed. URLs and paths are not followed, and raw rejected text is not kept in the snapshot.
- The waveform is synthetic. It is not samples from a probe, an MCU, or desktop OpenScope.
- Remote debugging in a finished product is out of scope. This shell only shows command UI states.
- Android is not part of this directory.
- This demo is not production-ready and does not integrate with the desktop program.
