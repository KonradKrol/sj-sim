# Rozgrywanie konkursu w Qt Quick

Branch: `feature/qml-competition-config`. Nowy widok obejmuje samo rozgrywanie konkursów: indywidualne, KO, drużyny i konkursy uruchamiane z sezonu. Konfiguracja przed konkursem, sezony i edytory baz pozostają bez zmian.

## Uruchomienie

```sh
./scripts/build-qml-preview.sh
./scripts/run-qml-preview.sh --app
```

Rozpoczęty konkurs automatycznie otwiera nowy ekran. Przycisk **Qt Widgets** przełącza tę samą rozgrywkę na klasyczny interfejs; **Widok Qt Quick** wraca do nowego widoku. Nie tworzy drugiego konkursu. `SJ_COMPETITION_WIDGETS=1` lub argument `--widgets-competition` uruchamiają klasyczne rozgrywanie. Budowanie bez `CONFIG+=competition_quick` nadal nie wymaga modułów Quick.

Przełącznik **☀ / ☾** zapamiętuje motyw między uruchomieniami. `Ctrl+Enter` oddaje skok lub przechodzi do następnego etapu. Sterowanie i aktualny zawodnik pozostają nad wynikami. Od rozmiaru 860 × 600 px lista startowa i podgląd skoku są stale widoczne obok wyników, także podczas oglądania klasyfikacji. Po każdym skoku podgląd aktualizuje się automatycznie, bez zmiany widoku. Dwuklik wyniku pokazuje wybranego zawodnika w tym samym panelu. **Pełne dane** otwierają komplet szczegółów, wszystkich serii i sektorów wiatru.

W mniejszych oknach są zakładki wyników, listy i podglądu; skrócony podgląd pojawia się również automatycznie pod głównym widokiem. Wyniki i listy mają niezależne przewijanie, a lista startowa podąża za zawodnikiem na belce. `Alt+2`, `Alt+3`, `Alt+4` wybierają wyniki, listę i podgląd w małym oknie; `Alt+5` pokazuje klasyfikację. Ustawienia generatorów, odwołanie konkursu, tryb live i webhook są w menu **Ustawienia**.

## Zakres i implementacja

- Zawodnik, flaga i zdjęcie, poprzednia pozycja, PB, odległości do prowadzenia i awansu, belka oraz obniżenie przez trenera.
- Wiatr, śnieg, generowanie warunków, manipulacja kolejnym skokiem, ustawienia generatorów i ich dialogi z oryginalną obsługą zatwierdzania/anulowania.
- Wyniki z kolumnami kolejnych serii, kolorami statusów i flagami; wyniki drużyn z zawodnikami, grupy KO, lista startowa i klasyfikacja na żywo.
- Szczegóły poszczególnych skoków, sektory i kierunki wiatru, noty, rekompensaty i pozostałe informacje wybrane w ustawieniach aplikacji.
- Auto-symulacja konkursu, serii, grupy i liczby skoków; odwołanie serii/konkursu, przechodzenie między grupami i seriami, zakończenie kwalifikacji/konkursu, rekordy, komunikaty i webhooki zgodnie z dotychczasowym C++.

`CompetitionLiveBridge` prezentuje istniejące kontrolki i modele `CompetitionManagerWindow`; operacje wywołują ich oryginalne sygnały. Kontrolki Widgets pozostają w ukrytym kontenerze. Lokalny filtr zamienia zawartość dialogów należących do danego konkursu na Quick, pozostawiając oryginalny `QDialog`, pętlę modalną, wynik i połączenia sygnałów. Filtr nie obejmuje innych ekranów aplikacji. Przy błędzie ładowania QML widoczny pozostaje interfejs Widgets.

Modele tabel są buforowane i unieważniane przez sygnały modeli i operacje konkursu. Odświeżanie tekstów podczas animacji rozgrywki nie wymaga ponownego odczytu wszystkich wyników.

## Weryfikacja

```sh
cd build-qml-preview
# Przy lokalnym SDK z tego komputera:
export QMAKEPATH="$PWD/qt-dev/root/usr/lib/x86_64-linux-gnu/qt5${QMAKEPATH:+:$QMAKEPATH}"
qmake ../sj-sim.pro CONFIG+=competition_quick CONFIG+=competition_quick_smoke -o Makefile.smoke
make -f Makefile.smoke -j4
LD_LIBRARY_PATH="$PWD/qt-runtime/lib" QML2_IMPORT_PATH="$PWD/qt-runtime/qml" \
QT_QPA_PLATFORM=offscreen QT_QUICK_BACKEND=software ./sj-sim-quick-smoke
```

Test działa na tymczasowej kopii danych. Oprócz dotychczasowych testów konfiguracji przeprowadza pełne konkursy indywidualny, KO i drużynowy, otwiera dialogi Quick, przełącza Widgets/Quick, sprawdza jednoczesną widoczność listy i podglądu w głównym ekranie, automatyczne pojawienie się danych ostatniego skoku, dwuklik wyników, przewijanie kółkiem myszy i automatyczną symulację. Zapisuje rendery przy szerokościach 1440, 1024 i 520 px dla obu motywów oraz widoki rzeczywistych wyników i szczegółów skoku.
