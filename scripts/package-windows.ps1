param(
    [string]$BuildDir = "build-win",
    [string]$OutputDir = "dist/sj-sim-windows-x64"
)

$ErrorActionPreference = "Stop"
$repoRoot = Split-Path -Parent $PSScriptRoot
$buildPath = [System.IO.Path]::GetFullPath((Join-Path $repoRoot $BuildDir))
$outputPath = [System.IO.Path]::GetFullPath((Join-Path $repoRoot $OutputDir))
$distRoot = [System.IO.Path]::GetFullPath((Join-Path $repoRoot "dist"))
$executable = Join-Path $buildPath "release/sj-sim.exe"

if (-not (Test-Path -LiteralPath $executable)) {
    $executable = Join-Path $buildPath "sj-sim.exe"
}
if (-not (Test-Path -LiteralPath $executable)) {
    throw "sj-sim.exe was not found under $buildPath"
}
if (-not $outputPath.StartsWith($distRoot + [System.IO.Path]::DirectorySeparatorChar, [System.StringComparison]::OrdinalIgnoreCase)) {
    throw "OutputDir must name a child directory of $distRoot"
}

if (Test-Path -LiteralPath $outputPath) {
    Remove-Item -Recurse -Force -LiteralPath $outputPath
}
New-Item -ItemType Directory -Force -Path $outputPath | Out-Null
Copy-Item -LiteralPath $executable -Destination (Join-Path $outputPath "sj-sim.exe") -Force

& windeployqt --release --compiler-runtime --no-translations --dir $outputPath (Join-Path $outputPath "sj-sim.exe")
if ($LASTEXITCODE -ne 0) { throw "windeployqt failed with exit code $LASTEXITCODE" }

foreach ($folder in @("flags", "userData")) {
    $source = Join-Path $repoRoot $folder
    if (-not (Test-Path -LiteralPath $source)) { throw "Required release folder is missing: $source" }
    Copy-Item -Recurse -Force -LiteralPath $source -Destination (Join-Path $outputPath $folder)
}

$translationDir = Join-Path $outputPath "translations"
New-Item -ItemType Directory -Force -Path $translationDir | Out-Null
foreach ($language in @('en', 'cs')) {
    & lrelease -nounfinished (Join-Path $repoRoot "translations/translation_$language.ts") -qm (Join-Path $translationDir "translation_$language.qm")
    if ($LASTEXITCODE -ne 0) { throw "lrelease failed with exit code $LASTEXITCODE" }
}
$qtTranslations = & qmake -query QT_INSTALL_TRANSLATIONS
if ($LASTEXITCODE -ne 0) { throw "qmake failed with exit code $LASTEXITCODE" }
foreach ($language in @('pl', 'cs')) {
    Copy-Item -LiteralPath (Join-Path $qtTranslations "qtbase_$language.qm") -Destination $translationDir
}

Write-Host "Packaged Windows application at $outputPath"
