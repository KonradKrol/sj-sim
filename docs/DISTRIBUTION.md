# Instalacja i wydawanie Sj.Sima

## Dla gracza

**Windows 10/11, 64-bit:** pobierz `SjSim-<wersja>-windows-x64-Setup.exe`, uruchom
instalator, następnie korzystaj ze skrótu Sj.Sim w menu Start lub na pulpicie.
Nie trzeba osobno instalować Qt ani rozpakowywać bibliotek. W razie potrzeby
instalator uruchomi dołączony oficjalny Microsoft Visual C++ Redistributable.
Sam Sj.Sim instaluje się dla bieżącego użytkownika bez wymuszania administratora;
instalacja brakującego składnika Microsoft może poprosić o podwyższone uprawnienia.

**Linux x86_64:** pobierz `SjSim-<wersja>-linux-x86_64.AppImage`. Właściwości pliku →
Uprawnienia → zezwól na uruchamianie, następnie otwórz plik. To jeden plik programu,
bez konieczności zachowania folderów obok niego. W terminalu:

```sh
chmod +x SjSim-*-linux-x86_64.AppImage
./SjSim-*-linux-x86_64.AppImage
```

Na systemie bez działającego FUSE można uruchomić ten sam plik przez
`APPIMAGE_EXTRACT_AND_RUN=1 ./SjSim-…AppImage`. Tymczasowe rozpakowanie wykonuje
runtime AppImage; nadal pobiera się jeden plik. Build CI bazuje na Ubuntu 22.04;
nie zakładamy obsługi starszych bibliotek systemowych niż użyte do kompilacji.

Pliki `.sha256` przy pobieraniu są opcjonalnymi sumami kontrolnymi, nie drugim
składnikiem potrzebnym do uruchomienia.

## Paczki i zapisy

Na obu platformach biblioteka jest w **Dokumenty/SjSimData** lub w folderze
wybranym w ustawieniach → Biblioteka. Pierwszy start przygotowuje aktualne
zasoby z `data/defaults`: 83 skoczków, 128 skoczni, zasady, punktacje i kalendarze.

Aktualizacja programu nie nadpisuje istniejącej biblioteki. Instalator i jego
uninstaller nie usuwają folderu Dokumenty/SjSimData ani folderu wybranego przez
grę. Nie używamy `[UninstallDelete]` do usuwania danych gracza.
AppImage aktualizujesz, pobierając nowy plik; stary można usunąć.
Dodatkowy opis folderów i paczek: [RESOURCE_LIBRARY.md](RESOURCE_LIBRARY.md).

## Zbudowanie dystrybucji

Zawsze zacznij od świeżego buildu Release. Nie pakuj uruchomionego procesu ani
starego executable po zmianach w źródłach. Skrypty pakowania nie kompilują gry.

### Windows

Terminal z MSVC, zgodnym Qt, `qmake`, `lrelease`, `windeployqt` oraz Inno Setup
6.3 lub nowszym. Compiler Inno jest wykrywany w PATH i standardowych folderach
instalacyjnych; można przekazać `-InnoCompiler 'C:\…\ISCC.exe'`.

```powershell
.\scripts\package-windows.ps1 -BuildDir build-win
.\scripts\test-windows-installer.ps1 -Installer .\dist\SjSim-2.0.0-windows-x64-Setup.exe
```

Wynik domyślny: jeden `Setup.exe` i suma kontrolna w `dist`. Wszystkie pliki Qt,
zdjęcia paczki, tłumaczenia, flagi i Defaults są wewnątrz instalatora.
`-KeepBundle` pozostawia dodatkowo `dist/windows-bundle` do diagnozowania wdrożenia.
`-BundleOnly` tworzy sam taki folder bez potrzeby posiadania Inno Setup; nie jest
to docelowy format dla gracza.

Test instalatora wykonuje cichą instalację do katalogu tymczasowego, uruchamia
faktycznie zainstalowany program z `--deployment-smoke`, instaluje aktualizację
oraz odinstalowuje program. Sprawdza zachowanie pliku utworzonego przez użytkownika.
Proces gry nie dostaje deweloperskich ścieżek Qt; start używa izolowanych danych.
Nie uruchamiaj testu instalatora na koncie z istniejącą produkcyjną instalacją
Sj.Sima: używa tej samej tożsamości instalatora. CI uruchamia go na świeżej maszynie.

Instalator ma stałe AppId, skróty Start/pulpit i rejestrację paczek `.sjsim-pack`
w „Otwórz za pomocą”. Nie zmienia wymuszeniem domyślnego programu dla plików.
Podpisywanie certyfikatem nie zostało skonfigurowane; Windows może wyświetlać
ostrzeżenie o nieznanym wydawcy przy pobieraniu.

### Linux

```sh
scripts/build-quick-ui.sh
scripts/package-linux.sh build-quick-ui
APPIMAGE_EXTRACT_AND_RUN=1 QT_QPA_PLATFORM=offscreen QT_QUICK_BACKEND=software \
  dist/SjSim-2.0.0-linux-x86_64.AppImage --deployment-smoke
```

Potrzebne są Python 3, Qt SDK z QML i narzędzia buildu. Pierwsze pakowanie pobiera
oficjalne linuxdeploy oraz plugin Qt do `build-packaging-tools/linuxdeploy` i
sprawdza SHA-256 udostępnione przez GitHub. Kolejne używają zweryfikowanej kopii;
`SJ_PACKAGE_OFFLINE=1` zabrania pobierania. Usunięcie cache wymusza pobranie nowych
wersji narzędzi. Nie trzeba ręcznie pobierać AppImage narzędzi ani instalować
ImageMagick — ikona PNG jest wyodrębniana bez przeskalowania z istniejącego ICO.

Skrypt korzysta z pełnego systemowego Qt. Wyłącznie przy naszym niepełnym
linuksowym SDK Qt 5 wykorzystuje pasujący runtime i qmlimportscanner z
`build-qml-preview`; instalacje Qt 6 i CI nie wymagają tego obejścia.

Budowanie odbywa się w tymczasowym katalogu. Dopiero udane pakowanie zastępuje
poprzedni plik wynikowy. Pliki robocze i układ AppDir nie są częścią pobieranej
paczki. Uprawnienia plików Defaults są normalizowane, aby paczkę mógł uruchomić
inny użytkownik niż autor buildu.

## CI i publikacja

`.github/workflows/release.yml` buduje obie platformy i testuje ich docelowe
formaty po pushu. Windows sprawdza instalację/aktualizację/uninstall, Linux
uruchamia rzeczywisty AppImage z izolowanymi danymi i bez ścieżek deweloperskiego Qt.
Artefakty to Setup.exe i AppImage wraz z SHA-256. Tag `v…` publikuje je jako wydanie;
zwykły push nie publikuje wydania.

Źródła narzędzi i zachowania:
[Inno Setup](https://jrsoftware.org/isinfo.php),
[tryb instalacji per-user](https://jrsoftware.org/ishelp/topic_admininstallmode.htm),
[wdrażanie Qt na Windows](https://doc.qt.io/qt-6/windows-deployment.html),
[linuxdeploy](https://docs.appimage.org/packaging-guide/from-source/linuxdeploy-user-guide.html).

Verification on Linux (13 November 2026): the generated AppImage passed an actual
Qt Quick startup with development Qt paths removed and an isolated fresh library.
Its payload contains the starter packs and six calendars, without user saves.
The Windows packaging script passed PowerShell parsing and isolated deployment
success/failure checks. Native installer compilation and installation are checked
by the Windows CI job; they cannot be run on the Linux development host.
