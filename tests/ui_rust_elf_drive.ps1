# Rust 版 bug2 回归（request.md "rust开发 bug" #2：elf 按键不能加载 .out/.elf 文件）
#   根因：ffi.rs 的 OPENFILENAMEW 结构体误加 2 个成员，lStructSize=168（真实 152），
#   GetOpenFileNameW 直接失败 -> 点"加载 ELF"根本不弹文件对话框。
#   A. 点"加载 ELF"按键 -> 弹出文件对话框(#32770)，取消后关闭（校验对话框路径可用）
#   B. 命令行加载 linix_stm32l031_v1.2.out -> 树 636 个符号（校验解析+填充管线）
#   C. 命令行连续加载 linix.out 与 elf_sample.out -> 树被替换为 elf_sample 的 4 个符号
#      （校验 fill_tree 的 TVI_ROOT 清树；此前传 LPARAM(-1) 删不掉导致叠加）
param(
    [string]$OutFile = "D:/OpenScope/tests/linix_stm32l031_v1.2.out",
    [string]$ElfFile = "D:/OpenScope/tests/elf_sample.out",
    [string]$ExePath = "D:/OpenScope/rust/target/release/openscope-app.exe"
)
$ErrorActionPreference = "Stop"
Add-Type @"
using System;
using System.Runtime.InteropServices;
using System.Text;
public class OsElfUi {
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumChildWindows(IntPtr parent, EnumProc cb, IntPtr l);
    [DllImport("user32.dll")] public static extern uint GetWindowThreadProcessId(IntPtr h, out uint pid);
    [DllImport("user32.dll", CharSet=CharSet.Unicode)] public static extern int GetClassName(IntPtr h, StringBuilder sb, int max);
    [DllImport("user32.dll")] public static extern IntPtr SendMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr h, uint msg, IntPtr wp, IntPtr lp);
    public delegate bool EnumProc(IntPtr h, IntPtr l);
    [StructLayout(LayoutKind.Sequential, CharSet=CharSet.Unicode)]
    public class TVITEMW {
        public uint mask;
        public IntPtr hItem;
        public uint state;
        public uint stateMask;
        public StringBuilder pszText;
        public int cchTextMax;
        public int iImage;
        public int iSelectedImage;
        public int cChildren;
        public IntPtr lParam;
    }
}
"@

function Find-ByClass([int]$ProcId, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = { param($h,$l) $wp=[uint32]0; [OsElfUi]::GetWindowThreadProcessId($h,[ref]$wp)|Out-Null
        if ($wp -eq $ProcId) { $sb=New-Object System.Text.StringBuilder 256; [OsElfUi]::GetClassName($h,$sb,256)|Out-Null
            if ($sb.ToString() -eq $Class) { $script:hit=$h; return $false } }; return $true }
    [OsElfUi]::EnumWindows($cb,[IntPtr]::Zero)|Out-Null; return $script:hit
}
function Find-ChildByClass([IntPtr]$Parent, [string]$Class) {
    $script:hit = [IntPtr]::Zero
    $cb = { param($h,$l) $sb=New-Object System.Text.StringBuilder 128; [OsElfUi]::GetClassName($h,$sb,128)|Out-Null
        if ($sb.ToString() -eq $Class) { $script:hit=$h; return $false }; return $true }
    [OsElfUi]::EnumChildWindows($Parent,$cb,[IntPtr]::Zero)|Out-Null; return $script:hit
}

$fails = 0
function Check([bool]$Ok, [string]$What) {
    Write-Output ("{0} {1}" -f ($(if ($Ok) { "PASS" } else { "FAIL" })), $What)
    if (-not $Ok) { $script:fails++ }
}
function TreeCount([IntPtr]$tree) {
    if ($tree -eq [IntPtr]::Zero) { return 0 }
    return [OsElfUi]::SendMessage($tree, 0x1105, [IntPtr]0, [IntPtr]0).ToInt64()  # TVM_GETCOUNT
}
function FirstTreeItemText([IntPtr]$tree) {
    if ($tree -eq [IntPtr]::Zero) { return "" }
    $root = [OsElfUi]::SendMessage($tree, 0x110B, [IntPtr]0, [IntPtr]0)  # TVM_GETNEXTITEM TVGN_ROOT=0
    if ($root -eq [IntPtr]::Zero) { return "" }
    $buf = New-Object System.Text.StringBuilder 512
    $item = New-Object OsElfUi+TVITEMW
    $item.mask = 1  # TVIF_TEXT
    $item.hItem = $root
    $item.pszText = $buf
    $item.cchTextMax = 512
    $ptr = [System.Runtime.InteropServices.Marshal]::AllocHGlobal(256)
    try {
        $itemPtr = [System.Runtime.InteropServices.Marshal]::AllocHGlobal([System.Runtime.InteropServices.Marshal]::SizeOf($item))
        try {
            [System.Runtime.InteropServices.Marshal]::StructureToPtr($item, $itemPtr, $false)
            [OsElfUi]::SendMessage($tree, 0x112F, [IntPtr]0, $itemPtr) | Out-Null  # TVM_GETITEMW
            $back = [System.Runtime.InteropServices.Marshal]::PtrToStructure($itemPtr, [type][OsElfUi+TVITEMW])
            return $back.pszText.ToString()
        } finally { [System.Runtime.InteropServices.Marshal]::FreeHGlobal($itemPtr) }
    } finally { [System.Runtime.InteropServices.Marshal]::FreeHGlobal($ptr) }
}
function Get-Tree([IntPtr]$main) { return Find-ChildByClass $main "SysTreeView32" }

