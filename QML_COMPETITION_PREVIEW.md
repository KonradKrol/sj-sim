# Konfiguracja konkursu w Qt Quick

Eksperyment na branchu `feature/qml-competition-config`. Pozostałe ekrany i symulator pozostają w C++/Qt Widgets.

## Uruchomienie gotowego podglądu

Z katalogu repozytorium:

```sh
./scripts/run-qml-preview.sh
```

Skrypt uruchamia `build-qml-preview/sj-sim` bezpośrednio na ekranie konfiguracji konkursu. Lokalnie przygotowany katalog `build-qml-preview/qt-runtime` zawiera brakujące biblioteki i moduły QML; nie wymaga instalacji przez sudo ani plików z `/tmp`. To podgląd na tym komputerze, nie samodzielny pakiet dystrybucyjny.

Przycisk **Widok klasyczny** otwiera dotychczasowy ekran z tymi samymi ustawieniami. Po zamknięciu tego okna wrócisz do QML, zachowując zmiany. Aby od początku używać Widgets:

```sh
./scripts/run-qml-preview.sh --widgets
```

Podgląd ma własną kopię `userData` i `flags` obok pliku wykonywalnego. Nie nadpisuje danych z `build-linux` ani katalogu źródłowego. Ponowne budowanie nie zastępuje danych podglądu.

## Zakres

- QML: wybór i pełne parametry skoczni, presety i ważność skoków, belka/auto-belka, DSQ, kwalifikacje, zasady, serie i ustawienia KO, wszystkie sektory wiatru, śnieg oraz indywidualna lista startowa (wyszukiwanie, wielokrotny wybór, przesuwanie, usuwanie, import i sortowanie).
- Istniejące okna pomocnicze Widgets: cechy charakterystyczne skoczni, edycja składów drużyn/grup KO, losowanie i zbiorcze wypełnianie wiatru, wybór pliku i komunikaty walidacji. To etapowa migracja ekranu, nie przepisanie całej aplikacji.
- Dwie kolumny na desktopie, jedna w wąskim oknie, przewijany formularz i stale dostępny start. Minimalne okno: 480 × 480. Krótkie animacje zakładek, przejścia sekcji, przełączników i przycisków.
- `Alt+1`…`Alt+5`: sekcje; `Ctrl+Enter`: start; Tab: nawigacja po kontrolkach.

## Budowanie

Qt 5.15 z modułami Widgets, Charts, Concurrent, QML, Quick, QuickWidgets i QuickControls2. Na Ubuntu potrzebne są m.in. `qtdeclarative5-dev`, `qtquickcontrols2-5-dev`, `qml-module-qtquick-controls2`, `qml-module-qtquick-layouts`, `qml-module-qtquick-window2`, `qml-module-qtquick2` i `qml-module-qtquick-templates2`, oprócz dotychczasowych zależności projektu.

```sh
./scripts/build-qml-preview.sh
```

Przy ręcznym budowaniu użyj `qmake ../sj-sim.pro CONFIG+=competition_quick` w osobnym katalogu. Bez `CONFIG+=competition_quick` projekt nadal buduje wyłącznie Widgets, bez zależności od Qt Quick. W razie problemów ze sterownikiem grafiki można uruchomić `QT_QUICK_BACKEND=software ./scripts/run-qml-preview.sh`.

## Kompatybilność

`CompetitionQuickBridge` jest adapterem istniejącej instancji `CompetitionConfigWindow`. Nie utrzymuje osobnego modelu danych konkursu ani nie kopiuje algorytmów symulacji. QML przekazuje zmiany do oryginalnych kontrolek, uruchamiając ich sygnały C++; ustawienia odczytywane przy starcie pochodzą z tej samej instancji. Zakresy, precyzja i opcje pól są pobierane z kontrolek. Dynamiczne serie/sektory są odświeżane po zakończeniu obsługi sygnałów; adapter korzysta z `QPointer` dla niszczonych kontrolek.

`fields.json` opisuje etykiety i kolejność pól. Dodając pole do edytora Widgets, należy dopisać je również do tego schematu. Stary ekran `.ui` i jego implementacja pozostają na miejscu. Zmiana w nagłówku inicjalizuje też dwa wcześniej niezainicjalizowane wskaźniki na `nullptr`.

## Test integracyjny

W zbudowanym katalogu podglądu, z dostępnym Qt Quick i Qt Test:

```sh
qmake ../sj-sim.pro CONFIG+=competition_quick CONFIG+=competition_quick_smoke -o Makefile.smoke
make -f Makefile.smoke -j4
QT_QPA_PLATFORM=offscreen QT_QUICK_BACKEND=software ./sj-sim-quick-smoke
```

Przy lokalnym runtime trzeba dodatkowo ustawić `LD_LIBRARY_PATH="$PWD/qt-runtime/lib"` i `QML2_IMPORT_PATH="$PWD/qt-runtime/qml"`.

Test sprawdza ładowanie QML, wpisywanie z klawiatury, przepływ ustawień w obie strony, powrót z Widgets, serie/KO/drużyny/sektory wiatru, usuwanie wielu zawodników, oryginalną walidację i otwarcie właściwego menedżera konkursu. Zapisuje 15 renderów sekcji przy szerokościach 1440, 900 i 520 px. Symulacja testowa korzysta z tymczasowej kopii danych; test nie przeprowadza całego konkursu do końca.
