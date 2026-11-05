# Poprawki po raporcie produkcyjnym z 04.11.2026

Raport źródłowy: [next-ten-production-issues](2026-11-04-next-ten-production-issues.md).
Każdy zakres zapisano osobnym commitem na `feature/simulation-v2` i wypchnięto.

1. **Nazwy presetów mogły wyjść poza katalog docelowy.** Wspólny walidator
   odrzuca separatory, ścieżki absolutne, nazwy puste, niedozwolone znaki i
   dowiązania. Dotyczy presetów punktacji, kalendarzy i ważności skoków.
   Odrzucenie nazwy ma komunikat. Regresja sprawdza także poprawną polską nazwę.
2. **Nieprawidłowe kolekcje zapisu stawały się pustymi tablicami.** Typy pól
   wymaganych są sprawdzane przed deserializacją: m.in. sezony, kalendarze,
   konkursy, klasyfikacje, grupy KO i wyniki. Skalar, null, brak pola lub błędny
   element kolekcji nie może zostać później zapisany jako utrata danych.
3. **Aktywny kalendarz mógł pochodzić z obcego sezonu.** Walidacja sprawdza
   przynależność kalendarza oraz lokalne źródła awansu, treningi, serie próbne
   i przypisania klasyfikacji. Istnienie UUID w całym zapisie już nie wystarcza.
4. **Modele list sezonu publikowały usunięcie przed zmianą danych.** Skoczkowie
   i skocznie są usuwani pomiędzy begin/endRemoveRows; wyeliminowano drugie
   usunięcie w widoku. Wstawianie wymaga pełnego obiektu przez insertItem.
   Dodano kontrolę zakresów i reset przy podmianie kontenera. Oba modele
   przechodzą QAbstractItemModelTester.
5. **Uszkodzony profil balansu znikał za cichym powrotem do standardu.** Błąd
   ma jawny stan, komunikat o użyciu standardu i ostrzeżenie w laboratorium
   z możliwością przywrócenia profilu. Sam odczyt nie zmienia pliku.
6. **Otwarcie laboratorium mogło nadpisać uszkodzony workspace.** Autosave
   i destruktor respektują blokadę odczytanego błędnie pliku. Jawne odzyskanie
   najpierw zachowuje jego dokładną kopię. Błąd zapisu jest komunikowany.
7. **Nieudany autosave gubił informację o niezapisanych zmianach.** Sezon ma
   niezależne rewizje zmian i zapisu. Błąd zachowuje dirty oraz ponawia timer.
   Nieudany zapis przed otwarciem konkursu lub zamknięciem sezonu pozostawia
   sezon otwarty. Zapis ręczny korzysta z tej samej ścieżki.
8. **Wyniki klasyfikacji i kalendarze nie miały pełnej własności.** Heapowe
   rekordy są jawnie przejmowane przez ownResult; kopie historii współdzielą
   ich czas życia. Pożyczone rekordy pozostają pożyczone. Usunięto podwójne
   ręczne zwalnianie wirtualnych wyników. Sezon zwalnia swój graf kalendarzy
   raz; kalendarze usunięte z edytora czekają na wygaśnięcie undo/redo.
9. **Lista zapisów wczytywała pełne grafy wszystkich sezonów naraz.** Katalog
   indeksuje nazwy i ścieżki; pełny graf powstaje dopiero po wybraniu otwarcia.
   Niewczytany wpis nie może nadpisać pliku pustym sezonem. Odczyt pojedynczego
   pliku ma limit 256 MiB, także gdy plik rośnie podczas odczytu.
10. **Parametry zamrożonego konkursu nie identyfikowały algorytmu.** Osobna
    rewizja `SimulationV2::modelRevision` jest zapisywana w konkursie.
    Trwający konkurs z wynikami i obcą/brakującą rewizją blokuje wczytanie;
    zakończone archiwa pozostają czytelne. Nie przeliczamy automatycznie części
    rozegranych skoków. Przy zmianie algorytmów trzeba podnieść tę rewizję.

## Weryfikacja

Build Linux/Qt 5.15 i regresje `--production-persistence-only`,
`--roster-workspace-only`, `--balance-lab-core-only`, `--balance-lab-only`,
`--dates-only` przeszły. Ten ostatni uruchamia również backendRegressions,
w tym klasyfikacje, modele, kalendarze, presety, punktację i historię.
Sprawdzone EN/CS i `git diff --check`.

To są testy automatyczne na izolowanych danych, w tym zdarzenia myszy
w Qt Quick. Nie stanowią ręcznego rozegrania sezonu 20 konkursów ani dowodu,
że nie ma innych błędów. Nie wykonano pełnego pomiaru wycieków całej aplikacji.
