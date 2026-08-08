; OpenScope 安装脚本
; 版本号与 code/src/version.rc 保持同步（make_setup.py 会校验一致性）。

[Setup]
AppId={{7D3E1A05-8B2C-4A9F-9D46-OpenScope-2026}
AppName=OpenScope
AppVersion=1.4.0.0
AppVerName=OpenScope 1.4.0
AppPublisher=OpenScope
AppCopyright=Copyright (C) 2026 OpenScope
DefaultDirName={autopf}\OpenScope
DefaultGroupName=OpenScope
DisableProgramGroupPage=yes
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
PrivilegesRequired=admin
OutputDir=..\dist
OutputBaseFilename=OpenScope-Setup-1.4.0
SetupIconFile=..\assets\openscope.ico
UninstallDisplayIcon={app}\OpenScope.exe
Compression=lzma2/max
SolidCompression=yes
WizardStyle=modern
VersionInfoVersion=1.4.0.0
VersionInfoCompany=OpenScope
VersionInfoDescription=OpenScope - MCU Variable Acquisition and Calibration
VersionInfoProductName=OpenScope
VersionInfoProductVersion=1.4.0.0
VersionInfoOriginalFileName=OpenScope-Setup-1.4.0.exe

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
; 主程序 + 动态模块（安装布局：exe 同目录 dll\，module_mgr 优先此布局）
Source: "..\bin\Release\OpenScope.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\dll\jlink.dll"; DestDir: "{app}\dll"; Flags: ignoreversion
Source: "..\dll\scope.dll"; DestDir: "{app}\dll"; Flags: ignoreversion
Source: "..\dll\JLink_x64.dll"; DestDir: "{app}\dll"; Flags: ignoreversion

[Icons]
Name: "{group}\OpenScope"; Filename: "{app}\OpenScope.exe"
Name: "{group}\Uninstall OpenScope"; Filename: "{uninstallexe}"
Name: "{autodesktop}\OpenScope"; Filename: "{app}\OpenScope.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\OpenScope.exe"; Description: "{cm:LaunchProgram,OpenScope}"; Flags: nowait postinstall skipifsilent
