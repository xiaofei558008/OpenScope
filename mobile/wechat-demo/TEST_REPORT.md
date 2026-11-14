# TEST_REPORT

Workspace date: 2026-11-14. Node: v22.23.4 (`/exec-daemon/node`). Working directory: repository root.

No WeChat DevTools binary, mini-program simulator, or device was available in this workspace (`wechatdevtools` was not on `PATH`). The mini-program was not imported, compiled, or screenshotted. There is no rendered screenshot.

Product code was not changed after the commands below. This file was written from their output.

## Fixture tests (pass)

Command:

```bash
node --test --test-reporter spec mobile/wechat-demo/test/domain.test.js mobile/wechat-demo/test/surface.test.js
```

Exit code: 0.

```text
✔ pairing stays pending, can be denied, and otherwise defaults to viewer (2.347927ms)
✔ the phone cannot approve its own pairing request (0.236144ms)
✔ hide clears the stale generation and show starts a fresh simulated session (0.565938ms)
✔ a gap is not rendered as zero or as a connected line (1.383532ms)
✔ sample rings stay inside the bounded limit (0.34253ms)
✔ payment return, server paid, and entitlement are independent (0.293029ms)
✔ a failed mode does not fall back or switch from LAN to relay (0.478287ms)
✔ unknown command is not replayed and disconnect is not a confirmed failure (0.17921ms)
✔ malicious demo QR text never becomes a request and is not logged raw (0.495184ms)
✔ revoking a remembered device removes access (0.485548ms)
✔ the same identity, request, and binding restores pending or denied without a second redemption (0.302419ms)
✔ a different claimant, an expired code, or a revoked code is denied (0.168243ms)
✔ self-host bare public IP is a platform restriction and not a connection (0.186872ms)
✔ executable controls are rejected and never run (0.168917ms)
✔ missing readings render as a dash and demo schema stays injected (0.167066ms)
✔ tourist appid is set and the mini-program source has no network path (8.889344ms)
ℹ tests 16
ℹ suites 0
ℹ pass 16
ℹ fail 0
ℹ cancelled 0
ℹ skipped 0
ℹ todo 0
ℹ duration_ms 52.847109
```

Counts: 16 tests, 16 passed, 0 failed. `node --check` on every `.js` file under `mobile/wechat-demo` also exited 0 (syntax only; not a behavior test).

The same 16 tests also passed when invoked as `node --test mobile/wechat-demo/test/*.test.js` in an earlier run on this Node (TAP, 16 pass, 0 fail). The spec run above is the record for the final tree before this report file.

## Directory invocation (fail)

Command:

```bash
node --test mobile/wechat-demo/test
```

Exit code: 1.

This Node build tries to load the directory as a module. It does not run the fixture files. Output:

```text
TAP version 13
# node:internal/modules/cjs/loader:1433
#   throw err;
#   ^
# Error: Cannot find module '/agent/repos/OpenScope/mobile/wechat-demo/test'
#     at Function._resolveFilename (node:internal/modules/cjs/loader:1430:15)
#     at defaultResolveImpl (node:internal/modules/cjs/loader:1040:19)
#     at resolveForCJSWithHooks (node:internal/modules/cjs/loader:1045:22)
#     at Function._load (node:internal/modules/cjs/loader:1216:25)
#     at wrapModuleLoad (node:internal/modules/cjs/loader:254:19)
#     at Function.executeUserEntryPoint [as runMain] (node:internal/modules/run_main:171:5)
#     at node:internal/main/run_main_module:36:49 {
#   code: 'MODULE_NOT_FOUND',
#   requireStack: []
# }
# Node.js v22.23.4
# Subtest: mobile/wechat-demo/test
not ok 1 - mobile/wechat-demo/test
  ---
  duration_ms: 25.038771
  type: 'test'
  location: '/agent/repos/OpenScope/mobile/wechat-demo/test:1:1'
  failureType: 'testCodeFailure'
  exitCode: 1
  signal: ~
  error: 'test failed'
  code: 'ERR_TEST_FAILURE'
  ...
1..1
# tests 1
# suites 0
# pass 0
# fail 1
# cancelled 0
# skipped 0
# todo 0
# duration_ms 30.402441
```

Counts for this invocation: 1 test, 0 passed, 1 failed. Use the file list in the section above.

## Not run

- WeChat DevTools import
- Simulator
- Physical phone or tablet
- Screenshot or screen recording
- Desktop OpenScope build
- Any network service, relay, payment API, or hardware probe