$existing = @(Get-Process openscope-app -ErrorAction SilentlyContinue)
if ($existing.Count -gt 0) { Write-Output "ERROR: app already running: $($existing.Id -join ',')"; exit 3 }

# ---------- A. 按键弹对话框 ----------
$proc = Start-Process -FilePath $ExePath -PassThru
try {
    $main = [IntPtr]::Zero
    for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
        $main = Find-ByClass $proc.Id "OpenScopeMain"
        if ($main -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Check ($main -ne [IntPtr]::Zero) "主窗口创建"
    Start-Sleep -Milliseconds 800
    [OsElfUi]::PostMessage($main, 0x111, [IntPtr]2007, [IntPtr]0) | Out-Null  # IDC_BTN_LOAD_ELF
    $dlg = [IntPtr]::Zero
    for ($i = 0; $i -lt 40 -and $dlg -eq [IntPtr]::Zero -and -not $proc.HasExited; $i++) {
        $dlg = Find-ByClass $proc.Id "#32770"
        if ($dlg -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Check ($dlg -ne [IntPtr]::Zero) "A1 点击加载ELF弹出文件对话框（bug2 修复）"
    if ($dlg -ne [IntPtr]::Zero) {
        [OsElfUi]::SendMessage($dlg, 0x111, [IntPtr]2, [IntPtr]0) | Out-Null  # IDCANCEL
        for ($i = 0; $i -lt 20; $i++) {
            if ((Find-ByClass $proc.Id "#32770") -eq [IntPtr]::Zero) { break }
            Start-Sleep -Milliseconds 200
        }
        Check ((Find-ByClass $proc.Id "#32770") -eq [IntPtr]::Zero) "A2 取消后文件对话框关闭"
    }
    Check (-not $proc.HasExited) "A3 进程存活（无闪退）"
} finally { if (-not $proc.HasExited) { $proc.Kill() } }

# ---------- B. CLI 加载 .out -> 树填充 ----------
$proc = Start-Process -FilePath $ExePath -ArgumentList $OutFile -PassThru
try {
    $main = [IntPtr]::Zero
    for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
        $main = Find-ByClass $proc.Id "OpenScopeMain"
        if ($main -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Start-Sleep -Milliseconds 1200
    $tree = Get-Tree $main
    $c1 = TreeCount $tree
    Write-Output ("B .out 树数量 = " + $c1)
    Check ($c1 -gt 0) "B1 命令行加载 .out 后变量树已填充"
} finally { if (-not $proc.HasExited) { $proc.Kill() } }

# ---------- C. 连续加载 -> 树被替换（fill_tree 清树） ----------
$proc = Start-Process -FilePath $ExePath -ArgumentList @($OutFile, $ElfFile) -PassThru
try {
    $main = [IntPtr]::Zero
    for ($i = 0; $i -lt 50 -and -not $proc.HasExited; $i++) {
        $main = Find-ByClass $proc.Id "OpenScopeMain"
        if ($main -ne [IntPtr]::Zero) { break }
        Start-Sleep -Milliseconds 200
    }
    Start-Sleep -Milliseconds 1200
    $tree = Get-Tree $main
    $c2 = TreeCount $tree
    Write-Output ("C 连续加载后树数量 = " + $c2)
    Check ($c2 -gt 0) "C1 连续加载后树仍有内容"
    Check ($c2 -eq 4) "C2 树被替换为 elf_sample 的 4 个符号（fill_tree TVI_ROOT 清树生效）"
} finally { if (-not $proc.HasExited) { $proc.Kill() } }

Write-Output ("ALL " + $(if ($fails -eq 0) { "PASS" } else { "FAILURES: $fails" }))
exit $fails
