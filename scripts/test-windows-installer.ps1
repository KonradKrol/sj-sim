param([Parameter(Mandatory=$true)][string]$Installer)
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
if ($env:OS -ne 'Windows_NT') { throw 'Run the installer smoke test on Windows' }
foreach ($key in @('HKCU:\Software\Microsoft\Windows\CurrentVersion\Uninstall\SjSim.Desktop_is1','HKLM:\Software\Microsoft\Windows\CurrentVersion\Uninstall\SjSim.Desktop_is1')) {
    if (Test-Path -LiteralPath $key) { throw 'Sj.Sim is already installed. Run this test on a fresh Windows account or VM.' }
}
$installerPath = (Resolve-Path -LiteralPath $Installer).Path
$workspace = Join-Path ([IO.Path]::GetTempPath()) ('SjSim-installer-test-' + [guid]::NewGuid().ToString('N'))
$application = Join-Path $workspace 'Application'
function Run-And-Wait([string]$File,[string]$Arguments) {
    $process = [Diagnostics.Process]::new()
    $process.StartInfo.FileName = $File; $process.StartInfo.Arguments = $Arguments
    $process.StartInfo.UseShellExecute = $false
    foreach ($key in @('QML2_IMPORT_PATH','QML_IMPORT_PATH','QT_PLUGIN_PATH','QT_QPA_PLATFORM_PLUGIN_PATH','QTDIR','QT_ROOT_DIR')) { $process.StartInfo.EnvironmentVariables.Remove($key) }
    $process.StartInfo.EnvironmentVariables['PATH'] = "$env:SystemRoot\System32;$env:SystemRoot"
    [void]$process.Start()
    if (-not $process.WaitForExit(180000)) { $process.Kill(); throw "Timed out: $File" }
    if ($process.ExitCode -notin @(0,3010)) { throw "Exit $($process.ExitCode): $File" }
}
try {
    New-Item -ItemType Directory -Force -Path $workspace | Out-Null
    Run-And-Wait $installerPath "/VERYSILENT /SUPPRESSMSGBOXES /NORESTART /DIR=`"$application`" /LOG=`"$workspace/install.log`""
    $env:QT_QPA_PLATFORM = 'offscreen'; $env:QT_QUICK_BACKEND = 'software'; $env:QT_FORCE_STDERR_LOGGING = '1'
    Run-And-Wait (Join-Path $application 'sj-sim.exe') '--deployment-smoke'
    $marker = Join-Path $application 'user-created-marker.txt'
    [IO.File]::WriteAllText($marker,'preserve user files')
    Run-And-Wait $installerPath "/VERYSILENT /SUPPRESSMSGBOXES /NORESTART /DIR=`"$application`""
    if ((Get-Content -Raw -LiteralPath $marker) -ne 'preserve user files') { throw 'Update changed user-created data' }
    Run-And-Wait (Join-Path $application 'unins000.exe') '/VERYSILENT /SUPPRESSMSGBOXES /NORESTART'
    if ((Get-Content -Raw -LiteralPath $marker) -ne 'preserve user files') { throw 'Uninstall removed user-created data' }
    Write-Host 'PASS installer: install, deployed Qt UI, upgrade and uninstall preserve user-created files'
} finally {
    $uninstaller = Join-Path $application 'unins000.exe'
    if (Test-Path -LiteralPath $uninstaller) { try { Run-And-Wait $uninstaller '/VERYSILENT /SUPPRESSMSGBOXES /NORESTART' } catch { Write-Warning $_ } }
    if (Test-Path -LiteralPath $workspace) { Remove-Item -Recurse -Force -LiteralPath $workspace }
}
