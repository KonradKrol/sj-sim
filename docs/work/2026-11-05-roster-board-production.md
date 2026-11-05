# Aktualny stan — 05.11.2026

- Wspólny edytor list: wdrożony, przetestowany, na remote.
- Dziesięć punktów raportu produkcyjnego: wdrożone osobnymi commitami.
  Szczegóły: [raport poprawek](../reviews/2026-11-05-production-fixes.md).
- Kalendarz: większy obrysowany „+”, planowanie w prawym inspektorze
  z widoczną listą, domyślne zasady w menu ustawień, empty state inspektora.
- Każda zmiana rodzaju dobiera domyślne zasady; nowe treningi i próbne
  korzystają z własnych ustawień domyślnych.
- Pojedyncza lista: klik wiersza/checkboxa bezpośrednio przełącza członkostwo,
  bez „Dodaj zaznaczonych”; wysokość listy wypełnia panel.
- Tekst pustego dnia jest rzeczowy: „Brak wydarzeń.”.
- Zmiana domyślnych zasad kwalifikacji: odroczona bezpieczna aktualizacja,
  osłony pustej paczki/indeksu; wielokrotne wybieranie przechodzi test UI.
- Końcowe testy produkcyjne, list, laboratorium i dat przechodzą.
- Gałąź feature/simulation-v2, bez zmian main i bez subagentów.

Poniżej dziennik chronologiczny; wcześniejsze wpisy „w trakcie” opisują
stan w chwili ich sporządzenia, nie aktualną kolejkę.

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

Nowa uwaga właściciela (po bieżących poprawkach): edytor kalendarza —
przycisk „+ KONKURS” ma mieć sam „+”, wciąż dobrze widoczny z dostępną nazwą.
Zmniejszyć liczbę jednocześnie niebieskich głównych przycisków w całym
edytorze (obecnie około 4); ustalić jeden wyraźny priorytet bez wizualnego szumu.
Nowa uwaga właściciela, na koniec: przejścia do szybkiego planowania i
ustawień domyślnych zasad mają nie udawać tabów działających inaczej;
uprościć nawigację, usunąć zaskakujące przesuwanie układu. Zachować kierunek,
ale zastosować klasyczny przewidywalny wzorzec.

Punkt 9 raportu naprawiony: katalog zapisów indeksuje nazwy/ścieżki plików,
nie deserializuje wszystkich sezonów ani nie blokuje na mapped/wait.
Pełny graf powstaje po otwarciu konkretnego zapisu; nieudany odczyt zostawia
wpis i plik. Niewczytane wpisy nie mogą zapisać pustego grafu; zapis zbiorczy
je pomija. Szczegóły przed otwarciem jasno oznaczone. Odczyt ograniczony do
256 MiB, także przy wzroście pliku podczas odczytu. Regresje indeksu,
niezapisywalnego placeholdera, poprawnego otwarcia i zbyt dużego sparse file
PASS --production-persistence-only (/tmp/sj-lazy-saves-test.log).
Na koniec również crash: w sezonie zmiana domyślnych zasad kwalifikacji
z dropdowna natychmiast zamknęła grę. Sprawdzić przy poprawianiu nawigacji
panelu domyślnych zasad, ze szczególnym uwzględnieniem reentrancy/model reset.

Punkt 10 raportu naprawiony: ModelRevision.h definiuje niezależną rewizję
algorytmów (obecnie 1). CompetitionInfo zapisuje rewizję; ukończone archiwa
są czytelne, ale rozpoczęty konkurs z wynikami i inną/brakującą rewizją
blokuje wczytanie sezonu przed deserializacją. Brak automatycznego przeliczania
częściowo rozegranych zawodów. Puste przyszłe konkursy przy starcie otrzymują
bieżącą rewizję. Przy zmianie algorytmów należy podnieść modelRevision.
Test niezgodnego trwającego konkursu i zakończonego archiwum PASS
(--production-persistence-only, /tmp/sj-model-revision-test.log).
Punkt 8 w trakcie: jawni współdzieleni właściciele heapowych wyników
klasyfikacji (bez przejmowania pożyczonych rekordów), zwalnianie kalendarzy
z grafu sezonu i wycofanych kalendarzy po wygaśnięciu undo/redo. Wymaga
regresji historii klasyfikacji, cofania kalendarza i ponownego buildu.

