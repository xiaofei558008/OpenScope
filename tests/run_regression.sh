#!/usr/bin/env bash
# 全量 UI 回归：开发版 + 安装版，各跑一遍全部 ui_*_drive 脚本。
# 用法: bash tests/run_regression.sh [--jlink]
set -u
ROOT="D:/OpenScope"
DEV="$ROOT/bin/Release/OpenScope.exe"
INSTALLED="C:/Program Files/OpenScope/OpenScope.exe"
JROOT="$ROOT/module/jlink/tests"

NON_JLINK=(
  ui_windows_drive.ps1
  ui_layout_drive.ps1
  ui_chartview_drive.ps1
  ui_bug2_restore_drive.ps1
  ui_n9_watch_drive.ps1
  ui_rename_drive.ps1
  ui_features_drive.ps1
  ui_chart_n13_drive.ps1
  ui_pick_multi_drive.ps1
  ui_logsplit_drive.ps1
  ui_rightmenu_drive.ps1
  ui_tree_multisel_drive.ps1
  ui_chart_bug5_drive.ps1
)
JLINK=(
  ui_record_dialog_drive.ps1
  ui_connect_drive.ps1
  ui_speed12000_drive.ps1
)

run_one() { # <script> <exe> <label>
  local script="$1" exe="$2" label="$3"
  taskkill //F //IM OpenScope.exe >/dev/null 2>&1
  sleep 1
  if [[ "$script" == *ui_connect_drive.ps1* ]]; then
    powershell -ExecutionPolicy Bypass -File "$script" -ExePath "$exe" >/tmp/reg_out.txt 2>&1
  else
    powershell -ExecutionPolicy Bypass -File "$script" -ExePath "$exe" >/tmp/reg_out.txt 2>&1
  fi
  local rc=$?
  local verdict="ALL PASS"
  if [[ $rc -ne 0 ]]; then verdict="FAILURES(rc=$rc)"; fi
  echo "[$label] $script -> $verdict"
  grep -E "ALL PASS|FAILURES|PASS |FAIL " /tmp/reg_out.txt | tail -40
  taskkill //F //IM OpenScope.exe >/dev/null 2>&1
  return $rc
}

pass=0; fail=0
for variant in dev installed; do
  [[ $variant == dev ]] && exe="$DEV" || exe="$INSTALLED"
  echo "===== variant: $variant ($exe) ====="
  for t in "${NON_JLINK[@]}"; do
    if run_one "$ROOT/tests/$t" "$exe" "$variant"; then pass=$((pass+1)); else fail=$((fail+1)); fi
  done
  if [[ "${1:-}" == "--jlink" ]]; then
    for t in "${JLINK[@]}"; do
      script="$ROOT/tests/$t"
      [[ -f "$script" ]] || script="$JROOT/$t"
      if run_one "$script" "$exe" "$variant"; then pass=$((pass+1)); else fail=$((fail+1)); fi
    done
  fi
done
echo "======================================"
echo "TOTAL PASS=$pass FAIL=$fail"
exit $fail
