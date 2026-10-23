# Windows UI i kody FIS — 23.10.2026

## Przełączanie stron

Kontrolery dawnych okien Widgets pozostają przy swoich właścicielach, a ich widoki Quick powstają bezpośrednio w stosie stron głównego okna. Zdarzenie `Polish` ustawia `WA_DontShowOnScreen` przed pierwszym natywnym `Show`. Usunięto zmianę rodzica na widoczny widget, ponowne `show()` i przesuwanie na `(-32000,-32000)`. Główne okno odzyskuje aktywację tylko wtedy, gdy rzeczywiście ją utraciło. Zachowano obsługę lokalnych pętli `QDialog::exec()` i zabezpieczenie przed ponownym `Show` podczas inicjalizacji.

Opcje konkursu „Przebieg”, „Warunki” i „Skok” są sprawdzane przez kliknięcia w rzeczywistym widoku, przy szerokościach 1240 i 520. Test wymaga otwartego panelu z właściwą sekcją, zawartości w obrębie okna, zmiany wyrenderowanej klatki i poprawnego zamknięcia.

## Kody krajów

`CountryCodes` normalizuje kody do FIS w modelach zawodników, skoczni, drużyn, krajów i historycznych klasyfikacji. Przykłady: `DEU → GER`, `CHE → SUI`, `SVN → SLO`, `HRV → CRO`. Wyszukiwanie przyjmuje oba warianty. Flagi nadal korzystają z istniejących plików SVG o nazwach ISO, przez odwrotne mapowanie.

55 różniących się kodów zweryfikowano z [oficjalnym wykazem federacji FIS](https://www.fis-ski.com/DB/v2/national-ski-and-snowboard-associations?mi=menu-nsa-members&rt=inside-fis). Nie zastępujemy tego wykazu ogólną listą olimpijską: np. FIS używa `TTO`. Kody niestandardowe lub bez odpowiednika w wykazie pozostają dostępne; wejście jest przycinane i zamieniane na wielkie litery.

Istniejące pliki baz i zapisów nie są masowo nadpisywane. Normalizacja następuje przy odczycie i edycji obiektów; następny zwykły zapis używa FIS. Nazwy pól JSON, identyfikatory, powiązania drużyn i zapisane punkty pozostają zgodne. Pole `alpha3` w JSON krajów zachowuje nazwę dla zgodności, a nagłówek edytora brzmi „Kod FIS”.

## Weryfikacja

Windows x64, Qt 6.8.3 / MSVC 2022, Release:

- Build aplikacji i testów zakończony poprawnie.
- `--navigation-only`: powtórny `Show`, zagnieżdżona nawigacja, powrót i zachowanie stanu.
- `--home-launch-only`: osiem ekranów menu; kontrolery są ukryte przed pierwszym `Show`, Windows `IsWindowVisible` nie wykazuje osobnych widocznych okien, a treść pozostaje w głównym oknie.
- `--race-options-only`: trzy opcje konkursu, dwie szerokości, otwieranie i zamykanie.
- `--bulk-only`: indywidualne, drużynowe i KO oraz zbiorcze akcje konkursu.
- Regresje backendu: stare ISO, nowe FIS, flagi, identyfikatory, powiązania zawodników i historyczne punkty; także dotychczasowe testy punktacji i danych.
- Wdrożone `build-win/release/sj-sim.exe --quick-competition` uruchomiono bez deweloperskiego PATH Qt i zamknięto normalnie z kodem 0.

Powyższe testy GUI wykonano w natywnym oknie Windows. Testy pracują na kopiach danych. Zwykłe exe i dane użytkownika pozostają w dotychczasowym katalogu; skrót „Sj.Sim QML” nadal wskazuje właściwe exe.

Pełny zestaw GUI nie jest deklarowany jako zaliczony. Dodatkowy `--workflow-only` po przewinięciu formularza zatrzymuje się na asercji `choosing custom rules opens their editor directly` w kalendarzu sezonu. Wcześniejszy zakres `--ux-only` zatrzymał się na akcji `rosterPresetRemove`. Są to osobne ścieżki do dalszego sprawdzenia; nie stanowią dowodu awarii trzech paneli opcji konkursu, które przeszły dedykowany test. Testy kliknięć przewijają teraz pola kółkiem myszy, ponieważ Windows ogranicza żądany rozmiar okna do obszaru monitora.
