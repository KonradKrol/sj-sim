param([string]$BuildDirectory = 'build-simulation-v2-tests-windows')
$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
if (-not (Get-Command cl.exe -ErrorAction SilentlyContinue)) {
    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
    $vsRoot = & $vswhere -latest -products '*' -version '[17.0,18.0)' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if (-not $vsRoot) { throw 'Visual Studio 2022 C++ Build Tools are required.' }
    & (Join-Path $vsRoot 'Common7/Tools/Launch-VsDevShell.ps1') -Arch amd64 -HostArch amd64 -SkipAutomaticLocation
}
$buildRoot = Join-Path $repoRoot $BuildDirectory
$cases = [ordered]@{
    random = @('RandomSource')
    quality = @('RandomSource', 'HillSkillCurve', 'QualityModel')
    weather = @('RandomSource', 'Weather', 'WeatherPresets')
    engine = @('RandomSource', 'HillSkillCurve', 'QualityModel', 'Weather', 'FlightModel', 'LandingModel', 'Scoring', 'Manipulation', 'Engine')
}
foreach ($entry in $cases.GetEnumerator()) {
    $caseDirectory = Join-Path $buildRoot $entry.Key
    New-Item -ItemType Directory -Force -Path $caseDirectory | Out-Null
    $executable = Join-Path $caseDirectory ('simulation-v2-' + $entry.Key + '.exe')
    $sources = @($entry.Value | ForEach-Object { Join-Path $repoRoot "source/simulation-v2/$_.cpp" })
    $sources += Join-Path $repoRoot "tests/simulation-v2-$($entry.Key).cpp"
    $compilerArguments = @('/nologo', '/std:c++17', '/Zc:__cplusplus', '/permissive-', '/utf-8', '/O2', '/EHsc', '/MD', '/W3', "/Fo$caseDirectory/", "/Fe$executable")
    & cl.exe @compilerArguments @sources
    if ($LASTEXITCODE -ne 0) { throw "Core build failed: $($entry.Key)" }
    & $executable
    if ($LASTEXITCODE -ne 0) { throw "Core test failed: $($entry.Key)" }
    Write-Output "PASS: simulation-v2-$($entry.Key) (MSVC)"
}
