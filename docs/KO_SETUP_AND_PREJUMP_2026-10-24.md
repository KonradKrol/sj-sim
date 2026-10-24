# Dobór grup KO i uproszczenie karty przed skokiem

## Grupy KO przed konkursem

- Osobna sekcja „Grupy KO” zajmuje dostępny obszar pracy. Zamiast odwzorowania zagnieżdżonych kontrolek Qt Widgets jest jeden wybór metody oraz przewijana siatka kart grup.
- Metody: Klasycznie, Klasycznie · większe grupy, Losowo z koszyków, Losowo. Zmiana metody od razu dobiera grupy. Dodatkowy przycisk ponownego losowania pojawia się tylko dla metod losowych i już istniejących grup.
- Każde miejsce w grupie jest zwykłym wyborem zawodnika z flagą. Wybranie zawodnika zajmującego inne miejsce zamienia oba miejsca; nie powstaje duplikat i nie otwiera się dialog.
- Zmiana zawodnika zachowuje pozycję przewijania. Liczba kolumn dostosowuje się do szerokości okna. Główna akcja rozpoczęcia konkursu pozostaje w stałym dolnym pasku.
- Przy braku grup jest jedna akcja ich doboru. Przy braku zawodników ekran kieruje do listy startowej zamiast oferować puste losowanie.
- Lista opcji i przypisanie używają tożsamości zawodników z aktualnej listy, również gdy szablony skoczków mają jednakowe puste UUID. Odrzucane są nieaktualne grupy, nieznani zawodnicy i nieprawidłowe miejsca.
- Aktywna zakładka pozostaje widoczna również po zmniejszeniu szerokości okna.
- Komponenty Qt Widgets pozostają w projekcie; nowy widok jest alternatywą Qt Quick dla przygotowania konkursu.

## Kolejność drużyny

- Usunięto widoczny przycisk „Przenieś” i formularz pozycji docelowej w składzie drużyny.
- Kliknięcie uchwytu nie otwiera już formularza. Przeciągnięcie zawodnika na innego zamienia ich miejscami, także między składem podstawowym i rezerwą.
- Krótka informacja wyjaśnia zamianę. Zwykłe listy startowe zachowują dotychczasowy mechanizm przenoszenia.

## Przed skokiem

- Jest tylko jeden komunikat punktowy: „Ma przewagę X pkt” albo „Traci X pkt”. Usunięto dodatkowe różnice do grupy KO i aktualnej sumy lidera.
- Lider jest wybierany z **aktualnego wyniku konkursu**, zgodnie z kolejnością rankingu i obsługą remisów.
- Dopiero potem obliczana jest różnica między punktami zawodnika i tego lidera **sprzed bieżącej serii**. Skoki oddane przez lidera w bieżącej serii nie są doliczane do tej różnicy.
- Wagi bieżącej serii stosowane są do punktów historycznych obu zawodników. Po resecie punktów oraz w pierwszej serii komunikat jest pomijany. Ta sama reguła obowiązuje w KO i drużynówkach.
- Regresja obejmuje zmianę lidera: zawodnik z niższym wcześniejszym wynikiem obejmuje prowadzenie po mocnym skoku; porównanie używa właśnie jego wcześniejszych punktów, a nie poprzedniego lidera ani nowej sumy ze skokiem więcej.

## Sprawdzenia

- Wielokrotna zmiana doboru, koszyki i nieparzyste pola startowe.
- Wybranie zawodnika w rzeczywistym dropdownie QML i weryfikacja obu zamienionych miejsc w konfiguracji.
- Zrzuty responsywnego widoku KO: 1440, 1024, 520 i 360 px, oba motywy.
- Brak przycisku i formularza przenoszenia w składzie drużyny.
- Ważone punkty, zmiana aktualnego lidera, pominięcie po resecie oraz tylko jeden komunikat w wieloseryjnym KO.

Wynik końcowy: pełny zestaw regresji PASS, **399 renderów**; osobny scenariusz wieloseryjnego KO PASS. Zmiany zapisano osobno: `c6bc131` (nowy obszar KO), `2f1542b` (pojedyncze porównanie z liderem) i `8ebe631` (usunięcie formularza przenoszenia z drużyny).
