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

Punkt 3 raportu naprawiony: aktywny kalendarz musi należeć do własnego
sezonu. Źródła awansu, serie próbne, treningi i przypisania klasyfikacji
muszą należeć do kalendarza wydarzenia. Kontrola przed utworzeniem grafu.
Regresje kalendarza z obcego sezonu i źródła awansu z obcego kalendarza
PASS (--production-persistence-only, /tmp/sj-production-ownership.log).

Punkt 6 raportu naprawiony: błędny workspace laboratorium jest chroniony
przed autosave i destruktorem. Widoczny stan odzyskiwania, jawny zapis nowego
obszaru najpierw zachowuje dokładną kopię uszkodzonego pliku. Błędy zapisu
są publikowane w komunikacie. Test otwarcia/edycji/zamknięcia zachowuje bajty
oryginału; test jawnego odzyskania sprawdza niezależną kopię. Core laboratorium
PASS (/tmp/sj-lab-recovery-test.log). EN/CS uzupełnione.
Punkt 7 w trakcie: niezależne rewizje sezonu i ponawianie zapisu po błędzie;
sprawdzić nową regresję przed commitem.

Punkt 7 raportu naprawiony: sezon ma rewizję zmian niezależną od timera;
nieudany zapis zachowuje dirty i planuje ponowną próbę. Przed rozpoczęciem
konkursu i zamknięciem sezonu błąd zapisu pozostawia sezon otwarty.
Zapis ręczny i zapis po konkursie korzystają ze wspólnej ścieżki.
Regresja wymuszonego błędu grafu + nieaktywny timer: dirty zachowany,
timer ponowiony. PASS --production-persistence-only
(/tmp/sj-autosave-revision-test.log).

Punkt 5 raportu naprawiony: BalanceRuntime rozróżnia brak profilu od błędu
odczytu/formatu. Udostępnia lastError, zgłasza użycie standardowego balansu
jednym komunikatem na wersję uszkodzonego pliku oraz stałym ostrzeżeniem
w labie z przywróceniem standardu. Nie zmienia pliku przy odczycie.
Cache odświeża się po zmianie metadanych pliku. Test złego aktywnego profilu,
zachowania jego bajtów i jawnego resetu PASS
(--balance-lab-core-only, /tmp/sj-active-balance-test.log).

Punkt 1 raportu domknięty: trzy typy presetów odrzucają niebezpieczne nazwy
z czytelnym komunikatem. Trwała regresja sprawdza slash/backslash, ścieżkę
absolutną, pustą nazwę, znaki niedozwolone, poprawną polską nazwę i symlink.
PASS --production-persistence-only (/tmp/sj-preset-security-test.log).
Stan raportu: 1–7 naprawione i testowane, 8–10 jeszcze do realizacji.
