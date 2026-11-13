# Exercises staging and failure paths with mocked Qt tools; no Windows binary is run.
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$repo = Split-Path -Parent $PSScriptRoot
$id = [guid]::NewGuid().ToString('N')
$buildRelative = "build-packaging-tests/windows-$id"
$outputRelative = "dist/.windows-script-test-$id"
$build = Join-Path $repo $buildRelative
$output = Join-Path $repo $outputRelative
$global:mockTranslations = Join-Path $build 'qt-translations'
$global:mockDeployFailure = $false
$global:mockDeployCalls = 0
function global:windeployqt {
    $global:mockDeployCalls++
    if ($global:mockDeployFailure) { $global:LASTEXITCODE=42; return }
    $values = @($args); $stage = $values[[array]::IndexOf($values,'--dir')+1]
    New-Item -ItemType Directory -Force -Path (Join-Path $stage 'platforms') | Out-Null
    [IO.File]::WriteAllText((Join-Path $stage 'platforms/qwindows.dll'),'mock Qt plugin')
    $global:LASTEXITCODE=0
}
function global:qmake { $global:LASTEXITCODE=0; return $global:mockTranslations }
function global:lrelease {
    $values = @($args); $target = $values[[array]::IndexOf($values,'-qm')+1]
    [IO.File]::WriteAllText($target,'mock compiled translation'); $global:LASTEXITCODE=0
}
try {
    New-Item -ItemType Directory -Force -Path $build, $global:mockTranslations | Out-Null
    [IO.File]::WriteAllText((Join-Path $build 'sj-sim.exe'),'mock application')
    foreach ($language in @('pl','cs')) { [IO.File]::WriteAllText((Join-Path $global:mockTranslations "qtbase_$language.qm"),'mock Qt translation') }
    & (Join-Path $repo 'scripts/package-windows.ps1') -BuildDir $buildRelative -OutputDir $outputRelative -BundleOnly
    $bundle = Join-Path $output 'windows-bundle'
    $athleteFile = Get-ChildItem (Join-Path $bundle 'Defaults/Packs/Athletes') -Recurse -Filter pack.json | Select-Object -First 1
    $hillFile = Get-ChildItem (Join-Path $bundle 'Defaults/Packs/Hills') -Filter '*.json' | Select-Object -First 1
    if ((Get-Content -Raw $athleteFile.FullName | ConvertFrom-Json).profiles.Count -ne 83) { throw 'Wrong athlete defaults' }
    if ((Get-Content -Raw $hillFile.FullName | ConvertFrom-Json).profiles.Count -ne 128) { throw 'Wrong hill defaults' }
    if (Test-Path (Join-Path $bundle 'userData')) { throw 'Legacy userData was packaged' }
    if (-not (Test-Path (Join-Path $bundle 'platforms/qwindows.dll'))) { throw 'Qt deployment output missing' }
    $marker = Join-Path $bundle 'previous-package.txt'; [IO.File]::WriteAllText($marker,'keep previous successful package')
    $global:mockDeployFailure = $true; $failed = $false
    try { & (Join-Path $repo 'scripts/package-windows.ps1') -BuildDir $buildRelative -OutputDir $outputRelative -BundleOnly } catch { $failed=$true }
    if (-not $failed -or -not (Test-Path $marker)) { throw 'Failed deployment destroyed previous package' }
    $calls = $global:mockDeployCalls; $failed = $false
    try { & (Join-Path $repo 'scripts/package-windows.ps1') -BuildDir $buildRelative -OutputDir 'outside-dist' -BundleOnly } catch { $failed=$true }
    if (-not $failed -or $calls -ne $global:mockDeployCalls) { throw 'Invalid output path was accepted' }
    Write-Host 'PASS Windows packaging script: complete defaults, Qt staging, failure preservation and output bounds'
} finally {
    if (Test-Path $build) { Remove-Item -Recurse -Force $build }
    if (Test-Path $output) { Remove-Item -Recurse -Force $output }
    foreach ($name in @('windeployqt','qmake','lrelease')) { Remove-Item "function:global:$name" -ErrorAction SilentlyContinue }
}
