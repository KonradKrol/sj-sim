#ifndef StageDir
  #error StageDir must point to the deployed Qt application.
#endif
#ifndef AppVersion
  #define AppVersion "2.0.0"
#endif
#ifndef NumericVersion
  #define NumericVersion AppVersion
#endif
#ifndef RepoRoot
  #define RepoRoot "..\.."
#endif
#ifndef OutputDir
  #define OutputDir "..\..\dist"
#endif
#ifndef HasVCRedist
  #define HasVCRedist 0
  #define VCRuntimeMS 0
  #define VCRuntimeLS 0
#endif

[Setup]
AppId=SjSim.Desktop
AppName=Sj.Sim
AppVersion={#AppVersion}
AppVerName=Sj.Sim {#AppVersion}
VersionInfoVersion={#NumericVersion}
AppPublisher=Konrad Król
AppPublisherURL=https://github.com/KonradKrol/sj-sim
DefaultDirName={autopf}\Sj.Sim
DefaultGroupName=Sj.Sim
DisableProgramGroupPage=yes
PrivilegesRequired=lowest
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
MinVersion=10.0
OutputDir={#OutputDir}
OutputBaseFilename=SjSim-{#AppVersion}-windows-x64-Setup
SetupIconFile={#RepoRoot}\sj-sim.ico
UninstallDisplayIcon={app}\sj-sim.exe
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
CloseApplications=yes
RestartApplications=no
ChangesAssociations=yes

[Languages]
Name: "polish"; MessagesFile: "compiler:Languages\Polish.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "czech"; MessagesFile: "compiler:Languages\Czech.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"

[Files]
Source: "{#StageDir}\*"; DestDir: "{app}"; Excludes: "vc_redist*.exe"; Flags: ignoreversion recursesubdirs createallsubdirs
#if HasVCRedist
Source: "{#StageDir}\vc_redist.x64.exe"; Flags: dontcopy
#endif

[Icons]
Name: "{autoprograms}\Sj.Sim"; Filename: "{app}\sj-sim.exe"; WorkingDir: "{app}"
Name: "{autodesktop}\Sj.Sim"; Filename: "{app}\sj-sim.exe"; WorkingDir: "{app}"; Tasks: desktopicon

[Registry]
Root: HKCU; Subkey: "Software\Classes\.sjsim-pack\OpenWithProgids"; ValueType: none; ValueName: "SjSim.Pack"; Flags: uninsdeletevalue
Root: HKCU; Subkey: "Software\Classes\SjSim.Pack"; ValueType: string; ValueData: "Sj.Sim resource pack"; Flags: uninsdeletekey
Root: HKCU; Subkey: "Software\Classes\SjSim.Pack\DefaultIcon"; ValueType: string; ValueData: "{app}\sj-sim.exe,0"
Root: HKCU; Subkey: "Software\Classes\SjSim.Pack\shell\open\command"; ValueType: string; ValueData: """{app}\sj-sim.exe"" ""%1"""

[Run]
Filename: "{app}\sj-sim.exe"; Description: "{cm:LaunchProgram,Sj.Sim}"; Flags: nowait postinstall skipifsilent

[Code]
#if HasVCRedist
function NeedsVCRuntime: Boolean;
var
  MS, LS: Cardinal;
begin
  Result := True;
  if FileExists(ExpandConstant('{sys}\msvcp140.dll')) and
     GetVersionNumbers(ExpandConstant('{sys}\vcruntime140.dll'), MS, LS) then
    Result := (MS < {#VCRuntimeMS}) or ((MS = {#VCRuntimeMS}) and (LS < {#VCRuntimeLS}));
end;

function PrepareToInstall(var NeedsRestart: Boolean): String;
var
  ExitCode: Integer;
begin
  Result := '';
  if not NeedsVCRuntime then Exit;
  ExtractTemporaryFile('vc_redist.x64.exe');
  if not Exec(ExpandConstant('{tmp}\vc_redist.x64.exe'), '/install /quiet /norestart', '', SW_HIDE, ewWaitUntilTerminated, ExitCode) then
    Result := 'Nie udało się uruchomić instalatora Microsoft Visual C++ Runtime.'
  else if ExitCode = 3010 then NeedsRestart := True
  else if (ExitCode <> 0) and (ExitCode <> 1638) then
    Result := Format('Microsoft Visual C++ Runtime: błąd %d. Zainstaluj składnik i ponów instalację Sj.Sima.', [ExitCode]);
end;
#endif
