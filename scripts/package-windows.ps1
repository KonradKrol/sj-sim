param(
    [string]$BuildDir = "build-win",
    [string]$OutputDir = "dist",
    [string]$InnoCompiler = "",
    [switch]$KeepBundle,
    [switch]$BundleOnly
)
Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"
$repoRoot = Split-Path -Parent $PSScriptRoot
$buildPath = [System.IO.Path]::GetFullPath((Join-Path $repoRoot $BuildDir))
$outputPath = [System.IO.Path]::GetFullPath((Join-Path $repoRoot $OutputDir))
$distRoot = [System.IO.Path]::GetFullPath((Join-Path $repoRoot "dist"))
if ($outputPath -ne $distRoot -and -not $outputPath.StartsWith($distRoot + [System.IO.Path]::DirectorySeparatorChar, [System.StringComparison]::OrdinalIgnoreCase)) { throw "OutputDir must be dist or a child directory of dist" }
$executable = Join-Path $buildPath "release/sj-sim.exe"
if (-not (Test-Path -LiteralPath $executable)) { $executable = Join-Path $buildPath "sj-sim.exe" }
if (-not (Test-Path -LiteralPath $executable)) { throw "sj-sim.exe was not found under $buildPath" }
foreach ($tool in @('windeployqt', 'qmake', 'lrelease')) { if (-not (Get-Command $tool -ErrorAction SilentlyContinue)) { throw "Missing build tool: $tool" } }
if (-not $BundleOnly) {
    if (-not $InnoCompiler) {
        $command = Get-Command ISCC.exe -ErrorAction SilentlyContinue
        if ($command) { $InnoCompiler = $command.Source }
    }
    if (-not $InnoCompiler) {
        foreach ($base in @(${env:ProgramFiles(x86)}, $env:ProgramFiles)) {
            if (-not $base) { continue }
            foreach ($edition in @('Inno Setup 7', 'Inno Setup 6')) {
                $candidate = Join-Path $base "$edition/ISCC.exe"
                if (Test-Path -LiteralPath $candidate) { $InnoCompiler = $candidate; break }
            }
            if ($InnoCompiler) { break }
        }
    }
    if (-not $InnoCompiler -or -not (Test-Path -LiteralPath $InnoCompiler)) { throw "Install Inno Setup 6.3+ (e.g. winget install --id JRSoftware.InnoSetup.7 -e), or pass -InnoCompiler. Use -BundleOnly only for deployment debugging." }
}
$versionSource = Get-Content -Raw -LiteralPath (Join-Path $repoRoot 'source/UI/mainwindow.cpp')
$match = [regex]::Match($versionSource, 'const QString appVersion\s*=\s*"([0-9]+\.[0-9]+\.[0-9]+(?:[-+][A-Za-z0-9.-]+)?)"')
if (-not $match.Success) { throw "Cannot determine application version" }
$packageVersion = $match.Groups[1].Value
$numericVersion = $packageVersion.Split('-')[0].Split('+')[0]
New-Item -ItemType Directory -Force -Path $outputPath | Out-Null
$workspace = Join-Path $distRoot ('.windows-package-' + [guid]::NewGuid().ToString('N'))
$stage = Join-Path $workspace 'Bundle'
$installerOutput = Join-Path $workspace 'Installer'
try {
    New-Item -ItemType Directory -Force -Path $stage, $installerOutput | Out-Null
    Copy-Item -LiteralPath $executable -Destination (Join-Path $stage 'sj-sim.exe')
    & windeployqt --release --compiler-runtime --no-translations --include-plugins qoffscreen --qmldir (Join-Path $repoRoot 'source/UI/Quick') --dir $stage (Join-Path $stage 'sj-sim.exe')
    if ($LASTEXITCODE -ne 0) { throw "windeployqt failed: $LASTEXITCODE" }
    Copy-Item -Recurse -LiteralPath (Join-Path $repoRoot 'flags') -Destination (Join-Path $stage 'flags')
    Copy-Item -Recurse -LiteralPath (Join-Path $repoRoot 'data/defaults') -Destination (Join-Path $stage 'Defaults')
    Copy-Item -LiteralPath (Join-Path $repoRoot 'LICENSE') -Destination (Join-Path $stage 'LICENSE.txt')
    $translationDir = Join-Path $stage 'translations'
    New-Item -ItemType Directory -Force -Path $translationDir | Out-Null
    foreach ($language in @('en', 'cs')) {
        & lrelease -nounfinished (Join-Path $repoRoot "translations/translation_$language.ts") -qm (Join-Path $translationDir "translation_$language.qm")
        if ($LASTEXITCODE -ne 0) { throw "lrelease failed: $LASTEXITCODE" }
    }
    $qtTranslations = & qmake -query QT_INSTALL_TRANSLATIONS
    if ($LASTEXITCODE -ne 0) { throw "qmake failed: $LASTEXITCODE" }
    foreach ($language in @('pl', 'cs')) { Copy-Item -LiteralPath (Join-Path $qtTranslations "qtbase_$language.qm") -Destination $translationDir }
    if (-not $BundleOnly) {
        $redist = Get-ChildItem -LiteralPath $stage -Recurse -Filter 'vc_redist.x64.exe' | Select-Object -First 1
        $hasRedist = 0; $runtimeMS = [uint32]0; $runtimeLS = [uint32]0
        if ($redist) {
            $rootRedist = Join-Path $stage 'vc_redist.x64.exe'
            if ($redist.FullName -ne $rootRedist) { Copy-Item -LiteralPath $redist.FullName -Destination $rootRedist }
            $info = (Get-Item -LiteralPath $rootRedist).VersionInfo
            $runtimeMS = [uint32](([uint32]$info.FileMajorPart -shl 16) -bor [uint32]$info.FileMinorPart)
            $runtimeLS = [uint32](([uint32]$info.FileBuildPart -shl 16) -bor [uint32]$info.FilePrivatePart)
            $hasRedist = 1
        }
        & $InnoCompiler "/DStageDir=$stage" "/DOutputDir=$installerOutput" "/DRepoRoot=$repoRoot" "/DAppVersion=$packageVersion" "/DNumericVersion=$numericVersion" "/DHasVCRedist=$hasRedist" "/DVCRuntimeMS=$runtimeMS" "/DVCRuntimeLS=$runtimeLS" (Join-Path $repoRoot 'packaging/windows/SjSim.iss')
        if ($LASTEXITCODE -ne 0) { throw "Inno Setup failed: $LASTEXITCODE" }
        $name = "SjSim-$packageVersion-windows-x64-Setup.exe"
        $artifact = Join-Path $outputPath $name
        Move-Item -Force -LiteralPath (Join-Path $installerOutput $name) -Destination $artifact
        $hash = (Get-FileHash -Algorithm SHA256 -LiteralPath $artifact).Hash.ToLowerInvariant()
        [IO.File]::WriteAllText("$artifact.sha256", "$hash  $name`n")
        Write-Host "Installer ready: $artifact"
    }
    if ($KeepBundle -or $BundleOnly) {
        $bundle = Join-Path $outputPath 'windows-bundle'
        if (Test-Path -LiteralPath $bundle) {
            if (((Get-Item -LiteralPath $bundle).Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0) { throw "Refusing to replace a linked bundle directory: $bundle" }
            Remove-Item -Recurse -Force -LiteralPath $bundle
        }
        Move-Item -LiteralPath $stage -Destination $bundle
        Write-Host "Debug bundle: $bundle"
    }
} finally {
    if (Test-Path -LiteralPath $workspace) { Remove-Item -Recurse -Force -LiteralPath $workspace }
}
