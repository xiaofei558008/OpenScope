# TEST_REPORT

Revision check on 2026-11-18. Node: v22.23.4. Working directory: repository root.

WeChat DevTools is not installed here. The mini-program was not imported, compiled, or tapped. There is no screenshot. Node tests are not simulator acceptance.

`node --check` on every `.js` file under `mobile/wechat-demo` exited 0 before the spec command below.

## Fixture tests (pass)

Command:

```bash
node --test --test-reporter spec mobile/wechat-demo/test/domain.test.js mobile/wechat-demo/test/surface.test.js mobile/wechat-demo/test/nav.test.js mobile/wechat-demo/test/ingress.test.js
```

Exit code: 0.

```text
✔ pairing stays pending, can be denied, and otherwise defaults to viewer (2.877612ms)
✔ the phone cannot approve its own pairing request (0.366121ms)
✔ hide clears the stale generation and show starts a fresh simulated session (1.039832ms)
✔ a gap is not rendered as zero or as a connected line (2.355991ms)
✔ sample rings stay inside the bounded limit (0.414105ms)
✔ payment return, server paid, and entitlement are independent (0.307549ms)
✔ a failed mode does not fall back or switch from LAN to relay (0.699808ms)
✔ unknown command is not replayed and disconnect is not a confirmed failure (0.209418ms)
✔ malicious demo QR text never becomes a request and is not logged raw (0.534665ms)
✔ revoking a remembered device removes access (0.590028ms)
✔ the same identity, request, and binding restores pending or denied without a second redemption (0.329066ms)
✔ a different claimant, an expired code, or a revoked code is denied (0.193405ms)
✔ self-host attempt is unimplemented transport, not a platform rejection (0.273495ms)
✔ skipped verify does not advance, and the verified order reaches viewer (0.211692ms)
✔ executable controls are rejected and never run (0.938121ms)
✔ missing readings render as a dash and demo schema stays injected (0.232321ms)
✔ cancel clears a pending demo request without a grant or a PC session (4.193339ms)
✔ ingress rejects stale and non-finite samples without using a slot or plotting zero (1.37972ms)
✔ section buttons redirect only to a different page (0.611845ms)
✔ tourist appid is set and the mini-program source has no network path (9.427479ms)
✔ nav buttons are page-level and docs do not claim tourist CLI open works (0.412081ms)
ℹ tests 21
ℹ suites 0
ℹ pass 21
ℹ fail 0
ℹ cancelled 0
ℹ skipped 0
ℹ todo 0
ℹ duration_ms 92.664582
```

Counts: 21 tests, 21 passed, 0 failed.

## Directory invocation (fail)

Command:

```bash
node --test mobile/wechat-demo/test
```

Exit code: 1. This Node still loads that directory as a module and does not run the fixture files.

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
  duration_ms: 27.878935
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
# duration_ms 33.576034
```

Counts for this invocation: 1 test, 0 passed, 1 failed. Use the file list above.

## Not run

- WeChat DevTools import, simulator, device, or nav taps
- Desktop build, Android build, network service, payment API, or hardware
- Repository CI: no Actions workflows and no check runs were present when this branch was refreshed

## Still not a wire contract

Sample `generation` is the existing demo epoch field. The ingress `source` argument is not stored on samples and is not a handshake or message-boundary contract. Cancel does not talk to a PC.