Punkt 8 domknięty: ownResult jawnie przejmuje heapowe wyniki klasyfikacji;
kopie historii współdzielą ich czas życia. Pożyczone rekordy (np. tymczasowy
ranking) nie są usuwane. Deserializacja, naliczanie sezonu i wirtualny ranking
korzystają z ownResult; usunięto ręczne kasowanie tych samych rekordów.
SimulationSave zwalnia jednoznacznie posiadany graf kalendarzy i jego obiekty
raz, a QuickCalendar zwalnia usunięte kalendarze dopiero po wygaśnięciu undo.
Regresje zachowania wyników przy kopiowaniu/odtwarzaniu oraz pożyczonych
rekordów PASS (--production-persistence-only, /tmp/sj-graph-ownership-test.log).
Pełne backendRegressions i --dates-only PASS (/tmp/sj-calendar-final-test.log),
w tym klasyfikacje, kalendarze, cofanie, presety i scenariusze dat.
Raport: wszystkie 10 punktów mają implementacje i regresje. Nie oznacza to
braku innych błędów produkcyjnych ani wykonania ręcznego sezonu 20 konkursów.

Dodatkowe uwagi kalendarza zrealizowane:
- sam „+” dodaje konkurs, ma widoczne tło, nazwę dostępności i tooltip;
- Nowy kalendarz, aktywacja i powrót do edycji mają wtórny akcent;
- stałe zakładki Kalendarz / Szybkie planowanie / Domyślne zasady,
  bez przycisku powrotu zmieniającego wysokość i bez toggle po drugim kliknięciu;
- szybkie planowanie z widoku miesięcznego prowadzi do tego samego panelu;
- wybór zasad kwalifikacji jest odłożony do zamknięcia popupu, żeby zmiana
  modelu nie usuwała dropdowna w trakcie obsługi jego sygnału;
- zabezpieczono brak paczki i indeks reguły poza jej zakresem.
Test UI wielokrotnie przełącza zasady kwalifikacji z klawiatury, następnie
korzysta z planowania i pełnego przepływu dat. PASS --dates-only, bez ostrzeżeń
QML; sprawdzono /tmp/sj-calendar-defaults.png. EN/CS aktualne.

Końcowa spójność edytora list: przycisk „+” w sekcji kraju przyjmuje tylko
skoczka z tego kraju, tak jak przeciąganie. Zapobiega dodaniu do niewidocznej
sekcji innego kraju. Ponowny build i --roster-workspace-only PASS
(/tmp/sj-roster-final-test.log). Aktualna kolejka tego zakresu jest pusta.

Nowe poprawki po zamknięciu raportu: każda zmiana rodzaju wydarzenia na
kwalifikacje wybiera jego domyślne zasady kalendarza, także przy ponownym
przełączeniu. Konkurs wraca do domyślnych zasad konkursu. Nowe treningi
oraz próbne używają swoich osobnych domyślnych zasad. Jawne wskazanie zasad
w tym samym poleceniu ma pierwszeństwo. Zachowana zgodność rodzaju drużynowego.
Regresje zmiany, ponownego przełączenia, undo i domyślnych zasad treningów/
próbnej PASS --dates-only (/tmp/sj-calendar-planning-sidebar-test.log).

