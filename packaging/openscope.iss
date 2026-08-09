; OpenScope 安装脚本
; 版本号与 code/src/version.rc 保持同步（make_setup.py 会校验一致性）。

[Setup]
AppId={{7D3E1A05-8B2C-4A9F-9D46-OpenScope-2026}
AppName=OpenScope
AppVersion=1.11.0.0
AppVerName=OpenScope 1.11.0
AppPublisher=OpenScope
AppCopyright=Copyright (C) 2026 OpenScope
DefaultDirName={autopf}\OpenScope
DefaultGroupName=OpenScope
DisableProgramGroupPage=yes
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
PrivilegesRequired=admin
OutputDir=..\dist
OutputBaseFilename=OpenScope-Setup-1.11.0
SetupIconFile=..\assets\openscope.ico
UninstallDisplayIcon={app}\OpenScope.exe
Compression=lzma2/max
SolidCompression=yes
WizardStyle=modern
VersionInfoVersion=1.11.0.0
VersionInfoCompany=OpenScope
VersionInfoDescription=OpenScope - MCU Variable Acquisition and Calibration
VersionInfoProductName=OpenScope
VersionInfoProductVersion=1.11.0.0
VersionInfoOriginalFileName=OpenScope-Setup-1.11.0.exe

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[InstallDelete]
; Bug14：清理遗留/陈旧动态模块。module_mgr 会加载 {app}\dll 下所有导出
; os_module_get 的 dll，安装器此前不清理，旧 scope.dll 等残留模块被加载
; 可能引发异常/闪退。安装前整目录删除，随后由 [Files] 重新装 jlink.dll + JLink_x64.dll。
Type: filesandordirs; Name: "{app}\dll"
Type: filesandordirs; Name: "{app}\modules"

[UninstallDelete]
Type: filesandordirs; Name: "{app}\dll"
Type: filesandordirs; Name: "{app}\modules"

[Files]
; 主程序 + 动态模块（安装布局：exe 同目录 dll\，module_mgr 优先此布局）
Source: "..\bin\Release\OpenScope.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\dll\jlink.dll"; DestDir: "{app}\dll"; Flags: ignoreversion
Source: "..\dll\JLink_x64.dll"; DestDir: "{app}\dll"; Flags: ignoreversion

[Icons]
Name: "{group}\OpenScope"; Filename: "{app}\OpenScope.exe"
Name: "{group}\Uninstall OpenScope"; Filename: "{uninstallexe}"
Name: "{autodesktop}\OpenScope"; Filename: "{app}\OpenScope.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\OpenScope.exe"; Description: "{cm:LaunchProgram,OpenScope}"; Flags: nowait postinstall skipifsilent
