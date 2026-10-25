# Edytor skoczków i zachowanie przewinięcia wyników

## Edytor skoczków

- Pojedyncze kliknięcie zawodnika otwiera jego dane w istniejącym edytorze. Nie zmieniono procedury zapisywania danych.
- Ctrl/Shift przy klikaniu nadal służą zaznaczaniu; nie przełączają edytowanego zawodnika.
- Usunięto z widoku listy przycisk „Otwórz” i menu przepełnienia.
- Dodawanie ma jeden widoczny przycisk „+”; nie powiela się w zestawie akcji.
- „Zaznacz wszystkie”, „Usuń”, „W górę” i „W dół” są widoczne bezpośrednio. Pasek zawija przyciski przy małej szerokości. Zachowano wyszukiwanie i czyszczenie zaznaczenia.
- Zachowanie jest włączone dla katalogu zawodników bazy danych. Pozostałe katalogi zachowują dotychczasowy sposób otwierania.

## Wyniki konkursu

Adapter tabeli resetuje model po przebudowaniu wyników, co resetowało ListView do początku. Widok zapamiętuje teraz pionowe przesunięcie przed resetem i przywraca je po aktualizacji, ograniczając do dostępnej długości listy. Kilka resetów zgrupowanych w jednym przebiegu zachowuje pierwotną pozycję.

Automatyczne zmiany kursora wynikające z odświeżenia modelu nie przewijają tabeli wyników nawet wtedy, gdy ma fokus. Jawna nawigacja klawiszami nadal odsłania wskazany wiersz. Lista startowa śledząca kolejnego zawodnika pozostaje przy dotychczasowym zachowaniu. Zmiana dotyczy prostych tabel wyników, w tym widoku drużynowego i KO.

## Weryfikacja

- `--jumper-catalog-only`: prawdziwe pojedyncze kliknięcie w drugiego zawodnika, otwarty poprawny edytor, bezpośrednie akcje, brak menu i podwójnego dodawania; rendery obu motywów, małych okien i dużego tekstu.
- `--result-scroll-only`: rzeczywiste konkursy indywidualny, KO i drużynowy. Po 12 skokach test ręcznie przewija wyniki, nadaje tabeli fokus, wykonuje kolejny skok i wymaga zachowania pionowego przesunięcia.
- Oba zestawy zakończyły się PASS. Nie wykryto ostrzeżeń rekurencyjnych layoutów ani pętli bindingów w tych przebiegach.

Pełny zestaw `sj-sim-ui-smoke` także zakończył się PASS: 399 responsywnych renderów, bez ostrzeżeń o rekurencyjnych layoutach i pętlach bindingów.