Edytor pojedynczej listy: klik całego wiersza, checkboxa lub przycisku +/-
od razu przełącza przynależność; bez kroku „Dodaj zaznaczonych”. Działa też
Spacja i akcja dostępności. Lista jest bezpośrednim dzieckiem layoutu zamiast
przeładowywanego Loadera, aby zajmowała pełną wysokość. „Dodaj widocznych”
pozostaje akcją masową obejmującą aktualne wyszukiwanie/kraj, z tooltipem
wyjaśniającym, że dotyczy także wierszy poza viewportem.
Test zdarzeń myszy: wiersz dodaje, wybrany wiersz usuwa, checkbox działa
natychmiast; lista wybranych zajmuje >65% wysokości workspace. PASS
--roster-workspace-only (/tmp/sj-roster-click-transfer-test.log).

Najnowsza nawigacja kalendarza zastępuje poprzednie zakładki: szybkie
planowanie zajmuje prawy inspektor obok stale widocznej listy konkursów;
ma osobny powrót do edycji, poza trzema zakładkami konkretnego wydarzenia.
Pusty inspektor wyjaśnia zaznaczenie konkursu i daje wtórną akcję planowania.
Domyślne zasady są pierwszą pozycją menu ustawień kalendarza, bez kolejnego
rzędu zakładek. Powiększony „+” ma kontrastowy obrys, bez dominującego fill.
Komunikaty podglądu planowania nie pojawiają się nad właściwą listą wydarzeń.
Test wejścia z empty state, widoczności listy, menu zasad i pełnego planowania
PASS --dates-only (/tmp/sj-final-calendar-check.log), light/dark i małe okna.

Usunięto dopisek „Dzień bez zawodów — czas na odpoczynek lub podróż.”
z widoku kalendarza. Zastąpiono rzeczowym „Brak wydarzeń.”, EN/CS aktualne.

Dolny pasek sezonu: jeden rząd, mniejsza flaga i padding, przycisk ograniczony
rozmiarem i wyrównany do prawej. Wysokość liczona z rzeczywistej treści;
wyeliminowano rezerwację dwóch rzędów przez GridLayout i nadmiar dołu.
Loader nie rozciąga paska w pionie. Na badanym widoku 1440 szerokości pasek
zmalał z 88,4 do około 45,6 px. Regresja wymaga wysokości nie większej niż
przycisk +12 px. Końcowy --dates-only PASS, także backend i planowanie,
bez ostrzeżeń układu (/tmp/sj-footer-final-test.log). Sprawdzone wyrównanie
przycisku po prawej na screenshotcie. EN/CS i diff-check PASS.

Korekta położenia szybkiego planowania: akcja jest na wysokości zakładek
Konkurs / Zasady / Powiązania, oddzielona odstępem i pionowym separatorem.
Nie zajmuje osobnego rzędu nad inspektorem. Pozostaje akcją całego kalendarza;
lista wydarzeń jest widoczna, a kliknięcie zakładki wraca do edycji wydarzenia.
Nagłówek planowania jest mierzony po ułożeniu, bez pętli implicitHeight w Qt.
Regresja sprawdza wyrównanie akcji i zakładek oraz cały przepływ planowania.
Końcowy build i --dates-only PASS bez ostrzeżeń układu
(/tmp/sj-planning-row-verified.log), również cały przepływ podglądu i zapisu dat.

Na końcu, według nowej prośby: osłabienie presetów i automatycznych propozycji
wiatru zmienione z 40% na 20% względem bazy sprzed c0e96b1.
strengthScale 0,8 ×0,8 =0,64; variationScale/gustScale 0,75 ×0,8 =0,60.
Zgodne JSON wbudowany, fallback generatorów i definicje laboratorium.
Ręczne pole oraz częstość podmuchów nie są zmieniane. Regresja tych samych
seedów potwierdza 80% siły/wahań/podmuchów starej bazy i 64% przestrzennych
różnic (iloczyn dwóch skal). Test simulation-v2-weather PASS (10 tys.
propozycji), build aplikacji PASS. Aktywny lokalny profil nie ma override'ów
trzech globalnych skal. Szczegóły w docs/NEW_SIMULATION.md.
