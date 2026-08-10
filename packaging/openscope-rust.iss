; OpenScope v2.0.0（Rust 重写版）安装脚本
; 与 C 版共用 AppId 前缀，但版本独立 2.0.0（request.md 需求 12：Rust 重写从 2.0.0 起）。
; 源文件：rust/target/release/openscope-app.exe（构建后安装为 OpenScope.exe）。

[Setup]
AppId={{7D3E1A05-8B2C-4A9F-9D46-OpenScope-2026}
AppName=OpenScope
AppVersion=2.0.0.0
AppVerName=OpenScope 2.0.0
AppPublisher=OpenScope
AppCopyright=Copyright (C) 2026 OpenScope
DefaultDirName={autopf}\OpenScope
DefaultGroupName=OpenScope
DisableProgramGroupPage=yes
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
PrivilegesRequired=admin
OutputDir=..\dist
OutputBaseFilename=OpenScope-Setup-2.0.0
SetupIconFile=..\assets\openscope.ico
UninstallDisplayIcon={app}\OpenScope.exe
Compression=lzma2/max
SolidCompression=yes
WizardStyle=modern
VersionInfoVersion=2.0.0.0
VersionInfoCompany=OpenScope
VersionInfoDescription=OpenScope - MCU Variable Acquisition and Calibration
VersionInfoProductName=OpenScope
VersionInfoProductVersion=2.0.0.0
VersionInfoOriginalFileName=OpenScope-Setup-2.0.0.exe

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[InstallDelete]
; Rust 版无动态模块，仍清理旧安装残留（C 版 dll/ 模块）。
Type: filesandordirs; Name: "{app}\dll"
Type: filesandordirs; Name: "{app}\modules"

[UninstallDelete]
Type: filesandordirs; Name: "{app}\dll"
Type: filesandordirs; Name: "{app}\modules"

[Files]
; Rust 重写主程序（安装为 OpenScope.exe）+ J-Link DLL（Rust 版 find_jlink_dll 优先 exe 同目录）
Source: "..\rust\target\release\openscope-app.exe"; DestDir: "{app}"; DestName: "OpenScope.exe"; Flags: ignoreversion
Source: "..\dll\JLink_x64.dll"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\OpenScope"; Filename: "{app}\OpenScope.exe"
Name: "{group}\Uninstall OpenScope"; Filename: "{uninstallexe}"
Name: "{autodesktop}\OpenScope"; Filename: "{app}\OpenScope.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\OpenScope.exe"; Description: "{cm:LaunchProgram,OpenScope}"; Flags: nowait postinstall skipifsilent
