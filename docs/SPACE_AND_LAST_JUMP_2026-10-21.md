# Wykorzystanie miejsca i pełne podsumowanie skoku

Gałąź `feature/qml-competition-config`; bez zmian na `main` i bez zmiany algorytmu symulacji.

## Listy jako główny obszar roboczy

- Edytor bazy: zawodnicy, skocznie, zasady i presety punktów wykorzystują dostępną wysokość. Lista rośnie również po przekroczeniu dawnego limitu wysokości. Ukryty edytor nie rezerwuje pustej kolumny.
- Otwarty formularz ma własne przewijanie. Na desktopie lista i formularz zajmują około 38% / 62% szerokości; obszerniejszy formularz nie wydłuża listy poza okno. Przy węższym oknie pojawiają się osobne widoki listy i edycji.
- Ten sam układ obejmuje sezonowe katalogi zawodników, skoczni, zasad, klasyfikacji i list zawodników. Zachowano akcje strony, w tym statystyki zawodnika, oraz oryginalne operacje dodawania, usuwania, kolejności i edycji.
- Ekran wyboru zapisów również oddaje pozostałą wysokość liście. Krótkie przyciski tworzą zwarty rząd, zamiast konkurować z listą o miejsce.
- Obsługę danych nadal zapewniają dotychczasowe modele i kontrolery Widgets. Zmiana dotyczy prezentacji Qt Quick; oryginalne formularze i nawigacja do edytorów list pozostają zachowane.

## Ostatni skok bez otwierania panelu

Na ekranie konkursu są widoczne: zawodnik, miejsce po skoku, odległość, punkty skoku, pięć not, suma punktów za noty, wiatr, belka, lądowanie, rekompensata wiatrowa, rekompensata za belkę, łączna rekompensata, miejsce w serii i suma punktów zawodnika. Dla drużyn dochodzą miejsca w grupie i konkursie, jeśli są dostępne. DSQ i informacja o 95% HS pojawiają się w tej samej karcie. Opcjonalne informacje silnika wskazane w ustawieniach także trafiają do podsumowania.

Noty są osobnymi polami. Jedna najniższa i jedna najwyższa są przekreślone; remisy nie powodują odrzucenia wszystkich jednakowych ocen. Pozostałe trzy mają wyraźne tło. Oznaczenie odrzuconej noty jest dostępne także dla czytnika ekranu.

Wiatr i łączna rekompensata mają niezależne oznaczenia: dodatnia wartość — zielony i znak plus; ujemna — czerwony i znak minus; zero — neutralny kolor. Kolor uwzględnia precyzję prezentowanej liczby. Punkty za wiatr mogą mieć przeciwny znak niż sam wiatr — nie są ze sobą mylone.

W małym oknie ostatni skok jest przed panelem zawodnika na belce. Dane mieszczą się w zwartej siatce; wartości nie są ucinane wielokropkiem. W układzie desktopowym karta otrzymuje większą część szerokości. W tabelach konkursu zmniejszono drugorzędne nagłówki i kontrolki, a miejsce na poziomy pasek przewijania jest rezerwowane tylko wtedy, gdy jest potrzebny. Wyniki i lista startowa zachowują wspólny obszar roboczy.

Prezentacja na żywo nadal ujawnia odległość, lądowanie, noty i wynik etapami. Dodatkowe dane nie ujawniają przedwcześnie końcowego wyniku. Oryginalny panel zachowuje historię serii i dodatkowe operacje.

## Jak sprawdzić ręcznie

1. Otwórz każdą kategorię bazy. Powiększ okno w pionie: lista powinna rosnąć. Otwórz element: formularz przewija się obok listy. Zmniejsz szerokość: przełączaj listę i edycję, otwórz kolejny element.
2. W sezonie sprawdź zawodników, skocznie, zasady, klasyfikacje i zapisane listy. Sprawdź również dostęp do statystyk zawodnika. W wyborze zapisów zwróć uwagę na wysokość listy i zwarty pasek przycisków.
3. Oddaj skok normalnie i na żywo. Obejrzyj kartę bez otwierania opcji. Sprawdź noty, rekompensaty, belkę i lądowanie. Powtórz w konkursie drużynowym i KO.
4. Sprawdź dodatni, zerowy i ujemny wiatr oraz dodatnią i ujemną sumę rekompensat. Zmień motyw i rozmiar okna, także do 360 × 480.

## Walidacja

- Pełny `scripts/test-quick-ui.sh`: kod 0, **339 renderów** w jasnym i ciemnym motywie, rozmiary od 1440 × 900 do 360 × 480 oraz dodatkowe warianty powiększonego tekstu.
- Testy potwierdzają wzrost list przy zwiększaniu wysokości okna, brak pustej kolumny po ukrytym edytorze i utrzymanie listy w obszarze okna po otwarciu długiego formularza.
- Karta po rzeczywistym skoku jest sprawdzana także przy najmniejszym oknie; noty zachowują pięć wartości i dokładnie dwie odrzucone. Test kolorów obejmuje przeciwne znaki wiatru i rekompensaty oraz zero, aby potwierdzić niezależność obu oznaczeń.
- Pełne konkursy indywidualne, KO i drużynowe, w tym etapowe ujawnianie wyniku na żywo, przeszły weryfikację. Końcowy konkurs drużynowy ma dodatkowy pełny przegląd rozmiarów i motywów.
- Wariant Widgets-only: kompilacja kod 0, bez zależności Qt Quick/QML.
- Nowe ciągi Quick mają angielskie tłumaczenia. `git diff --check` nie wykazuje błędów.

Po końcowym pełnym przebiegu skrócono wyłącznie etykiety przycisków wyboru zapisu i ukryto sortowanie krajami w kategoriach, dla których nie miało działania; te zmiany sprawdzono ponownie testem `--space-only`.

Nowe testy znajdują się w `tests/quick-ui-smoke.cpp`: `spaceRegressions` oraz scenariusze konkursowe i sprawdzenia geometrii. Dostępne są `--space-only`, `--ux-only`, `--team-only` i `--competitions-only`. Zrzuty są w `build-quick-ui/ui-renders/`. Weryfikacja automatyczna korzysta z Qt 5.15.3 i renderera programowego offscreen; nie obejmuje wszystkich systemowych menedżerów okien ani sterowników GPU.
