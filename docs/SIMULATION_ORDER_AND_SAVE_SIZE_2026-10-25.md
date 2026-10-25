# Kolejność symulacji i mniejsze pliki zapisu — 25.10.2026

## Stała kolejność

Kolejność listy symulacji jest zapamiętywana w ustawieniach aplikacji pod kluczem `simulationSaves/order`. Wcześniej nowe symulacje trafiały na wybraną pozycję, ale po uruchomieniu gry pliki były ponownie wyliczane według nazw. Teraz wczytana lista odtwarza zapamiętaną kolejność.

Dodanie i usunięcie symulacji aktualizuje kolejność. Pliki dodane z zewnątrz pojawiają się na końcu; nie przestawiają już znanych pozycji. Pierwsze uruchomienie bez zapisanej kolejności zachowuje kolejność początkowego odczytu. Nieudane wczytanie nie nadpisuje zapamiętanej listy częściowymi danymi. Nazwy identyfikują pliki tak samo jak w dotychczasowym loaderze. Zewnętrzna zmiana nazwy pliku jest traktowana jak nowy plik.

To ustawienie lokalnego interfejsu; nie zmienia zawartości ani schematu zapisów symulacji.

## Mniejsze zapisy bez zmiany danych

- Nowe symulacje domyślnie zapisują kompaktowy JSON, korzystając z istniejącego mechanizmu. Tak samo wczytywane są pliki bez określonej preferencji formatowania.
- Dotychczasowe jawne ustawienie czytelnego formatu pozostaje respektowane. W tym trybie cztery spacje wcięcia zastępuje tabulator. Zmiana obejmuje tylko początek wiersza; nie zmienia spacji ani znaków w danych tekstowych.
- Pozostają wszystkie pola, identyfikatory, wartości liczbowe, wyniki, historie i powiązania. Format nadal jest zwykłym JSON-em; nie wprowadzono archiwów ani nowej zależności do kompresji.
- Stare pliki nie są przepisywane podczas samego wczytywania. Zmniejszenie następuje przy kolejnym zapisie. Zachowano atomowy zapis przez `QSaveFile`.

## Weryfikacja

`--save-storage-only`: PASS. Sprawdzono odtworzenie kolejności przy innym porządku nazw, wstawianie na wybraną pozycję, usuwanie, nowe pliki i brak wcześniejszych ustawień. Oba warianty JSON porównano pole po polu z pełnym obiektem przed zapisem i ponownie wczytano. Test obejmuje tekst ze spacjami, tabulatorami, znakami nowej linii i polskimi literami; także wcześniejsze wcięcia spacjami oraz respektowanie jawnej preferencji formatu.

Mała izolowana próbka: czytelny zapis 955 zamiast 1357 bajtów, kompaktowy 741 zamiast 1356 bajtów. To pomiar tej próbki; oszczędność w rzeczywistym sezonie zależy od jego zawartości.

`--audit-only`: PASS, w tym tworzenie symulacji i dotychczasowe regresje danych. `make -C build-widgets-compat -j2`: PASS. Testy nie zmieniają danych użytkownika.
