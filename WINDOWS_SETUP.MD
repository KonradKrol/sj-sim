# Sj.Sim — rozwój na Windows od zera

Instrukcja dla osoby pobierającej projekt oraz dla Codexa przygotowującego środowisko. Dotyczy gałęzi **`feature/qml-competition-config`**, z domyślnym interfejsem Qt Quick. Nie wymaga przechodzenia na `main`.

## 1. Najważniejsze informacje dla Codexa

- Projekt buduje **qmake**, plik wejściowy: [`sj-sim.pro`](sj-sim.pro). Standard języka: **C++17**.
- Zalecany zestaw Windows: **Qt 6.8.3 / MSVC 2022 / x64**. Takie wersje wskazuje [workflow Windows](.github/workflows/release.yml); nie wybieraj automatycznie najnowszego Qt ani innego kompilatora.
- Weryfikacja Windows x64 (22.10.2026): Qt 6.8.3, Build Tools 2022 17.14.42, MSVC 19.44.35229 i SDK 10.0.26100.0. Zbudowano Release Quick i program testowy; uruchomiono natywne okno aplikacji, a `--backend-only` i `--author-only` zakończyły się kodem 0. Pełny zestaw GUI i osobny build Widgets-only nie były wykonywane na Windowsie. Wcześniejsze testy pełnego zestawu wykonywano na Linuxie z Qt 5.15.3.
- Nowy interfejs jest w `source/UI/Quick`. Eksperyment `source/UI/Competition/Quick` jest starszą, odrębną implementacją. **Nie dodawaj `CONFIG+=competition_quick` do zwykłego buildu.**
- Widoki Quick nadal korzystają z kontrolerów Widgets. Potrzebne są oba zestawy bibliotek. `--widgets` przełącza UI podczas uruchamiania; `CONFIG+=widgets_only` tworzy osobny build bez Quick.
- W Qt 6 aplikacja wybiera styl Quick Controls `Basic` przed załadowaniem QML. Własne tła i zawartość kontrolek wymagają stylu obsługującego modyfikacje; domyślny natywny styl Windows nie jest odpowiedni dla tego interfejsu.
- Najpierw wykryj istniejące narzędzia, potem doinstaluj brakujące. Zachowaj jeden zgodny zestaw Qt, kompilatora, bibliotek i narzędzi x64.
- Dane muszą być **obok uruchamianego `.exe`**, zwykle w podkatalogu `release`, nie tylko obok Makefile. Nie nadpisuj istniejącego `userData` przy kolejnych buildach.
- Skrypty `scripts/*.sh` są dla Linuxa. Na Windowsie użyj poleceń PowerShell poniżej; nie kopiuj linuxowych bibliotek z `build-qml-preview`.
- Buduj poza źródłami, np. w `build-win`, `build-win-smoke`, `build-win-widgets`. Katalogi `build-*` oraz `dist` są ignorowane przez Git.
- Pracuj na gałęzi roboczej, zachowaj obecny numer wersji i rób commity. Nie resetuj zmian użytkownika i nie wypychaj na `main`.

## 2. Zależności

| Składnik | Do czego jest potrzebny |
| --- | --- |
| Windows 10/11 x64 | Docelowy system tej instrukcji; nie jest to konfiguracja ARM64. |
| Git for Windows | Pobranie kodu i historia zmian. |
| Visual Studio **2022** lub Build Tools **2022** | Narzędzia C++ x64/x86 **MSVC v143**, `cl`, `nmake`, kompilator zasobów i Windows SDK. W instalatorze wybierz workload C++ dla aplikacji desktopowych. |
| Qt **6.8.3**, kit **MSVC 2022 64-bit** | Core, GUI, Widgets, Concurrent, QML, Quick, Quick Widgets i Quick Controls 2. Biblioteki Network są też zależnością stosu QML. |
| **Qt Charts** dla tego samego kitu | Moduł dodatkowy; jest wymagany również przez interfejs Quick, ponieważ korzysta on z istniejących danych wykresów. |
| Qt SVG / plugin SVG oraz plugin JPEG | Flagi, ikony i zdjęcie autora. Upewnij się, że instalacja i pakiet zawierają te składniki. |
| Narzędzia Qt: `qmake`, `moc`, `uic`, `rcc`, `lrelease`, `windeployqt`, `qmlimportscanner` | Budowa, tłumaczenia i pakowanie. |
| Qt Test | Testy C++ z `CONFIG+=quick_ui_smoke`; moduł z instalacji Qt Base. |
| Python 3.12 + `aqtinstall` — opcjonalnie | Pobranie Qt z terminala oraz skrypty dokumentacji. Python nie jest zależnością uruchomieniową gry; przy użyciu innej obsługiwanej wersji dostosuj `py -3.12` w poleceniach. |
| Qt Creator — opcjonalnie | IDE i debugger; terminalowy build go nie wymaga. |

