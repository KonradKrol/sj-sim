# Kolejność drużyn, awans LL i podgląd sposobów punktowania

## Szybkie przesuwanie drużyn

Przyciski góra/dół przebudowywały model wszystkich drużyn i składów oraz podmieniały model widoku. Teraz `TeamsSquadsTreeModel::moveTeam` przenosi istniejący wiersz wraz ze składem przez `beginMoveRows`/`endMoveRows`. Kolejność danych i elementów drzewa zmienia się razem. Zaznaczenie, bieżąca drużyna i indeksy składów zachowują tożsamość. Adapter Qt Quick przekazuje ruch wiersza bez resetowania całej listy; filtrowany widok korzysta z bezpiecznego odświeżenia. Ruch poza listę lub na tę samą pozycję jest ignorowany.

Test obejmuje faktyczne akcje góra/dół, kontrakty obu modeli Qt, zachowanie indeksów drużyny i zawodnika, kolejność danych, zaznaczenie i sto kolejnych przesunięć bez resetu. Pomiar porównuje sto ruchów z setką pełnych przebudów składów. Lokalny launcher `qmlprofiler` nie ma docelowego programu `/usr/lib/qt5/bin/qmlprofiler`, dlatego pomiar dotyczy bezpośrednio modeli i sygnałów, a nie śladu profilera QML.

## Pozycja wyjściowa lucky losera

W „Największym awansie” pozycja po poprzedniej serii jest ograniczona do limitu obecnej serii. Przykład: zawodnik wszedł z 40. miejsca jako LL, a limit wynosi 30 — awans na 10. miejsce oznacza +20, nie +30. Liczba pozostałych skoczków nadal koryguje bieżącą pozycję, żeby sam fakt oczekiwania rywali nie generował awansu. Dane wyników i faktyczne miejsca poprzedniej serii pozostają niezmienione; korekta dotyczy wyłącznie tej statystyki.

## Podgląd rozegranej klasyfikacji

W klasyfikacji z rozegranymi zawodami można przełączać wszystkie sposoby naliczania punktów i oglądać ich punktację. Sam wybór sposobu nie jest zmianą konfiguracji. Nadal zablokowane są edycja punktów, nazwy sposobu, dodawanie i usuwanie sposobów oraz pozostałe ustawienia wpływające na naliczanie. Przełączenie na sposób dodatkowy nie odblokowuje przycisku usunięcia. Nazwę całej klasyfikacji nadal można bezpiecznie zmienić.

Test przełącza domyślny i dodatkowy sposób o innej punktacji po rozegraniu konkursu, sprawdza widoczne wartości, blokady pól i niezmienność zapisanych opcji.

## Weryfikacja

- `scripts/test-quick-ui.sh --team-setup-compact-only` — PASS, w tym kontrakty modeli, zachowanie indeksów i brak resetów przy kolejnych przesunięciach. Wspólne regresje obliczeń sprawdzają przypadek 40. miejsca LL i limitu 30 oraz uwzględnienie pozostałych zawodników.
- `scripts/test-quick-ui.sh --workflow-only` — PASS, w tym rzeczywiste kliknięcia sposobów w zablokowanej klasyfikacji, różna punktacja sposobów, blokady edycji/usuwania i zachowanie konfiguracji. Test obejmuje też dotychczasowe kalendarze, cofanie, masowe zmiany i blokady historii.
- `sj-sim-ui-smoke --classifications-only` — PASS, w tym naliczanie klasyfikacji i wkład kraju.
- `make -C build-widgets-compat -j2` — PASS.
- Kontrolny pomiar z aktywnymi testerami kontraktów modeli: 100 ruchów około 1,1 ms; 100 pełnych przebudów około 1218 ms. Te liczby opisują pracę modeli w tym teście, nie czas reakcji całej aplikacji ani deklarowaną poprawę FPS.

Testy używają izolowanych danych. Qt offscreen nadal zgłasza ostrzeżenia środowiska i przejściowe komunikaty `Polish loop detected` w teście zmiany rozmiarów kalendarza, obecne także przed tymi trzema poprawkami.
