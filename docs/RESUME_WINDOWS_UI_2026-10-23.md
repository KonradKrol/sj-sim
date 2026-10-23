# Do kontynuacji: interfejs Windows

**Aktualizacja 23.10.2026:** użytkownik polecił kontynuować. Naprawę prezentacji stron, testy opcji konkursu i przejście na kody FIS opisano w [raporcie wykonanych prac](WINDOWS_UI_AND_FIS_2026-10-23.md). Poniżej zachowano historyczne zgłoszenie i punkt wyjścia; wcześniejsze polecenie oczekiwania już nie obowiązuje.

Zapis zgłoszenia użytkownika po sesji 22.10.2026. Użytkownik polecił zapisać problemy na jutro i zaczekać na jego polecenie kontynuacji. Nie rozpoczynać napraw w ramach samego zapisu.

## Otwarte problemy zgłoszone przez użytkownika

1. **Nienaturalne otwieranie ekranów na Windowsie.** Przy przejściu z menu widać, jakby osobny dialog otwierał się lub przemieszczał, zamiast płynnego przełączenia zawartości jednego okna. Użytkownik opisuje to jako toporną, subtelną animację, której nie widział na Linuxie. Chce prawdziwego przełączania ekranów bez widocznego pojawiania się natywnych dialogów. Ustalić przyczynę i poprawić zachowanie, zamiast maskować je dodatkową animacją.
2. **Nie otwierają się opcje konkursu.** Użytkownik wskazał konkretnie **„Przebieg”** oraz inne opcje konkursu. Trzeba odtworzyć dokładną ścieżkę w aplikacji, sprawdzić wszystkie te opcje i naprawić nawigację. Nazwy pozostałych niedziałających opcji nie zostały jeszcze podane; nie zakładać, że problem ogranicza się do „Przebiegu”.

Oba problemy pozostają otwarte. Poprzednie testy przejścia z menu nie potwierdzają działania wszystkich podstron konkursu ani jakości widocznych przejść.

## Stan środowiska i kodu

- Repozytorium: `C:\Users\user\Desktop\sj-sim`.
- Gałąź: `feature/qml-competition-config`; nie przełączać na `main`.
- Aktualny commit przed zapisaniem tej notatki: `db2b4ee` — poprawki inicjalizacji stron Quick na Windowsie i wybór stylu Basic w Qt 6.
- Wcześniejszy lokalny commit: `d55ed9d` — sygnatury qHash dla Qt 6 i sprzątanie scen Quick przed niszczeniem okna głównego.
- Commity są lokalne; nie były wypychane do GitHub.
- Wersja aplikacji pozostaje `1.3.0 beta-3`.
- Qt 6.8.3 MSVC 2022 x64: `build-tools\Qt\6.8.3\msvc2022_64`.
- Visual Studio Build Tools 2022 17.14.42, MSVC 19.44.35229, SDK 10.0.26100.0.
- Lokalny skrypt: `build-tools\dev.ps1`, akcje `Env`, `Build`, `Test`, `Run`. Jest ignorowany przez Git, podobnie jak lokalna instalacja Qt, logi i katalogi budowania.
- Zwykłe exe: `build-win\release\sj-sim.exe`. Skrót na pulpicie: **Sj.Sim QML**.
- Baza, ustawienia, flagi i tłumaczenie są obok exe. Zachować istniejące `userData` i zapisy użytkownika.

## Co naprawiono i zweryfikowano wcześniej

- Natywne Qt na Windowsie wysyłało ponowne zdarzenie Show w trakcie konstruowania QQuickWidget. QuickUi próbowało dodać do stosu widok, którego adapter jeszcze nie miał. Zabezpieczenie `sjQuickInstalling` zapobiegało crashowi `0xc0000005` w `QStackedLayout::addWidget`.
- Domyślny natywny styl Quick Controls na Windowsie źle współpracował z własnymi kontrolkami aplikacji. Wybór `Basic` w konstruktorze QuickUi, przed załadowaniem QML, usuwał zatrzymanie obserwowane przy otwieraniu konfiguracji konkursu.
- Release zbudowany. `--navigation-only` i `--home-launch-only` przeszły w offscreen/software i w natywnym oknie Windows. Drugi test otwiera osiem ekranów menu i wraca z nich do menu; nie przechodzi wszystkich podstron konkursu.
- Wdrożone exe z `--quick-competition` uruchomiono i zamknięto z kodem 0. To również nie pokrywa obecnego zgłoszenia dotyczącego podstron konkursu.
- Nie porównywano pełnego buildu Debug z Release. Nie przypisywać obecnych problemów optymalizacji kompilatora bez odtworzenia i dowodów.

## Punkty startowe na kolejną sesję

- `source/UI/Quick/QuickUi.cpp`: `eventFilter`, `attach`, `push`, `removePage`, `showCurrent`, obsługa modalności oraz `WA_DontShowOnScreen`, `setParent`, `show` i przesuwanie kontrolerów poza ekran. Zbadać, czy to te operacje wywołują widoczny efekt natywnego dialogu. To hipoteza do sprawdzenia, nie potwierdzona przyczyna.
- `source/UI/Quick/QuickBridge.cpp`: przekazywanie akcji i otwieranie ekranów konkursu. Prześledzić akcję „Przebieg” do właściwego kontrolera i sprawdzić, na którym etapie przestaje być widoczna.
- `tests/quick-ui-smoke.cpp`: rozszerzyć testy o rzeczywiste podstrony konkursu i przejścia między nimi. Natywny Windows jest obowiązkowy do oceny widocznego pojawiania się dialogów; offscreen tego nie potwierdzi.
- `WINDOWS_SETUP.MD`: budowanie i testy. Nie używać starego eksperymentu `CONFIG+=competition_quick` do zwykłego buildu.
- Lokalne narzędzia diagnostyczne w `build-tools`: `trace-crash.py` (stos natywnego procesu), `run-ui-checks.py`, logi native/offscreen. Nie zakładać, że są częścią repozytorium na innym komputerze.

Po poleceniu użytkownika, aby kontynuować, najpierw odtworzyć oba zgłoszenia na aktualnym exe, a następnie naprawić i zweryfikować widoczne zachowanie oraz działanie opcji konkursu. Nie uznawać samego braku crasha za zakończenie zadania.
