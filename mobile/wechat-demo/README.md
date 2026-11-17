# OpenScope WeChat mini-program DEMO shell

This directory is an isolated, importable WeChat mini-program shell. It is a local fixture and a simulated state machine. It is not a desktop OpenScope build, not an Android client, and not a production remote-control product.

The repository was inspected before this directory was added. There was no existing mini-program, WeChat project, or `mobile/` tree, and nothing named `wechat-demo`. Desktop code, installers, plugins, and docs were left in place.

`cc046627e3b9ed08253ca2ff54929a06ac3cd8c7` is only the git revision this demo was branched from. It is not a frozen integration contract. A local `project_tab` revision `d67c139` was not available here and was not reconciled. Schema names, TTL, and ids inside `miniprogram/domain/` are injected demo values. They are not a frozen wire protocol. Do not wait on this demo for a real cloud protocol.

## What you can open

1. Install WeChat DevTools on your own machine. This cloud workspace does not have DevTools, a phone, or a mini-program simulator, so nothing here was accepted by a simulator.
2. Import the project directory that contains `project.config.json`: `mobile/wechat-demo`.
3. The committed `appid` value `touristappid` is only a public placeholder. It is not a working DevTools open path. On Windows WeChat DevTools (2026-11-15) a CLI open with `touristappid` was invalid.
4. To open it locally, create `mobile/wechat-demo/project.private.config.json` on your machine and set `appid` to the test AppID of your own official mini-program test account. That file is gitignored and overrides the committed placeholder. A local override of that kind loaded the mode page. Do not commit the file, and do not put a real AppID in this repository.

```json
{
  "description": "Local DevTools override. Do not commit.",
  "appid": "<your official mini-program test AppID>"
}
```

5. Compile. The first page is connection-mode selection. A DEMO / simulated-data banner stays on every page. It cannot be dismissed. Section changes are ordinary buttons (`nav-mode`, `nav-pair`, and the other `nav-*` ids) that call `wx.redirectTo`. This workspace did not tap them in DevTools.

There is no service to start. Do not point it at a PC, a relay, or a payment provider. The connect action always fails closed and does not switch mode or play “online” data as a fallback.

## Pages

- Mode: LAN direct to a PC, managed paid relay, or self-host relay. These are choices only. This demo implements no transport in any of them. LAN is a future same-network path to a PC; this demo does not call that a WeChat platform ban and does not add a gateway requirement. Self-host has a separate production prerequisite: a real mini-program request would need a legal request/socket domain, and a bare public IP is not that domain. That sentence is not an observed connection failure. The typed endpoint is discarded and is not stored.
- Pair: simulated scan. Type a short code or use a fixture button. The page states the next required step. Submit, then 校验声称 (verified preview, not authorization), then request PC approval. The phone cannot approve itself. A labeled peer-PC fixture button is the only way to reach viewer. Skipping 校验声称 does not advance. The same verified identity + request + binding can restore pending or denied without a second redemption, a new grant, or a TTL extension.
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

- No network path. Source under `miniprogram/` does not call `wx.request`, sockets, download/upload, login, payment, or scan APIs. A failed connect does not fall back to mock online data, and LAN does not auto-switch to a relay. Local synthetic playback is not a connection. It stays stopped outside LAN mode so a curve is not mistaken for a relay session; on the self-host page that stopped state is the missing demo transport, not a platform rejection that already happened.
- A real desktop peer that can approve pairing, a real LAN path, and a future self-host legal domain are not implemented here. Those contracts are still open. Android is a separate branch and is not in this directory.
- Encryption is not implemented. TLS is not claimed, and TLS would not be end-to-end encryption even if a later build added HTTPS.
- Short codes are not control permission and are not long-term secrets. Unknown QR text fails closed. URLs and paths are not followed, and raw rejected text is not kept in the snapshot.
- The waveform is synthetic. It is not samples from a probe, an MCU, or desktop OpenScope.
- Remote debugging in a finished product is out of scope. This shell only shows command UI states.
- Android is not part of this directory.
- This demo is not production-ready and does not integrate with the desktop program.
