# Wspólny edytor list i raport produkcyjny — praca w toku

Uzgodnienia właściciela:
- Nowy ekran zajmujący cały obszar pracy, wejścia z list sezonu i przeglądu.
- Wspólna biblioteka wszystkich skoczków; drag and drop do wielu list.
- Dwa układy: kraje → listy oraz listy → kraje.
- Chipy wybierają widoczne listy; odznaczenie nie usuwa listy ani składu.
- Przełącznik wyłączności dotyczy tylko wybranych list. Przeciągnięcie
  przenosi zawodnika z innych wybranych list, nie narusza ukrytych.
- Jawne Zastosuj, stan niezapisanych zmian, ostrzeżenie przy opuszczaniu.
- Kompaktowy, czytelny UI: hierarchia, kontrast, clarity, bez szumu.
- Wcześniejsza propozycja automatycznych składów wymaga pogodzenia z tym
  ekranem; nowsza specyfikacja wspólnego edytora ma pierwszeństwo.

Punkty wejścia: SeasonRosterChoice.qml, SeasonRostersWorkspace.qml,
SimulationSaveManagerWindow (rosterEntries, setRosterMembers, publishRosters),
QuickRoster, SaveJumpersList. Wersja robocza musi być niezależna od danych
sezonu aż do atomowego zastosowania całego zestawu.

Raport do realizacji: docs/reviews/2026-11-04-next-ten-production-issues.md.
Dziesięć punktów w tym raporcie nadal wymaga sprawdzenia/napraw.
Rozpoczęto punkt 1: wspólna kontrola nazw plików presetów w AtomicJsonFile,
wykorzystana w punktacji, kalendarzach i ważności skoków. Próba na katalogu
QTemporaryDir potwierdziła odrzucanie wyjścia poza katalog i symlinków.
Pozostaje trwały test regresji i jasna informacja o odrzuconej nazwie w UI.
Nie uznawać całego punktu 1 za skończony.

Wspólny ekran list jest zaimplementowany: RosterBoard.qml,
RosterBoardLane.qml, RosterBoardAthlete.qml; wersja robocza i transakcja
w SimulationSaveManagerWindow. Wejścia „Rozdziel” w przeglądzie i
„Rozdziel zawodników” w sekcji list. Dwa układy, chipy, przenoszenie
wyłącznie między wybranymi listami, przeciąganie, przewijanie krawędziowe,
alternatywa wybór + przycisk, jawne Zastosuj, odrzucanie i ostrzeżenie
przy zamykaniu sezonu. Wersja robocza przetrwa przełączanie sekcji sezonu.
Konflikt z zewnętrzną edycją lub usuniętym skoczkiem blokuje całą transakcję.
Limity krajów i aktywna lista pozostają zachowane.

Build i --roster-workspace-only PASS (600 zawodników, prawdziwe zdarzenia
myszy, zmiana układu, konflikt transakcji, ukryta lista, odrzucanie).
Sprawdzone zrzuty /tmp/sj-roster-board.png i /tmp/sj-roster-board-lists.png.
EN/CS uzupełnione, check-translations.py i git diff --check PASS.
Nie deklarować jeszcze zakończenia 10 punktów raportu.
Punkty 2 i 4 są rozpoczęte w drzewie roboczym; wymagają własnych testów
oraz osobnych commitów. Pozostałe 3 i 5–10 nadal do wykonania.
Pracować bez subagentów, poza main; osobne commity i push po każdym.

2026-11-05: punkt 2 raportu naprawiony: wymagane kolekcje zapisu, sezonu,
kalendarza, grup KO i wyników są walidowane przed deserializacją. Nie-array,
brak pola i niewłaściwe elementy są odrzucane; referencje muszą być tekstem.
Regresje skalarnych/null/brakujących kolekcji i poprawnego roundtrip PASS
(--production-persistence-only, /tmp/sj-production-validation.log).

Punkt 4 raportu naprawiony: SeasonJumpersListModel i SeasonHillsListModel
mutują kontener między begin/endRemoveRows, odrzucają puste insertRows
(wymagane insertItem), resetują model przy podmianie kontenera i bronią
odczytu poza zakresem. Usunięto podwójne kasowanie z DatabaseItemsListView.
Test QAbstractItemModelTester obu modeli + insert/remove/invalid PASS
(--production-persistence-only, /tmp/sj-production-models.log).