Nie trzeba instalować DPP, Boost, Node.js, npm, Reacta, Electrona, baz danych ani osobnego serwera. Projekt dołącza `source/3rdparty/linux-dpp-stub` **także na Windowsie**. Nazwa katalogu jest historyczna; w tej konfiguracji dostarczanie webhooków Discord jest wyłączone. Nie przywracaj starych zewnętrznych bibliotek DPP, żeby uzyskać zwykły build.

## 3. Instalacja i pobranie projektu

### Git i narzędzia MSVC

W PowerShell, jeżeli tych programów jeszcze nie ma:

```powershell
winget install --exact --id Git.Git --source winget
winget install --exact --id Microsoft.VisualStudio.2022.BuildTools --source winget --override "--wait --passive --add Microsoft.VisualStudio.Workload.VCTools --includeRecommended"
```

Można użyć instalatorów [Git for Windows](https://git-scm.com/download/win) i [Visual Studio Build Tools](https://visualstudio.microsoft.com/downloads/). W istniejącym Visual Studio 2022 wystarczy uzupełnić workload C++, MSVC v143 i Windows SDK. Zakończ instalację i otwórz nowy terminal, aby zobaczył zmienione środowisko.

Pobierz **właściwą gałąź**:

```powershell
$repoRoot = Join-Path $env:USERPROFILE "dev\sj-sim"
New-Item -ItemType Directory -Force (Split-Path $repoRoot) | Out-Null
git clone --branch feature/qml-competition-config https://github.com/KonradKrol/sj-sim.git $repoRoot
if ($LASTEXITCODE -ne 0) { throw "Nie udało się pobrać repozytorium" }
Set-Location $repoRoot
git status --short --branch
```

Jeżeli repozytorium już istnieje, przejdź do niego i ustaw `$repoRoot = (Get-Location).Path`. Nie klonuj ponownie do istniejącego katalogu ani nie przełączaj gałęzi z niezapisanymi zmianami bez ich sprawdzenia.

### Qt — wybierz jeden sposób

**A. Instalator Qt.** Użyj [Qt Online Installer](https://www.qt.io/download-qt-installer-oss). Zainstaluj Qt 6.8.3, MSVC 2022 64-bit, Qt Charts i narzędzia Qt. Jeżeli starsza wersja nie jest widoczna w instalatorze, skorzystaj ze sposobu B. Qt Creator jest opcjonalny. Przykładowy kit po instalacji: `C:\Qt\6.8.3\msvc2022_64`.

**B. Instalacja z terminala dla Codexa.** `aqtinstall` pobiera gotowe pakiety Qt; nie instaluje MSVC. Składnię i wyszukiwanie wersji opisuje [dokumentacja aqtinstall](https://aqtinstall.readthedocs.io/en/latest/getting_started.html).

```powershell
# Pomiń instalację Pythona, jeżeli masz już odpowiedni interpreter.
winget install --exact --id Python.Python.3.12 --source winget
# Po pierwszej instalacji otwórz nowy PowerShell i ustaw repoRoot ponownie.

$aqtEnv = Join-Path $env:USERPROFILE "tools\sj-sim-aqt"
py -3.12 -m venv $aqtEnv
if ($LASTEXITCODE -ne 0) { throw "Nie udało się utworzyć środowiska aqt" }
$aqtPython = Join-Path $aqtEnv "Scripts\python.exe"
& $aqtPython -m pip install --upgrade pip aqtinstall
if ($LASTEXITCODE -ne 0) { throw "Nie udało się zainstalować aqtinstall" }

$qtRoot = Join-Path $env:USERPROFILE "Qt"
& $aqtPython -m aqt install-qt windows desktop 6.8.3 win64_msvc2022_64 --outputdir $qtRoot -m qtcharts
if ($LASTEXITCODE -ne 0) { throw "Instalacja Qt nie powiodła się" }
$qtKit = Join-Path $qtRoot "6.8.3\msvc2022_64"
```

Jeżeli pobieranie zgłasza brak wersji/architektury/modułu, sprawdź ofertę repozytorium, zamiast zgadywać nazwę lub przechodzić na inny kompilator:

```powershell
& $aqtPython -m aqt list-qt windows desktop --arch 6.8.3
& $aqtPython -m aqt list-qt windows desktop --modules 6.8.3 win64_msvc2022_64
```

## 4. Przygotowanie terminala

Najprościej uruchom **Developer PowerShell for VS 2022**, z narzędziami x64. Dla istniejącego zwykłego terminala Codexa można załadować środowisko w bieżącym procesie:

```powershell
$vswhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
$vsRoot = & $vswhere -latest -products '*' -version '[17.0,18.0)' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $vsRoot) { throw "Brak Visual Studio 2022 z narzędziami C++" }
& (Join-Path $vsRoot "Common7\Tools\Launch-VsDevShell.ps1") -Arch amd64 -HostArch amd64 -SkipAutomaticLocation
```

Zwykłe uruchomienie `vcvars64.bat` w osobnym `cmd /c` nie przenosi jego zmiennych do nadrzędnego PowerShell. Szczegóły: [Microsoft — Developer PowerShell](https://learn.microsoft.com/en-us/visualstudio/ide/reference/command-prompt-powershell?view=vs-2022).

W tym samym terminalu ustaw faktyczne ścieżki. Jeżeli korzystasz z aqt według poprzedniej sekcji, `$qtKit` jest już ustawione; przy Qt Online Installer dopasuj je do swojego katalogu:

```powershell
$ErrorActionPreference = "Stop"
$repoRoot = Join-Path $env:USERPROFILE "dev\sj-sim"
$qtKit = Join-Path $env:USERPROFILE "Qt\6.8.3\msvc2022_64"
# Alternatywnie: $qtKit = "C:\Qt\6.8.3\msvc2022_64"
$qtBin = Join-Path $qtKit "bin"
$env:PATH = "$qtBin;$env:PATH"
$env:QML_IMPORT_PATH = Join-Path $qtKit "qml"
$env:QML2_IMPORT_PATH = Join-Path $qtKit "qml"
Set-Location $repoRoot

Get-Command cl, nmake, qmake, lrelease, windeployqt, qmlimportscanner | Select-Object Name, Source
qmake -v
qmake -query QT_INSTALL_PREFIX
$env:VSCMD_ARG_TGT_ARCH
```

Oczekiwane: Qt 6.8.3 z wybranego kitu i architektura `x64`. `qmake`, `lrelease` i `windeployqt` mają pochodzić z tej samej instalacji. Jeśli PowerShell blokuje skrypt deweloperski, użyj Developer PowerShell z menu Start; ewentualną zmianę execution policy ogranicz do tej sesji (`-Scope Process`).

## 5. Budowa i pierwsze uruchomienie

W przygotowanym terminalu:

```powershell
$buildDir = Join-Path $repoRoot "build-win"
New-Item -ItemType Directory -Force $buildDir | Out-Null
Push-Location $buildDir
try {
    & (Join-Path $qtBin "qmake.exe") (Join-Path $repoRoot "sj-sim.pro") -spec win32-msvc CONFIG+=release
    if ($LASTEXITCODE -ne 0) { throw "qmake zakończył się błędem" }
    nmake /NOLOGO
    if ($LASTEXITCODE -ne 0) { throw "Kompilacja zakończyła się błędem" }
} finally {
    Pop-Location
}
```

`nmake` nie przyjmuje linuxowego `-j`. Jeśli masz już `jom`, możesz użyć go zamiast `nmake` do równoległego budowania. Nie trzeba go instalować do pierwszego uruchomienia.

Zdefiniuj dwie funkcje pomocnicze, używane także przez testy poniżej:

```powershell
function Find-SjExecutable([string]$BuildDirectory, [string]$Target = "sj-sim") {
    foreach ($relative in @("release\$Target.exe", "$Target.exe")) {
        $candidate = Join-Path $BuildDirectory $relative
        if (Test-Path -LiteralPath $candidate) { return $candidate }
    }
    throw "Nie znaleziono $Target.exe w $BuildDirectory"
}

function Initialize-SjRuntime([string]$Executable) {
    $runtimeDir = Split-Path -Parent $Executable
    foreach ($folder in @("userData", "flags")) {
        $destination = Join-Path $runtimeDir $folder
        if (-not (Test-Path -LiteralPath $destination)) {
            Copy-Item -Recurse -LiteralPath (Join-Path $repoRoot $folder) -Destination $destination
        }
    }
    $translationDir = Join-Path $runtimeDir "translations"
    New-Item -ItemType Directory -Force $translationDir | Out-Null
    & (Join-Path $qtBin "lrelease.exe") (Join-Path $repoRoot "translations\translation_en.ts") -qm (Join-Path $translationDir "translation_en.qm")
    if ($LASTEXITCODE -ne 0) { throw "Nie udało się zbudować tłumaczeń" }
}

$appExe = Find-SjExecutable $buildDir
Initialize-SjRuntime $appExe
Start-Process -FilePath $appExe -WorkingDirectory (Split-Path $appExe)
```

Obok `.exe` mają być: `userData\GlobalDatabase`, `userData\Settings`, `flags` oraz `translations\translation_en.qm`. Program ustawia katalog roboczy na katalog wykonywalny w `source/main.cpp`; sama zmiana katalogu roboczego w IDE nie zastąpi tych plików.

Uruchomienie starego UI w tym samym buildzie:

```powershell
Start-Process -FilePath $appExe -ArgumentList "--widgets" -WorkingDirectory (Split-Path $appExe)
```

Do codziennej pracy wystarczy ponownie wykonać build. Przy zmianach `.pro`, dodaniu plików lub zmianie kitu uruchom również qmake. QML jest pakowany przez `source/UI/Quick/quick-ui.qrc`, więc edycja pliku QML wymaga przebudowy zasobów i ponownego uruchomienia gry. Zamknij starą instancję, jeżeli linker nie może zastąpić `.exe`.

W Qt Creator otwórz `sj-sim.pro`, wybierz ten sam kit MSVC x64, ustaw osobny katalog budowania i przygotuj dane przy rzeczywistym pliku wykonywalnym. Zmiana instalacji Qt lub kompilatora wymaga osobnego katalogu buildu; nie wykorzystuj plików obiektowych z innego kitu.

## 6. Testy i diagnostyka

Testy tej gałęzi są programem C++/Qt Test, nie projektem npm ani osobnym zestawem `qmltestrunner`. Zbuduj je w oddzielnym katalogu, zachowując wcześniejsze zmienne i funkcje:

```powershell
$smokeDir = Join-Path $repoRoot "build-win-smoke"
New-Item -ItemType Directory -Force $smokeDir | Out-Null
Push-Location $smokeDir
try {
    & (Join-Path $qtBin "qmake.exe") (Join-Path $repoRoot "sj-sim.pro") -spec win32-msvc CONFIG+=release CONFIG+=quick_ui_smoke
    if ($LASTEXITCODE -ne 0) { throw "qmake testów zakończył się błędem" }
    nmake /NOLOGO
    if ($LASTEXITCODE -ne 0) { throw "Nie udało się zbudować testów" }
} finally {
    Pop-Location
}
$smokeExe = Find-SjExecutable $smokeDir "sj-sim-ui-smoke"
Initialize-SjRuntime $smokeExe

$env:QT_QPA_PLATFORM = "offscreen"
$env:QT_QUICK_BACKEND = "software"
try {
    & $smokeExe --backend-only
    if ($LASTEXITCODE -ne 0) { throw "Regresje backendu nie przeszły" }
    & $smokeExe --author-only
    if ($LASTEXITCODE -ne 0) { throw "Test autora nie przeszedł" }
    & $smokeExe --navigation-only
    if ($LASTEXITCODE -ne 0) { throw "Regresje nawigacji nie przeszły" }
    & $smokeExe --home-launch-only
    if ($LASTEXITCODE -ne 0) { throw "Otwieranie ekranów menu nie przeszło" }
    # Pełny zestaw: uruchom bez flagi zamiast powtarzać wszystkie testy osobno.
    # & $smokeExe
    # if ($LASTEXITCODE -ne 0) { throw "Pełny smoke test nie przeszedł" }
} finally {
    Remove-Item Env:\QT_QPA_PLATFORM -ErrorAction SilentlyContinue
    Remove-Item Env:\QT_QUICK_BACKEND -ErrorAction SilentlyContinue
}
```

Następnie sprawdź nawigację i wszystkie osiem lokalnych ekranów menu w natywnym oknie Windows, bez `offscreen` i wymuszonego renderera software:

```powershell
& $smokeExe --navigation-only
if ($LASTEXITCODE -ne 0) { throw "Natywna nawigacja Windows nie przeszła" }
& $smokeExe --home-launch-only
if ($LASTEXITCODE -ne 0) { throw "Natywne otwieranie ekranów Windows nie przeszło" }
& $smokeExe --race-options-only
if ($LASTEXITCODE -ne 0) { throw "Opcje konkursu na Windows nie przeszły" }
```

`--navigation-only` wymusza również ponowne zdarzenie Show podczas tworzenia widoku: odtwarza błąd ujawniony na Windowsie nawet w rendererze offscreen. `--home-launch-only` otwiera sezony, konkurs, pojedyncze skoki, bazę, ustawienia, autora, pomoc i podziękowania, wraca do menu i odrzuca testowe zmiany bazy.

Weryfikacja poprawek Windows (22.10.2026, Release): oba zakresy zakończyły się kodem 0 w `offscreen` oraz w natywnym oknie z domyślnym rendererem Windows. Zwykłe, wdrożone `sj-sim.exe --quick-competition` również uruchomiono i zamknięto z kodem 0, bez dodatkowych zmiennych środowiskowych. Przed zabezpieczeniem inicjalizacji nowy test ponownego Show kończył się wyjątkiem `0xc0000005` w `QStackedLayout::addWidget`.

Inne przydatne zakresy: `--single-jumps-only`, `--ux-only`, `--workflow-only`, `--appearance-only`; pomiar: `--single-jumps-benchmark`. Testy pracują na tymczasowych kopiach danych i ustawień, ale pliki bazowe nadal muszą znajdować się obok testowego `.exe`. Obrazy trafiają do `ui-renders` przy tym pliku. Testy GUI trzeba zweryfikować również na Windowsie — pozytywny wynik linuksowego renderera offscreen nie potwierdza wszystkich zachowań Windows/GPU.

`--round-points-only` sprawdza normalnie widoczny reset punktów przed serią oraz rozwijane wagi zaawansowane. Obliczenia i zgodność starych zapisów są częścią regresji backendu. Opis funkcji: [reset i wagi punktów](docs/ROUND_POINT_WEIGHTS.md).

Aktualizacja 23.10.2026: natywne testy sprawdzają także ukrycie kontrolerów przed pierwszym `Show` oraz systemowe `IsWindowVisible`. `--race-options-only` klika „Przebieg”, „Warunki” i „Skok” przy dwóch szerokościach, sprawdza ich widoczną zawartość i zamyka panele. Szczegóły poprawek i zgodności starych zapisów z kodami FIS: [raport Windows UI i FIS](docs/WINDOWS_UI_AND_FIS_2026-10-23.md).

Do testu zgodności Widgets utwórz **osobny** katalog, np. `build-win-widgets`, i powtórz build z `CONFIG+=release CONFIG+=widgets_only`, bez `quick_ui_smoke`. Przygotuj jego dane tak samo. Nie usuwaj Widgets ani jego zależności z normalnego buildu Quick.

| Objaw | Sprawdź / zrób |
| --- | --- |
| Brak `cl`, `nmake`, `rc.exe` | Załaduj środowisko MSVC x64 i sprawdź workload C++ oraz Windows SDK. |
| Brak `qmake`, `lrelease` lub `windeployqt` | Popraw `$qtKit` i PATH; doinstaluj narzędzia do tego samego kitu. |
| `Unknown module(s) in QT: charts` | Doinstaluj Qt Charts dla dokładnej wersji i architektury Qt. |
| Brak `quickwidgets` / `quickcontrols2`, błąd importu `QtQuick.Controls` | Uzupełnij Qt Declarative/Quick i Controls; sprawdź, czy narzędzia oraz ścieżki QML wskazują ten sam kit. |
| Brak Qt DLL lub błąd `0xc000007b` | Zweryfikuj PATH i zgodność x64/MSVC; nie mieszaj Qt 5/6, MinGW/MSVC ani Debug/Release. |
| Brak pluginu `windows` lub błędy pluginów | Użyj `QT_DEBUG_PLUGINS=1` w bieżącej sesji; sprawdź `platforms\qwindows.dll` w paczce i obce `QT_PLUGIN_PATH`. |
| Nie widać flag, SVG lub zdjęcia | Sprawdź `flags`, `Qt6Svg.dll`, pluginy SVG/JPEG i wynik `windeployqt`. Fonty i zdjęcie są zasobami repozytorium; nie instaluj fontów ręcznie. |
| Puste bazy / błędy odczytu JSON | Dane skopiowano obok Makefile zamiast do katalogu `.exe`, albo brakuje części `userData`. |
| Brak angielskiego tłumaczenia | Wykonaj `lrelease`; `.ts` nie zastępuje uruchomieniowego `.qm`. |
| Pusta scena / problem sterownika | Próbnie uruchom z `QT_QUICK_BACKEND=software`, potem usuń tę zmienną i sprawdź właściwy renderer. |
| LNK1104 przy kolejnym buildzie | Zamknij uruchomiony proces gry/testów. |

Do debugowania utwórz inny build z `CONFIG+=debug CONFIG-=release`. W Release projekt definiuje `QT_NO_DEBUG_OUTPUT`, dlatego brak `qDebug` jest zamierzony; ostrzeżenia i błędy pozostają dostępne.

## 7. Paczka Windows do uruchomienia bez Qt w PATH

Najpierw zbuduj wersję Release. Skrypt pakowania odtwarza wskazany katalog w `dist`; używaj go do tworzenia paczki, a nie jako katalogu codziennej rozgrywki z ważnymi zapisami.

```powershell
Set-Location $repoRoot
& .\scripts\package-windows.ps1 -BuildDir build-win -OutputDir dist\sj-sim-windows-x64

$packageDir = Join-Path $repoRoot "dist\sj-sim-windows-x64"
& (Join-Path $qtBin "windeployqt.exe") --release --compiler-runtime --no-translations --qmldir (Join-Path $repoRoot "source\UI\Quick") --dir $packageDir (Join-Path $packageDir "sj-sim.exe")
if ($LASTEXITCODE -ne 0) { throw "Pakowanie zależności Qt Quick nie powiodło się" }
```

**Drugi krok jest istotny:** obecny `package-windows.ps1` wywołuje `windeployqt` bez `--qmldir`. Dla domyślnego interfejsu Quick wskaż źródła QML, aby narzędzie przeanalizowało importy i dołączyło ich moduły/pluginy. Opis tego mechanizmu: [Qt — wdrażanie na Windows](https://doc.qt.io/qt-6.8/windows-deployment.html).

Sprawdź zawartość paczki: `.exe`, biblioteki Qt, plugin `platforms\qwindows.dll`, zależności QML, obsługa SVG/JPEG, `flags`, `userData` i skompilowane tłumaczenie. Przetestuj ją również z terminala bez deweloperskiego PATH; inaczej brakujące pliki mogą być nieświadomie pobierane z instalacji Qt. Na komputerze docelowym może być wymagany oficjalny Microsoft Visual C++ Redistributable x64; nie kopiuj losowych bibliotek systemowych z internetu.

## 8. Gotowe polecenie dla Codexa

> Przeczytaj WINDOWS_SETUP.MD i sj-sim.pro. Przygotuj środowisko Windows x64 do rozwoju aktualnej gałęzi feature/qml-competition-config. Wykryj istniejące MSVC/SDK/Qt, doinstaluj brakujące zależności zgodnie z instrukcją, zbuduj domyślny interfejs Quick w osobnym buildzie, przygotuj dane obok właściwego .exe i uruchom aplikację oraz odpowiednie testy. Nie pracuj na main, nie nadpisuj istniejącego userData i nie używaj starego eksperymentu competition_quick. Podaj wersje narzędzi, ścieżkę do .exe, wynik testów i ewentualny konkretny błąd wymagający dalszej poprawki.

Dalszy kontekst: [architektura Qt Quick](QUICK_UI_MIGRATION.md), [pełny wykaz zmian](docs/RELEASE_NOTES_VS_MAIN.md), [ostatni stan prac](docs/RESUME_2026-10-22.md). Instrukcje Qt z `.agents/skills` są dołączone do repozytorium i mogą być używane przez Codexa także na Windowsie.
