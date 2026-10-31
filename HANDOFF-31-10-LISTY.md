# Pilne przekazanie — listy zawodników, 31.10.2026

## Zacznij tutaj

Użytkownik poprosił o szybkie zakończenie dokumentacją z powodu kończących się
tokenów. Kontynuuj WSZYSTKIE niedokończone prośby opisane poniżej. Nie kończ po
pojedynczym punkcie. Pracuj sam, na `feature/simulation-v2`, nigdy main.
Osobne commity ukończonych zmian i push po każdym. Nie zamykaj gry użytkownika
ani nie wykorzystuj jego rzeczywistych danych do testów.

**Ostatnia duża przebudowa jest WIP. Nie przedstawiaj jej jako gotowej.**
Została zapisana do repo razem z tym plikiem, aby mogła przejść na inny komputer.
Nie wdrażano jeszcze tłumaczeń nowych tekstów i pełnych testów nowego katalogu.
Poprzedni komunikat „34/34” dotyczył wcześniejszej listy, NIE obecnych poprawek.
Użytkownik odrzucił dotychczasowy UX list i zgłosił rzeczywiste błędy.

## Aktualne prośby użytkownika — do dokończenia

1. Wybór list ma mieć piękne, kompaktowe UI; **nie zwykły dropdown**.
2. Cały obszar list ma być nowym, spójnym ekranem. Nie wracaj do starej tabeli,
   ukrytych akcji i przycisku „Otwórz” w prawym górnym rogu.
3. Tworzenie, wybór, otwieranie i usuwanie mają być proste i niezawodne.
   Użytkownik zgłaszał znikanie list, utratę synchronizacji i gubienie dodanych.
4. Kliknięcie listy od razu ma pokazywać jej zawartość do edycji inline.
5. Wiersze zawodników nie mogą być tak ciasne, że kontrolki na siebie nachodzą.
   Kompaktowość ma zachowywać równowagę, hierarchię i czytelność.
6. Crash po przejściu z „Kolejność” na widok krajów/drużyn został ODTWORZONY
   i podstawowa naprawa jest już wypchnięta. Zachowaj ją i rozbuduj regresję
   również dla nowego ekranu inline.
7. Priorytet dodatkowy: niewidoczna zawartość dropdownów. Wspólny komponent
   został naprawiony i wypchnięty; sprawdź jego współpracę z nowymi ekranami.

## Ustalone zachowanie, którego nie zmieniać

- Dwa panele: wszyscy zawodnicy oraz wybrani. Domyślnie grupowanie krajami.
- Na małym ekranie przełączaj panele, pokazując liczbę wybranych.
- Zaznaczanie wielu i zbiorcze dodawanie/usuwanie; osobne +/− też mogą zostać.
- Przeciąganie między panelami oraz zmiana kolejności w widoku kolejności.
- **Pusta lista = wszyscy. Brak aktywnej listy = wszyscy.**
- Limity krajów są pomocnicze, nigdy nie blokują doboru.
- Lista przypisana do kalendarza staje się aktywna przed jego konkursem.
  Ręczna zmiana składu przed tym konkursem nie zmienia przypisania kalendarza.
- Usunięcie aktywnej listy wybiera wszystkich i wyświetla informację.
- UI: hierarchia, kontrast, jasność, brak szumu, konserwatywny polski tekst,
  ekspresyjna alpejska tożsamość, desktop, małe laptopy, reduced motion.

## Już zakończone i wypchnięte poprawki w tej turze

- `095549e`: `SChoice.qml` — wysokość popupu wynika z liczby opcji i jawnej
  wysokości wiersza, a nie z contentHeight jeszcze nieutworzonych delegatów.
  Etykiety zwykłych opcji przez `textAt(index)`, jawne wymagane role delegatów.
  Test `--choice-popup-only`: stringi, obiekty, model C++, dwa motywy, skale
  0.7/1.0, wyszukiwanie włączone/wyłączone; tekst i kliknięcie opcji PASS.
- `6974812`: `JumpersListsListModel` mutuje wektor MIĘDZY begin/endInsertRows
  i begin/endRemoveRows. Stary kod ogłaszał wiersze przed dopisaniem danych,
  a potem dopiero zewnętrzny DatabaseItemsListView zmieniał wektor.
  Zewnętrzne podwójne mutacje zostały usunięte. To realna przyczyna desynchronizacji.
- `9d598e3`: osobny `groupedLibrary` w QuickRoster, bez sortowania na żywo
  jednego modelu przy przełączaniu grupowania. `setGrouped()` tylko zmienia
  tryb i emituje changed. `SavedRosterPane` tworzy oddzielną instancję
  `SavedRosterList` dla widoku grupowanego i zwykłego. Nie wracaj do jednego
  ListView, którego model i section.property zmieniają się naraz.
