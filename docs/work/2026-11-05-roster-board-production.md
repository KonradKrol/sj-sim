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

Nowy ekran list NIE JEST jeszcze zaimplementowany. Punkty 2–10 raportu
NIE SĄ jeszcze naprawione. Nie deklarować zakończenia tych prac.
Pracować bez subagentów, poza main; osobne commity i push po każdym.
