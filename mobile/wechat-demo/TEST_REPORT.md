# TEST_REPORT

Revision check on 2026-11-17. Node: v22.23.4. Working directory: repository root.

WeChat DevTools is not installed in this workspace. The mini-program was not imported, compiled, or tapped here. There is no new screenshot. The 2026-11-15 Windows DevTools notes below are the user's report, not a run repeated in this workspace. Node tests are not simulator acceptance.

## Fixture tests (pass)

Command:

```bash
node --test --test-reporter spec mobile/wechat-demo/test/domain.test.js mobile/wechat-demo/test/surface.test.js mobile/wechat-demo/test/nav.test.js
```

`node --check` on every `.js` file under `mobile/wechat-demo` exited 0 before this command. Spec exit code: 0.

```text
✔ pairing stays pending, can be denied, and otherwise defaults to viewer (2.963277ms)
✔ the phone cannot approve its own pairing request (0.301002ms)
✔ hide clears the stale generation and show starts a fresh simulated session (1.623089ms)
✔ a gap is not rendered as zero or as a connected line (2.869661ms)
✔ sample rings stay inside the bounded limit (0.358951ms)
✔ payment return, server paid, and entitlement are independent (0.400379ms)
✔ a failed mode does not fall back or switch from LAN to relay (1.539935ms)
✔ unknown command is not replayed and disconnect is not a confirmed failure (0.281402ms)
✔ malicious demo QR text never becomes a request and is not logged raw (0.602728ms)
✔ revoking a remembered device removes access (0.610138ms)
✔ the same identity, request, and binding restores pending or denied without a second redemption (0.420168ms)
✔ a different claimant, an expired code, or a revoked code is denied (1.432405ms)
✔ self-host attempt is unimplemented transport, not a platform rejection (0.335735ms)
✔ skipped verify does not advance, and the verified order reaches viewer (0.264081ms)
✔ executable controls are rejected and never run (0.20535ms)
✔ missing readings render as a dash and demo schema stays injected (0.813693ms)
✔ section buttons redirect only to a different page (0.744773ms)
✔ tourist appid is set and the mini-program source has no network path (13.237775ms)
✔ nav buttons are page-level and docs do not claim tourist CLI open works (0.442619ms)
ℹ tests 19
ℹ suites 0
ℹ pass 19
ℹ fail 0
ℹ cancelled 0
ℹ skipped 0
ℹ todo 0
ℹ duration_ms 65.789856
```

Counts: 19 tests, 19 passed, 0 failed.

## Directory invocation (fail)

Command:

```bash
node --test mobile/wechat-demo/test
```

Exit code: 1. This Node loads the directory as a module and does not run the fixture files.

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
  duration_ms: 26.378655
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
# duration_ms 32.420092
```

Counts for this invocation: 1 test, 0 passed, 1 failed. Use the file list above.

## Not run

- WeChat DevTools import, CLI open, simulator, or device
- Taps on the new section buttons (the earlier automator reached pages with `redirectTo` and did not verify nav taps)
- Desktop build, Android build, network service, payment API, or hardware
- Repository CI: the repo has no Actions workflows and the previous head had no check runs

## User-reported DevTools evidence (not re-run here)

On 2026-11-15, Windows DevTools rejected a CLI open that used the placeholder `touristappid`. A local `project.private.config.json` AppID override, using the user's official mini-program test account, loaded the mode page. A later automator pass completed submit, 校验声称, preview, PC request (redemption 1, grant 0), simulated PC approve, and viewer with control off and transport disconnected. A separate reject fixture ended denied with grant 0. An earlier pass skipped 校验声称 and could not advance. Section buttons were not tapped. Self-host synthetic playback stayed 0/180 and stopped; this demo has no transport, so that is not recorded here as a connection bug.
