# Konkurs, dialogi i źródła kwalifikacji

Kontynuacja zmian opisanych w `SPACE_AND_LAST_JUMP_2026-10-21.md`. Wersja pozostaje 1.3.0 beta-3; Qt Quick jest domyślnym interfejsem, Widgets pozostaje dostępny.

## Co sprawdzić jako użytkownik

1. **Karta ostatniego skoku:** miejsce ma osobne, kontrastowe oznaczenie. Obok nazwiska jest flaga zawodnika, a pod nim dyskretna strata w punktach do lidera albo „Lider”. W drużynówce miejsce i strata dotyczą całej drużyny. Dane nie pochodzą od kolejnego zawodnika na belce.
2. **Przed skokiem i na żywo:** zawodnik na belce ma miejsce po poprzedniej serii. Podczas prezentacji na żywo karta utrzymuje poprzednie miejsce z podpisem serii, aż do ujawnienia wyniku; wtedy pokazuje nowe miejsce i stratę. W pierwszej serii poprzednie miejsce nie istnieje, więc nie jest wymyślane.
3. **Miejsce na wyniki:** karty mają wysokość wynikającą z zawartości i są wyrównane do góry. Puste miejsce pod krótszą kartą nie rozciąga jej tła. Łączna szerokość obu kart jest ograniczona do 1052 px, ostatni skok do 720 px, zawodnik do 320 px. Tabele wykorzystują pozostałą szerokość i wysokość, również przy 2560 × 1440. Na wąskim ekranie karty pozostają w jednej kolumnie.
4. **Opcje konkursu:** przyciski „Przebieg”, „Warunki”, „Skok” otwierają od razu właściwą kategorię. Są w dotychczasowym pasku sterowania, bez dodatkowego rzędu na desktopie. Przełącznik „Na żywo” ma krótszy podpis.
5. **Dialogi:** sprawdź informację o rekordzie skoczni, potwierdzenia, wybór presetu i liczbę skoków do symulacji. Długi tytuł i zawartość przewijają się, a przyciski pozostają na dole. Przyciski mogą przejść do kolejnego rzędu. Rozmiar jest ponownie dopasowywany po natywnych zmianach geometrii i po zmniejszeniu głównego okna.
6. **Pola liczbowe:** szybko klikaj „+” i „−” przy belce i liczbie skoków. Każde naciśnięcie działa od razu. Przytrzymanie uruchamia powtarzanie po 250 ms, następnie co 40 ms. Granice wartości i kroki ułamkowe są zachowane.
7. **Kalendarz → powiązania:** „Kwalifikacja na podstawie konkursu” pozwala wybrać wyłącznie wcześniejsze wydarzenie tego samego typu (indywidualne/drużynowe). Konkurs nr 1 nie ma wcześniejszego źródła; nr N może korzystać najwyżej z nr N−1. Dla wielu zaznaczonych konkursów lista obejmuje wspólne dozwolone źródła. Przesunięcie źródła za zależny konkurs jest blokowane. Powiązania zachowują tożsamość obiektów, nie zapamiętany numer wiersza. Nieprawidłowe przyszłe i własne źródła są usuwane podczas naprawy lub wczytywania kalendarza.

## Szczegóły napraw

`advancementCompetition` w silniku oznacza źródło kwalifikacji. Qt Quick miał odwrócone oba porównania: budowę listy wyboru i kontrolę przestawiania. Naprawa zachowuje istniejący format zapisu i kierunek relacji silnika.

Miejsce i strata ostatniego skoku są zapamiętywane przy wypełnianiu oryginalnego panelu wyniku. Adapter nie odczytuje później surowego wskaźnika do elementu wektora, który może zostać unieważniony przez dodanie lub sortowanie wyników. Testy nowego pola ujawniły ten problem w początkowej implementacji i umożliwiły jego usunięcie przed commitem.

Miejsce po poprzedniej serii jest liczone z odpowiedniego prefiksu skoków, bez zmieniania wyników. Uwzględnia remisy (1, 1, 3); w drużynówce prefiks obejmuje pełną poprzednią serię wszystkich zawodników drużyny.

## Weryfikacja

- Pełny `scripts/test-quick-ui.sh`: kod 0, **343 rendery**, motywy jasny/ciemny, rozmiary od 360 × 480 do 2560 × 1440 oraz warianty powiększonego tekstu.
- `--competitions-only`: kod 0 po końcowej zmianie sposobu odczytu miejsca; pełne konkursy indywidualne, KO i drużynowe oraz analiza sezonu. Test obejmuje poprzednie miejsce drużyny na belce.
- `--ux-only`: kod 0 po końcowej korekcie zawijania etykiet metryk; sprawdza również rzeczywisty skok w drugiej serii i zmianę poprzedniego miejsca na nowe.
- `interactionRegressions` (także osobne `--interaction-only`): szybkie serie 10 naciśnięć w obu kierunkach, przytrzymanie, kroki ułamkowe i limit, długie komunikaty z wymuszoną natywną zmianą rozmiaru oraz zmniejszeniem głównego okna; sprawdzenie widoczności stopki i działającego przycisku OK.
- Testy kalendarza: brak źródeł dla pierwszego konkursu, tylko wcześniejsze źródła dla kolejnych, nieprawidłowy indeks, blokada złej kolejności, cofanie i naprawa błędnych powiązań.
- Testy rankingu prefiksu skoków: remisy 1, 1, 3, zmiana lidera w kolejnej serii i nieistniejąca seria. Końcowe miejsce i strata są porównywane z wynikami zawodnika/drużyny w rzeczywistych konkursach.
- Końcowa kompilacja Widgets-only: kod 0, bez zależności Qt Quick/QML.
- Brak nowych brakujących angielskich tłumaczeń Quick; `git diff --check` przechodzi.

Automatyczne sprawdzenia używają Qt 5.15.3 i renderera programowego offscreen; nie obejmują wszystkich menedżerów okien ani sterowników GPU. Zrzuty: `build-quick-ui/ui-renders/`.
