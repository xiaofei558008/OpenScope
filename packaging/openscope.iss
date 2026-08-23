; OpenScope 安装脚本
; 版本号与 code/src/version.rc 保持同步（make_setup.py 会校验一致性）。

[Setup]
AppId={{7D3E1A05-8B2C-4A9F-9D46-OpenScope-2026}
AppName=OpenScope
AppVersion=1.22.2.0
AppVerName=OpenScope 1.22.2
AppPublisher=OpenScope
AppCopyright=Copyright (C) 2026 OpenScope
DefaultDirName={autopf}\OpenScope
DefaultGroupName=OpenScope
DisableProgramGroupPage=yes
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
PrivilegesRequired=admin
OutputDir=..\dist
OutputBaseFilename=OpenScope-Setup-1.22.2
SetupIconFile=..\assets\openscope.ico
WizardImageFile=wizard_sidebar.bmp
UninstallDisplayIcon={app}\OpenScope.exe
Compression=lzma2/max
SolidCompression=yes
WizardStyle=modern
VersionInfoVersion=1.22.2.0
VersionInfoCompany=OpenScope
VersionInfoDescription=OpenScope - MCU Variable Acquisition and Calibration
VersionInfoProductName=OpenScope
VersionInfoProductVersion=1.22.2.0
VersionInfoOriginalFileName=OpenScope-Setup-1.22.2.exe

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
Source: "..\dll\stlink.dll"; DestDir: "{app}\dll"; Flags: ignoreversion
Source: "..\dll\network.dll"; DestDir: "{app}\dll"; Flags: ignoreversion
Source: "..\dll\JLink_x64.dll"; DestDir: "{app}\dll"; Flags: ignoreversion
; ST-Link 自含运行时：CubeProgrammer_API.dll + 依赖 DLL + FlashLoader（目标机无需安装 STM32CubeProgrammer）
Source: "..\dll\stlink\*"; DestDir: "{app}\dll\stlink"; Flags: ignoreversion recursesubdirs createallsubdirs
; 左侧广告栏图片（make_setup.py 从 icon\iso*.bmp 生成，[Code] 运行时按页面加载，不安装到目标机）
Source: "wizard_sidebar.bmp"; Flags: dontcopy
Source: "wizard_sidebar2.bmp"; Flags: dontcopy
Source: "wizard_sidebar3.bmp"; Flags: dontcopy

[Icons]
Name: "{group}\OpenScope"; Filename: "{app}\OpenScope.exe"
Name: "{group}\Uninstall OpenScope"; Filename: "{uninstallexe}"
Name: "{autodesktop}\OpenScope"; Filename: "{app}\OpenScope.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\OpenScope.exe"; Description: "{cm:LaunchProgram,OpenScope}"; Flags: nowait postinstall skipifsilent

[Code]
{ 左侧广告栏：每个安装页左侧按步骤展示一张产品图（164x314 横幅，图片居中缩放）：
    欢迎/准备安装 = iso1，目录/安装中 = iso2，任务 = iso3（完成页由嵌入的 iso1 显示）。
  图片由 make_setup.py 从 icon\iso*.bmp 生成三张 wizard_sidebarN.bmp，[Files] dontcopy
  打进安装器后运行时解出；欢迎/完成页由 [Setup] WizardImageFile 编译期嵌入渲染
  （Inno 官方机制），内部页用窗口化 STATIC 位图覆盖层（窗体级非窗口化图片控件
  在内部页不绘制），换页用 STM_SETIMAGE 切换。
  内部页容器 InnerPage 默认铺满整个客户区（modern 风格）会盖住左栏：
  CurPageChanged 每次换页按客户区坐标校准容器与当前页位置（引擎会重置）。}
type
  WinRect = record
    Left: Integer;
    Top: Integer;
    Right: Integer;
    Bottom: Integer;
  end;

var
  SidebarWnd: HWND;
  SidebarHbm1: THandle;
  SidebarHbm2: THandle;
  SidebarHbm3: THandle;
  SidebarTextCn: TNewStaticText;
  SidebarTextUrl: TNewStaticText;
  SidebarCopyright: TNewStaticText;
  SidebarGithub: TNewStaticText;

function SetWindowPos(hWnd: HWND; hWndInsertAfter: HWND; X, Y, cx, cy: Integer;
  uFlags: Cardinal): BOOL; external 'SetWindowPos@user32.dll stdcall';
function GetWindowRect(hWnd: HWND; var R: WinRect): BOOL; external 'GetWindowRect@user32.dll stdcall';
function CreateWindowEx(dwExStyle: DWORD; lpClassName, lpWindowName: string;
  dwStyle: DWORD; X, Y, nWidth, nHeight: Integer; hWndParent: HWND; hMenu: THandle;
  hInstance: THandle; lpParam: LongInt): HWND; external 'CreateWindowExW@user32.dll stdcall';
function LoadImage(hInst: THandle; name: string; typ: Cardinal; cx, cy: Integer;
  fuLoad: Cardinal): THandle; external 'LoadImageW@user32.dll stdcall';
function SendMessageW(hWnd: HWND; Msg: Cardinal; wParam: LongInt; lParam: LongInt): LongInt;
  external 'SendMessageW@user32.dll stdcall';

procedure InitializeWizard;
begin
  { 欢迎/完成页的图片由 [Setup] WizardImageFile 编译期嵌入（官方机制）。
    内部页用窗口化 STATIC 位图覆盖层绘制左侧栏图片（窗体级非窗口化图片控件
    在内部页不绘制）；右上角小图标隐藏 }
  ExtractTemporaryFile('wizard_sidebar.bmp');
  ExtractTemporaryFile('wizard_sidebar2.bmp');
  ExtractTemporaryFile('wizard_sidebar3.bmp');
  WizardForm.WizardSmallBitmapImage.Visible := False;

  { 三张产品位图一次加载并缩放到图片栏控件尺寸（横幅内照片已上下左右居中，
    SS_BITMAP 不拉伸，由 LoadImage 的 cx/cy 缩放实现满栏居中显示）；
    换页用 STM_SETIMAGE 切换 }
  SidebarHbm1 := LoadImage(0, ExpandConstant('{tmp}\wizard_sidebar.bmp'), 0,
    WizardForm.WizardBitmapImage.Width, WizardForm.WizardBitmapImage.Height, $10);
  SidebarHbm2 := LoadImage(0, ExpandConstant('{tmp}\wizard_sidebar2.bmp'), 0,
    WizardForm.WizardBitmapImage.Width, WizardForm.WizardBitmapImage.Height, $10);
  SidebarHbm3 := LoadImage(0, ExpandConstant('{tmp}\wizard_sidebar3.bmp'), 0,
    WizardForm.WizardBitmapImage.Width, WizardForm.WizardBitmapImage.Height, $10);
  SidebarWnd := CreateWindowEx(0, 'STATIC', '',
    $40000000 or $10000000 or $0E,             { WS_CHILD | WS_VISIBLE | SS_BITMAP }
    WizardForm.WizardBitmapImage.Left, WizardForm.WizardBitmapImage.Top,
    WizardForm.WizardBitmapImage.Width, WizardForm.WizardBitmapImage.Height,
    WizardForm.Handle, 0, 0, 0);
  SendMessageW(SidebarWnd, $0172, 0, SidebarHbm1);   { STM_SETIMAGE, IMAGE_BITMAP }

  { 内部页容器解除对齐（否则属性赋值会被布局引擎重置）并缩到广告栏右侧 }
  WizardForm.InnerPage.Align := alNone;
  WizardForm.InnerPage.Left := WizardForm.WizardBitmapImage.Width;
  WizardForm.InnerPage.Width := WizardForm.ClientWidth - WizardForm.InnerPage.Left;

  { 中文宣传语：苹果风格（PingFang 在 Windows 无内置，微软雅黑最接近） }
  SidebarTextCn := TNewStaticText.Create(WizardForm);
  SidebarTextCn.Parent := WizardForm;
  SidebarTextCn.Left := 6;
  SidebarTextCn.Top := WizardForm.WizardBitmapImage.Top + WizardForm.WizardBitmapImage.Height + 12;
  SidebarTextCn.Width := 152;
  SidebarTextCn.Height := 22;
  SidebarTextCn.Caption := '晶圆上的生物提供技术支持';
  SidebarTextCn.Color := clWindow;
  SidebarTextCn.Font.Name := 'Microsoft YaHei';
  SidebarTextCn.Font.Size := 10;
  SidebarTextCn.Font.Color := $3C3C3C;

  { 网址：console 字体 Consolas }
  SidebarTextUrl := TNewStaticText.Create(WizardForm);
  SidebarTextUrl.Parent := WizardForm;
  SidebarTextUrl.Left := 6;
  SidebarTextUrl.Top := SidebarTextCn.Top + SidebarTextCn.Height + 6;
  SidebarTextUrl.Width := 152;
  SidebarTextUrl.Height := 18;
  SidebarTextUrl.Caption := 'www.opendebugger.com';
  SidebarTextUrl.Color := clWindow;
  SidebarTextUrl.Font.Name := 'Consolas';
  SidebarTextUrl.Font.Size := 9;
  SidebarTextUrl.Font.Color := $3C3C3C;

  { 版权说明（仅第一页显示） }
  SidebarCopyright := TNewStaticText.Create(WizardForm);
  SidebarCopyright.Parent := WizardForm;
  SidebarCopyright.Left := 6;
  SidebarCopyright.Top := SidebarTextUrl.Top + SidebarTextUrl.Height + 14;
  SidebarCopyright.Width := 152;
  SidebarCopyright.Height := 20;
  SidebarCopyright.Caption := 'Copyright (C) 2026 OpenScope';
  SidebarCopyright.Color := clWindow;
  SidebarCopyright.Font.Name := 'Microsoft YaHei';
  SidebarCopyright.Font.Size := 8;
  SidebarCopyright.Font.Color := $3C3C3C;
  SidebarCopyright.Visible := False;

  { GitHub 仓库地址（仅第一页显示） }
  SidebarGithub := TNewStaticText.Create(WizardForm);
  SidebarGithub.Parent := WizardForm;
  SidebarGithub.Left := 6;
  SidebarGithub.Top := SidebarCopyright.Top + SidebarCopyright.Height + 2;
  SidebarGithub.Width := 152;
  SidebarGithub.Height := 28;
  SidebarGithub.Caption := 'github.com/xiaofei558008/OpenScope';
  SidebarGithub.WordWrap := True;
  SidebarGithub.Color := clWindow;
  SidebarGithub.Font.Name := 'Consolas';
  SidebarGithub.Font.Size := 7;
  SidebarGithub.Font.Color := $3C3C3C;
  SidebarGithub.Visible := False;
end;

procedure CurPageChanged(CurPageID: Integer);
var
  R: WinRect;
  Shift: Integer;
  PageW: Integer;
begin
  { 引擎每次换页把 InnerPage 重置回客户区 (0,0)；SetWindowPos 对子窗口使用客户区
    坐标，直接校准到 [侧栏宽, 客户区宽]，幂等且无需状态 }
  Shift := WizardForm.WizardBitmapImage.Width;
  GetWindowRect(WizardForm.InnerPage.Handle, R);
  SetWindowPos(WizardForm.InnerPage.Handle, 0,
    Shift, 0,
    WizardForm.ClientWidth - Shift,
    R.Bottom - R.Top, $4 or $10);
  { 部分页面（欢迎/完成/准备安装等）在容器移位前完成布局，仍占满整个客户区，
    会盖住左侧图片栏：按页面 ID 手动把当前页移到容器客户区原点（幂等）。
    页面是容器的子窗口，坐标为容器客户区坐标。 }
  PageW := WizardForm.ClientWidth - Shift;
  if CurPageID = wpWelcome then
    SetWindowPos(WizardForm.WelcomePage.Handle, 0, 0, 0, PageW, R.Bottom - R.Top, $4 or $10);
  if CurPageID = wpSelectDir then
    SetWindowPos(WizardForm.SelectDirPage.Handle, 0, 0, 0, PageW, R.Bottom - R.Top, $4 or $10);
  if CurPageID = wpSelectTasks then
    SetWindowPos(WizardForm.SelectTasksPage.Handle, 0, 0, 0, PageW, R.Bottom - R.Top, $4 or $10);
  if CurPageID = wpReady then
    SetWindowPos(WizardForm.ReadyPage.Handle, 0, 0, 0, PageW, R.Bottom - R.Top, $4 or $10);
  if CurPageID = wpPreparing then
    SetWindowPos(WizardForm.PreparingPage.Handle, 0, 0, 0, PageW, R.Bottom - R.Top, $4 or $10);
  if CurPageID = wpInstalling then
    SetWindowPos(WizardForm.InstallingPage.Handle, 0, 0, 0, PageW, R.Bottom - R.Top, $4 or $10);
  if CurPageID = wpFinished then
    SetWindowPos(WizardForm.FinishedPage.Handle, 0, 0, 0, PageW, R.Bottom - R.Top, $4 or $10);
  { 默认向导位图在内部页会被隐藏，强制在所有页面显示 }
  WizardForm.WizardBitmapImage.Visible := True;
  { 三个步骤各一张产品图：欢迎/准备安装=iso1，目录/安装中=iso2，任务=iso3，
    完成页由嵌入的 iso1 显示（覆盖层同步切到 iso1，保持一致） }
  if CurPageID = wpSelectDir then
    SendMessageW(SidebarWnd, $0172, 0, SidebarHbm2)
  else if CurPageID = wpSelectTasks then
    SendMessageW(SidebarWnd, $0172, 0, SidebarHbm3)
  else if (CurPageID = wpPreparing) or (CurPageID = wpInstalling) then
    SendMessageW(SidebarWnd, $0172, 0, SidebarHbm2)
  else
    SendMessageW(SidebarWnd, $0172, 0, SidebarHbm1);
  { 版权说明与 GitHub 仓库地址仅第一页（欢迎页）显示 }
  SidebarCopyright.Visible := (CurPageID = wpWelcome);
  SidebarGithub.Visible := (CurPageID = wpWelcome);
end;

