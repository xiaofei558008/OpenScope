; OpenScope 安装脚本
; 版本号与 code/src/version.rc 保持同步（make_setup.py 会校验一致性）。

[Setup]
AppId={{7D3E1A05-8B2C-4A9F-9D46-OpenScope-2026}
AppName=OpenScope
AppVersion=1.17.2.0
AppVerName=OpenScope 1.17.2
AppPublisher=OpenScope
AppCopyright=Copyright (C) 2026 OpenScope
DefaultDirName={autopf}\OpenScope
DefaultGroupName=OpenScope
DisableProgramGroupPage=yes
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
PrivilegesRequired=admin
OutputDir=..\dist
OutputBaseFilename=OpenScope-Setup-1.17.2
SetupIconFile=..\assets\openscope.ico
WizardImageFile=wizard_sidebar.bmp
UninstallDisplayIcon={app}\OpenScope.exe
Compression=lzma2/max
SolidCompression=yes
WizardStyle=modern
VersionInfoVersion=1.17.2.0
VersionInfoCompany=OpenScope
VersionInfoDescription=OpenScope - MCU Variable Acquisition and Calibration
VersionInfoProductName=OpenScope
VersionInfoProductVersion=1.17.2.0
VersionInfoOriginalFileName=OpenScope-Setup-1.17.2.exe

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
; 左侧广告栏图片（make_setup.py 从 icon\isolator.jpg 生成，[Code] 运行时加载，不安装到目标机）
Source: "wizard_sidebar.bmp"; Flags: dontcopy

[Icons]
Name: "{group}\OpenScope"; Filename: "{app}\OpenScope.exe"
Name: "{group}\Uninstall OpenScope"; Filename: "{uninstallexe}"
Name: "{autodesktop}\OpenScope"; Filename: "{app}\OpenScope.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\OpenScope.exe"; Description: "{cm:LaunchProgram,OpenScope}"; Flags: nowait postinstall skipifsilent

[Code]
{ 左侧广告栏：每个安装页左侧显示 icon\isolator.bmp/jpg 裁剪图（164x314）+ 两行宣传文字。
  图片由 make_setup.py 每次打包重新生成 wizard_sidebar.bmp，经 [Files] dontcopy 打进
  安装器，运行时 ExtractTemporaryFile 解出后加载进默认向导位图控件
  （Inno 自己的渲染路径，欢迎/完成页原生支持，内部页由 CurPageChanged 强制显示）。
  内部页容器 InnerPage 默认铺满整个客户区（modern 风格）会盖住图片：
  InitializeWizard 解除其对齐，CurPageChanged 每次换页按客户区坐标校准其位置
  （引擎每次换页会把它重置回客户区原点）。}
type
  WinRect = record
    Left: Integer;
    Top: Integer;
    Right: Integer;
    Bottom: Integer;
  end;

var
  SidebarImage: TBitmapImage;
  SidebarTextCn: TNewStaticText;
  SidebarTextUrl: TNewStaticText;

function SetWindowPos(hWnd: HWND; hWndInsertAfter: HWND; X, Y, cx, cy: Integer;
  uFlags: Cardinal): BOOL; external 'SetWindowPos@user32.dll stdcall';
function GetWindowRect(hWnd: HWND; var R: WinRect): BOOL; external 'GetWindowRect@user32.dll stdcall';

procedure InitializeWizard;
var
  BmpPath: string;
begin
  { 欢迎/完成页的图片由 [Setup] WizardImageFile 编译期嵌入（官方机制）。
    内部页用自定义 TBitmapImage 覆盖左侧栏；右上角小图标隐藏 }
  ExtractTemporaryFile('wizard_sidebar.bmp');
  BmpPath := ExpandConstant('{tmp}\wizard_sidebar.bmp');
  WizardForm.WizardSmallBitmapImage.Visible := False;

  SidebarImage := TBitmapImage.Create(WizardForm);
  SidebarImage.Parent := WizardForm;
  SidebarImage.Left := WizardForm.WizardBitmapImage.Left;
  SidebarImage.Top := WizardForm.WizardBitmapImage.Top;
  SidebarImage.Width := WizardForm.WizardBitmapImage.Width;
  SidebarImage.Height := WizardForm.WizardBitmapImage.Height;
  SidebarImage.Stretch := True;
  SidebarImage.Bitmap.LoadFromFile(BmpPath);

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
end;

procedure CurPageChanged(CurPageID: Integer);
var
  R: WinRect;
begin
  { 引擎每次换页把 InnerPage 重置回客户区 (0,0)；SetWindowPos 对子窗口使用客户区
    坐标，直接校准到 [侧栏宽, 客户区宽]，幂等且无需状态 }
  GetWindowRect(WizardForm.InnerPage.Handle, R);
  SetWindowPos(WizardForm.InnerPage.Handle, 0,
    WizardForm.WizardBitmapImage.Width, 0,
    WizardForm.ClientWidth - WizardForm.WizardBitmapImage.Width,
    R.Bottom - R.Top, $4 or $10);
  { 默认向导位图在内部页会被隐藏，强制在所有页面显示 }
  WizardForm.WizardBitmapImage.Visible := True;
  { 欢迎/完成页文字被容器连带右移，左移回原位（页面激活后原生坐标才就绪） }
  if CurPageID = wpWelcome then
  begin
    WizardForm.WelcomeLabel1.Left := WizardForm.WelcomeLabel1.Left - WizardForm.WizardBitmapImage.Width;
    WizardForm.WelcomeLabel2.Left := WizardForm.WelcomeLabel2.Left - WizardForm.WizardBitmapImage.Width;
  end;
  if CurPageID = wpFinished then
  begin
    WizardForm.FinishedHeadingLabel.Left := WizardForm.FinishedHeadingLabel.Left - WizardForm.WizardBitmapImage.Width;
    WizardForm.FinishedLabel.Left := WizardForm.FinishedLabel.Left - WizardForm.WizardBitmapImage.Width;
  end;
end;

