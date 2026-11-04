# Paczki Windows i Linux

Workflow `.github/workflows/release.yml` buduje Release na obu systemach po pushu i ręcznie przez `workflow_dispatch`. Windows używa MSVC, Linux GCC; oba Qt 6.8.3. ZIP i TAR.GZ trafiają do artefaktów na 14 dni. Publikacja GitHub Release następuje dopiero dla tagu `v…`; tag zawierający `-` oznacza prerelease.

## Lokalnie

Windows, w terminalu z Qt i MSVC:

```powershell
powershell -ExecutionPolicy Bypass -File build-tools/dev.ps1 -Action Build
powershell -ExecutionPolicy Bypass -File scripts/package-windows.ps1
$env:QT_QPA_PLATFORM = 'offscreen'
$env:QT_QUICK_BACKEND = 'software'
& ./dist/sj-sim-windows-x64/sj-sim.exe --deployment-smoke
Compress-Archive -Path dist/sj-sim-windows-x64 -DestinationPath dist/sj-sim-windows-x64.zip -Force
```

Linux, z Qt 6, kompilatorem i narzędziami wymienionymi w workflow:

```sh
mkdir -p build-linux
cd build-linux
qmake ../sj-sim.pro CONFIG+=release
make -j4
cd ..
export APPIMAGE_EXTRACT_AND_RUN=1
export QML_SOURCES_PATHS="$PWD/source/UI/Quick"
export EXTRA_PLATFORM_PLUGINS=libqoffscreen.so
bash scripts/package-linux.sh
QT_QUICK_BACKEND=software xvfb-run -a timeout 30s dist/sj-sim-linux-x64/Sj.Sim --deployment-smoke
```

Skrypt Linux wymaga dwóch plików `linuxdeploy-…AppImage` pobieranych w workflow. Nie tworzy pojedynczego AppImage: gotową paczką jest archiwum z całym katalogiem programu. Folder `userData` jest przenośny i pozostaje w paczce obok pliku wykonywalnego.

`--deployment-smoke` kończy program kodem 0 dopiero po poprawnym utworzeniu widoków QML. Samo pozostawanie procesu przy życiu nie wystarcza. Kontrola składni workflow: `actionlint -shellcheck= .github/workflows/release.yml`.

Lokalna próba nie uruchamia GitHub Actions i nie publikuje wydania. Nie zastępuje sprawdzenia paczki na czystym Windowsie i docelowej dystrybucji Linuksa.

## Sprawdzone lokalnie — 4 listopada 2026

- `actionlint` 1.7.12: workflow bez błędów.
- Windows: MSVC, Qt 6.8.3, Release, `windeployqt`, uruchomienie widoków QML z paczki i ZIP. Test zakończył się kodem 0.
- Linux: WSL Ubuntu 26.04, GCC, Qt 6.10.2, pełny build Release, `linuxdeploy`, TAR.GZ i uruchomienie z gotowej paczki w trybie `offscreen`. Test zakończył się kodem 0. To dodatkowa kontrola zgodności; dokładny obraz CI z Qt 6.8.3 na Ubuntu 22.04 nie był uruchamiany.
- Skrypt Linux: składnia Bash i odrzucenie ścieżek wychodzących poza `dist`, z zachowaniem pliku kontrolnego.
- Windows: dwie kopie programu z różnymi folderami `userData` nie mogą równocześnie zapisywać wspólnych profili AppData.

Nie wykonano pusha, uruchomienia zdalnego workflow ani publikacji wydania.