- `b5a8b28`: `--roster-workspace-only`, kontrakt QAbstractItemModelTester,
  atomowe dodawanie/usuwanie, 50 rzeczywistych przełączeń widoku dla 600 skoczków.
  Przed naprawą deterministyczny SIGSEGV, po naprawie PASS.

Backtrace crasha: Qt Quick section handling → QString comparison →
QuickRoster::refresh() → zapis grouped z QML. Nie był to hipotetyczny crash.
Lokalny log: `/tmp/sj-roster-grouping-gdb.log` (na innym komputerze go nie będzie).

## WIP zapisany razem z tym przekazaniem

### Model i kontroler

`source/UI/Seasons/SimulationSaveManagerWindow.{h,cpp}`:

- Własne API katalogu: rosterEntries, editingRosterId, editingRosterName,
  rosterEditor (QObject*), canRestoreRoster.
- Metody: createRoster, selectRoster, renameRoster(id,name), removeRoster,
  restoreRoster, openRosterWorkspace, setRosterMembers, setRosterCountryLimits.
- Stały QuickRoster związany z menedżerem sezonu i identyfikatorem listy.
- publishRosters() scala powiadomienia przez QTimer::singleShot(0), aby nie
  przebudowywać QML w środku obsługi kliknięcia/pola tekstowego.
- Tworzenie generuje unikalną nazwę i od razu wybiera/otwiera listę.
- Usuwanie ma jedno cofnięcie: kopia SaveJumpersList, pozycja, stan aktywności,
  identyfikatory powiązanych kalendarzy. Przywracanie nie trzyma surowych
  wskaźników do potencjalnie usuniętych kalendarzy.
- selectActiveRoster korzysta teraz z odroczonej publikacji.

`source/UI/Quick/QuickRoster.{h,cpp}`:

- Nowa ścieżka source=SimulationSaveManagerWindow, lista odnajdywana po ID.
- setSeasonRoster(id) czyści zaznaczenie/filtry/undo i odracza sync.
- read/candidates/write oraz limity obsługują bezpośrednio sezon, bez starego
  JumpersListEditorWindow i bez jego dwóch ukrytych widgetowych tabel.
- Dotychczasowa ścieżka starszego edytora pozostaje dla kompatybilności testów.

`JumpersListsListModel.{h,cpp}`: nowa metoda insertList(row,value), aby publikować
od razu prawidłową nazwę i ID, również podczas cofania usunięcia.

### UI i nawigacja

- NOWY `source/UI/Quick/SeasonRostersWorkspace.qml`: nagłówek, Nowa lista,
  pionowy katalog z lewej, na małym ekranie poziome karty, edytor inline,
  nazwa listy, Użyj w konkursach, Usuń listę, Cofnij usunięcie, empty state.
- NOWY `RosterCard.qml`: wspólna karta wyboru z alpejskim Surface, mocnym
  zaznaczeniem, nazwą i krótką informacją o składzie.
- `SeasonRosterChoice.qml` przebudowany: poziome karty aktywnej listy zamiast
  SChoice; wirtualne „Wszyscy”, przyciski Zarządzaj i +.
- `QuickBridge.cpp` zwraca kind=seasonRosters dla katalogu list, PRZED
  tworzeniem starej tabeli/toolbaru. To ma eliminować „Otwórz” i dawny katalog.
- `Workspace.qml`, `Node.qml`, `quick-ui.qrc` rejestrują nowy ekran i komponenty.
- `SavedRosterWorkspace.qml`: inlineEditor ukrywa stare projected name/default/
  submit; backend może być podany bez node. W trybie inline jest osobne Cofnij.
- `SavedRosterList.qml`: zwiększona bezpieczna wysokość wierszy, jawne rozmiary
  checkboxa/przycisku i min-width nazwy. TO JESZCZE WYMAGA OCENY WIZUALNEJ.
- `SavedRosterPane.qml`: footer jako responsywny GridLayout, zbiorcze akcje
  pojawiają się przy zaznaczeniu; zwalnia miejsce i nie ściska przycisków.

## Od czego zacząć po pullu

1. Zbuduj i uruchom `--roster-workspace-only`. Ostatni build WIP był uruchomiony
   komendą poniżej; wynik w momencie pisania NIE był jeszcze zweryfikowany:
   `/tmp/sj-inline-roster-build.log`, exec session 60424 na tym komputerze.
   Po przeniesieniu nie używaj tego numeru sesji.
