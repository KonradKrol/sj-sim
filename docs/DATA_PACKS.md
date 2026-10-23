# Paczki i dane urodzenia

W edytorze bazy danych wybierz **Aktywną paczkę**, a przyciskiem **Paczka…** otwórz jej nazwę, ustawienia oraz import i eksport. Każdy z trzech rodzajów ma osobny wybór: skoczkowie, skocznie oraz zasady konkursów. Zakładki zasad i presetów punktowych korzystają z tej samej paczki. Wybór określa dane dostępne przy tworzeniu nowych konkursów i sezonów; istniejące zapisy sezonów zachowują własne kopie danych.

Możesz zmieniać nazwę, tworzyć puste paczki, importować i eksportować je jako JSON. **Eksportuj / udostępnij** zapisuje jeden plik, który druga osoba może zaimportować przyciskiem **Importuj paczkę**. Import dodaje osobną paczkę, również przy ponownym imporcie tego samego pliku. Nie zastępuje istniejących paczek. Odwołania do własnych zdjęć skoczków pozostają nazwami plików w `jumpersImages`; zdjęcia należy przekazać osobno.

Każda paczka skoczków określa format:

- **Data urodzenia**: `RRRR-MM-DD`. Przy niepełnych danych można zachować sam rok urodzenia.
- **Wiek rocznikowy**: wiek oraz obowiązkowy rok odniesienia paczki. Wiek 26 w roku 2026 oznacza rocznik 2000, niezależnie od dnia urodzin.

Zmiana roku odniesienia przelicza wyświetlany wiek, zachowując rok urodzenia. Zmiana formatu zachowuje znaną pełną datę; sam wiek nie powoduje wymyślenia dnia ani miesiąca urodzenia. Dane mogą pozostawać nieznane, również dla zawodników z dotychczasowej bazy.

Przełączenie paczki zachowuje otwarte dane edytora w pamięci. Przy wyjściu z edytora **Tak** zapisuje wszystkie paczki i aktywny wybór, **Nie** przywraca stan sprzed wejścia, a **Anuluj** pozostawia edytor otwarty. Eksport zapisuje bieżącą zawartość wskazanej paczki niezależnie od późniejszego zatwierdzenia zmian w bazie.

## Format i zgodność

Przenośny dokument ma `format: "sj-sim-pack"`, `version: 1`, `id`, `name`, `kind` (`jumpers`, `hills` albo `rules`). Paczka skoczków ma `age-format` (`birth-date` albo `age`), `reference-year` oraz tablicę `jumpers`. Wiek jest eksportowany w polu `age`, a kanoniczny `birth-year` pozwala przeliczać go w kolejnych latach; opcjonalny `birth-date` przechowuje dokładną datę. Paczka skoczni ma `hills`. Paczka zasad zawiera zarówno `competitionsRules`, jak i `pointsForPlacesPresets`.

Katalog `userData/GlobalDatabase/packs.json` zawiera wszystkie paczki i aktywne identyfikatory. Jest zapisywany atomowo przez `QSaveFile`. Bez katalogu dotychczasowe listy zostają paczkami domyślnymi; pliki starej bazy pozostają obsługiwane. Dane urodzenia są też częścią serializacji zawodnika w zapisach sezonów. Nieprawidłowy import jest odrzucany przed zmianą bazy, a uszkodzony istniejący katalog nie zostanie automatycznie nadpisany.

## Weryfikacja na Windowsie — 23.10.2026

Qt 6.8.3, MSVC 2022, Release:

- Testy backendu: wiek rocznikowy, zachowanie pełnej daty, brakujące dane, wymagany rok odniesienia, sprzeczne wartości i błędne identyfikatory, wszystkie trzy rodzaje paczek, presety z ułamkowymi punktami, przełączanie oraz zapis i ponowne wczytanie katalogu. Zachowana zgodność ze starymi zerowymi identyfikatorami.
- Natywny `--packs-only`: konwersja formatów, edycja wieku, zmiana roku odniesienia, eksport i import przez wybór pliku, utworzenie pustej paczki przyciskiem, wspólna paczka zasad i presetów oraz zapis, odrzucenie zmian i anulowanie wyjścia.
- Natywne regresje `--navigation-only`, `--round-points-only` i `--single-jumps-only`: kod zakończenia 0.
- Renderowanie panelu w jasnym i ciemnym wyglądzie przy 1440, 1024, 520 i 360 pikselach oraz z powiększonym tekstem.
- Właściwy `build-win/release/sj-sim.exe` uruchomiony z plikami wdrożenia; okno otwarte i normalnie zamknięte z kodem 0.