2. Napraw błędy kompilacji/QML, jeśli występują. WIP nie jest zatwierdzonym UI.
3. Dodaj test nowego katalogu INLINE, nie tylko starego JumpersListEditorWindow:
   stwórz 10–30 list przez UI, każda ma natychmiast istnieć i być wybrana;
   nazwij, dodaj skoczków, przełącz kilka razy, usuń aktywną/ostatnią/inne,
   cofnij usunięcie, zapisz i wczytaj sezon. Kontroluj ID, skład, aktywną listę,
   powiązania kalendarzy i brak duplikatów. Nie wystarczy model z 2–3 skoczkami.
4. Ponów stres przełączania grupowania na nowym inline backendzie dla 600+ osób,
   z filtrami i zaznaczeniem, resize i powrotem między listami. Nie sortować
   na żywo tego samego modelu przy zmianie section.property.
5. Sprawdź pułapki WIP: szybka zmiana wybranej listy przed odroczonym sync;
   usunięcie ostatniej; odroczone callbacki po zamknięciu; edycja nazwy i zmiana
   listy (rename musi użyć ID z chwili rozpoczęcia edycji, nie nowo wybranej);
   cofnięcie usunięcia po innych zmianach aktywności i kalendarza; zewnętrzne
   modyfikacje starszą ścieżką. W razie potrzeby popraw serializację undo/stanu.
6. Obejrzyj screenshoty jasny/ciemny, 1440×900, 1024×640 i mały laptop,
   skale .7 i 1.0. Wiersze nie mogą ucinać checkboxów, nazw, kraju ani +/−.
   Nowy wybór list ma być charakterystyczny i ładny, nie generyczna tabelka.
7. Uzupełnij EN/CS dla WSZYSTKICH nowych qsTr/tr. Nowe konteksty m.in.
   SeasonRostersWorkspace, RosterCard, zmieniony SeasonRosterChoice,
   SimulationSaveManagerWindow (Nowa lista/Nowa lista %1), JumpersListsListModel.
   W WIP tłumaczenia nie są jeszcze uzupełnione.
8. Uruchom ponownie wybory dropdownów oraz testy poniżej. Testy starego katalogu
   mogą wymagać dostosowania do nowej nawigacji; nie przywracaj „Otwórz” dla testu.
9. Zaktualizuj aktualny handoff i opis zmian. Nie ogłaszaj znów „wszystko gotowe”
   na podstawie samych testów backendu bez obejrzenia nowego ekranu.

## Polecenia i środowisko

Linux Qt 5.15.3; zachować zgodność z Qt5/Qt6 (wersje importów, int/qsizetype).
`SJ_BUILD_JOBS=8 scripts/test-quick-ui.sh --roster-workspace-only`

Istotne flagi: `--choice-popup-only`, `--simulation-v2-season-only`,
`--roster-only`, `--workflow-only`, `--classifications-only`,
`--archive-history-only`, `--example-season-only`, `--translation-only`.

`python3 scripts/check-translations.py` oraz `git diff --check`.

Bez przebudowy: uruchamiaj build-quick-ui/sj-sim-ui-smoke z cwd=build-quick-ui,
QT_QPA_PLATFORM=offscreen, QT_QUICK_BACKEND=software i ABSOLUTNYMI ścieżkami:
LD_LIBRARY_PATH=<repo>/build-qml-preview/qt-runtime/lib
QML2_IMPORT_PATH=<repo>/build-qml-preview/qt-runtime/qml
Test zmienia cwd do tymczasowego katalogu; względne QML paths powodują błędy.

Push: `git -c remote.origin.pushurl=git@github.com:KonradKrol/sj-sim.git push origin HEAD:feature/simulation-v2`.
Bez subagentów. Dostępne umiejętności w `.agents/skills`, szczególnie qt-qml
oraz qt-ui-design. Serwera run_qmlPreview nie było w tej sesji.

## Starsze niezakończone kwestie — nie zgubić

- Pusta lista po Qualifications → Classical KO: zabezpieczono różne instancje
  zawodników o tym samym ID i dodano test z rzeczywistymi presetami i reloadem.
  Nie odtworzono dokładnie prywatnego zapisu użytkownika. Potwierdził, że wyniki
  kwalifikacji są, a następny konkurs pokazywał 0. Jeśli wróci zgłoszenie,
  diagnozuj dalej; nie przepuszczaj wszystkich jako obejścia pustej kwalifikacji.
- Dawny PB 212,5 + Enter: `30-10-work.md`; ochrony dodane, pierwotny losowy
  segfault nieodtworzony. Użytkownik zabronił ponownie badać dane wskazanego skoczka.
- Pełny historyczny smoke całej aplikacji nie jest potwierdzony jako zielony.
- Pozostała dokumentacja: `31-10-start.md`, `docs/WORK_QUEUE_31_10.md`,
  `docs/RESUME-28-10-2026-simulation.MD`. Pierwotna lista 34 punktów jest w repo,
  ale obecne poprawki użytkownika OTWIERAJĄ ponownie obszar list zawodników.
