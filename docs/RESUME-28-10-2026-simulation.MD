# Wznowienie prac nad nową symulacją — 28.10.2026

Aktualizacja 29.10: wdrożono osobne laboratorium balansu obok starego playgroundu.
15 pytań, 607 ustawień, trzy serie, paczki i grupy, mapy, lądowanie, pełna analiza
wiatru, generator, scenariusze celów, profile, historia, odtwarzanie i CLI.
Profile stosuje się osobno od następnego konkursu/sesji, również w sezonie;
rozpoczęty konkurs zachowuje kopię. KO, reprezentacje i drużyny testowe są w zakresie.
Instrukcja: [BALANCE_LAB.md](BALANCE_LAB.md), wykonanie i testy:
[BALANCE_LAB_IMPLEMENTATION.md](BALANCE_LAB_IMPLEMENTATION.md). Historyczna kolejka
poniżej nie jest poleceniem ponownej implementacji tych funkcji.

## Poprzedni etap na Windows — playground v2

**Nowy playground v2 został wdrożony.** Szczegóły: `PLAYGROUND_V2.md`
i końcowa sekcja `SIMULATION_V2_PROGRESS.md`. Starsza kolejka poniżej
opisuje punkt przekazania przed tym etapem; nie implementować playgroundu od nowa.

- 199 ustawień, bezpośredni Engine v2, kanoniczne źródła, dwa tryby zegara,
  porównania, skoczek, cała stawka, lądowanie/noty i pełna analiza pola wiatru.
- Podgląd w tle z debounce i odrzucaniem starych wyników, duże batchowe próby,
  zapis konfiguracji/historii, replay, cele kalibracji, CLI i eksport analizy.
- Audyt następnego sezonu: nowe drafty, poprawne wstawianie profili v2,
  zachowane UUID, perki, zdjęcia i izolacja anulowanych zmian.
- Naprawiona publikacja migracji skoczni na Windows: member rename pliku
  tymczasowego oraz wyłączenie autoRemove po udanym opublikowaniu.
- Zsynchronizowane źródła lupdate, EN/CS bez unfinished i błędów placeholderów;
  uzupełnione również brakujące teksty z wcześniejszych etapów v2.
- Windows/Qt 6.8.3: build i regresje playgroundu, migracji/katalogu, konfiguracji,
  konkursów, pogody, manipulacji, sezonów, edytorów, list, kalendarza i języków PASS.
  Rdzeń bez Qt: cztery zestawy MSVC PASS. Polecenie:
  `powershell -ExecutionPolicy Bypass -File scripts/test-simulation-v2-core.ps1`.

Następny kierunek: kalibracja na podstawie eksperymentów i oceny użytkownika.
Playground jest odizolowany od parametrów normalnej gry. KO/drużyny w samym
playgroundzie są świadomie poza pierwszą wersją, zgodnie z odpowiedzią użytkownika;
normalna gra zachowuje te tryby. Automatyczny rozwój formy nadal jest odłożony.
Ostatnie zmiany testowano na Windows/Qt6; wcześniejsze Linux/Qt5 zachowują
odrębne wyniki, bez deklaracji ponownego uruchomienia tej nowej wersji na Linuxie.

Ten plik jest punktem startowym dla kolejnej sesji AI na innym komputerze.
Zakładamy aktualny pull całego repo. **Całość `NEW_SIMULATION.md` nie jest jeszcze
ukończona.** Nie rozpoczynać ponownie gotowych etapów.

## Branch i zasady pracy

- Pracować na **`feature/simulation-v2`**. Nie dotykać maina.
- Pracować **samodzielnie, bez subagentów** — wyraźna preferencja użytkownika.
- Kończyć etapy testami, commitować i wypychać na ten branch. Nie zostawiać
  istotnych ustaleń wyłącznie w rozmowie.
- Używać odpowiednich skills z `.agents/skills`, zwłaszcza `qt-qml` i
  `qt-ui-design`. Projekt obsługuje Qt5 i Qt6; nie wprowadzać importów dostępnych
  wyłącznie w Qt6. Lokalnie testowano **Qt 5.15.3 na Linuxie**.
- UI: kontrast, hierarchia, wyrazisty sportowy charakter, kompaktowy desktop,
  małe laptopy, dark mode. Tekst krótki, rzeczowy, konserwatywny po polsku.
- Zachować opcjonalne Widgets i dawne ścieżki. Nowe normalne ekrany korzystają
  z nowych profili; nie przywracać przełącznika starego/nowego edytora.

## Co czytać i w jakiej kolejności

1. [`NEW_SIMULATION.md`](NEW_SIMULATION.md) — pełne wymagania i decyzje użytkownika.
2. **Końcówkę** [`SIMULATION_V2_PROGRESS.md`](SIMULATION_V2_PROGRESS.md)
   — szczegółowy aktualny stan, ukończone etapy, testy i kolejka. Starsze sekcje
   opisują wcześniejsze etapy, czasem już zastąpione nowszymi.
3. [`docs/SIMULATION_V2_ENGINE.md`](SIMULATION_V2_ENGINE.md) — model, wzory,
   parametry i integracja.
4. [`docs/WIND_MEASUREMENT_RESEARCH.md`](WIND_MEASUREMENT_RESEARCH.md) oraz
   [`docs/WEATHER_RESEARCH.md`](WEATHER_RESEARCH.md) — źródła i granice
   założeń dotyczących pomiaru oraz deszczu.
5. [`docs/TRANSLATION_AUDIT_2026-10-28.md`](TRANSLATION_AUDIT_2026-10-28.md)
   — naprawy tłumaczeń i świadomie pozostawiony szerszy audyt.
6. [`docs/RELEASE_NOTES_VS_MAIN.md`](RELEASE_NOTES_VS_MAIN.md) — dopisywać
   szczegółowe zmiany do późniejszych release notes.

## Gotowe przed ostatnią poprawką wiatru

- Nowy deterministyczny silnik jakości, lotu, geometrii, lądowania/not i punktów;
  OVR/forma/perki, deszcz i śnieg, ciągłe pole wiatru w przestrzeni i czasie.
- Kanoniczne paczki i nowe edytory skoczków/skoczni, ostrożna przybliżona migracja,
  integracja pojedynczych skoków, konkursów oraz nowych sezonów.
- Jeden zegar pogody dla skoków ręcznych, DNS, czekania i automatycznych przebiegów.
- Podgląd utrwalonego wiatru konkretnego skoku przed/w locie/po; archiwum używa
  danych historycznych, a nie aktualnego pola. Opcjonalny podgląd przed skokiem.
- **Sześć metod pomiaru wiatru**, niezależnych od fizycznego lotu.
  Użytkownik wyraźnie zaakceptował **„FIS — przybliżenie”**. Nie szukać ponownie
  niedostępnej pełnej receptury i nie przedstawiać naszych wag jako oficjalnych.
- Naprawione odświeżanie podpisów po zmianie PL/EN/CS, w tym „Zakończ konkurs”.
  Przy zmianie języka nie podłączać ponownie handlerów przejść serii/grup.
- **Nowa manipulacja skokiem**, commit `63862c7`, wypchnięty na remote:
  `SimulationJumpDraft`, trzy sekcje Lot / Lądowanie i noty / Warunki,
  opcjonalne korekty jakości/odległości, zakresy, częściowe noty, landing,
  procentowe DSQ i lokalny ciągły wiatr. Zatwierdzony draft działa przez jeden
  skok; anulowanie nie nadpisuje poprzedniego. Fizyczny wiatr, opady i pomiar do
  punktacji są niezależne. Wspólne pole pogody pozostaje nietknięte.
  Jest również natywny widok Widgets tego samego draftu oraz EN/CS.
  Usunięto recursive rearrange w siatce sędziów.

## Ostatni etap: uwagi użytkownika do konfiguracji wiatru

Po manipulacji użytkownik poprosił o poprawki poniżej. **Zostały ukończone**
i są zapisane w commicie razem z tym plikiem. Wyniki testów są na dole.

- Cztery ręczne ustawienia: kierunek, siła, zmienność w czasie i różnice na
  zeskoku mają zachowywać swoje wartości. Błąd był w `inferControls()` wywoływanym
  po każdej edycji; wtórnie przeliczał inne inputy. Oddzielono wartości wybrane
  ręcznie od szacowania parametrów proponowanego scenariusza.
- Ręczna zmiana kierunku jednego miejsca zachowuje jego siłę. Podczas edycji
  grafika pokazuje `basePreview`, czyli te same wartości wyjściowe co inputy,
  bez chwilowych podmuchów. Zwykły podgląd nadal pokazuje rzeczywiste pole.
- `reroll()` dla własnych warunków zachowuje cztery ustawienia, ale losuje nowy
  profil przestrzenny i przebieg w czasie. Przycisk ma tooltip, że zastępuje
  ręczne lokalne zmiany. Presety nadal losują wariant w swoich zakresach.
- Większa, czytelniejsza ikona skoczka/nart; usunięte dwie boczne pomocnicze
  strzałki. `preventStealing` utrzymuje przeciąganie poza kołem w ScrollView.
- Jeden dropdown **Bez opadów / Śnieg / Deszcz**, ikony SVG; intensywność,
  przebieg i opis ukryte przy braku opadów. Także w natywnym edytorze.
- Zamiast słupków przypominających zasięg sieci: własne proste ikony scenariuszy.
- Edycja całej skoczni i wybranego odcinka otwierają się zamiennie. Mają wyraźne
  zakresy i odmienną oprawę; nie tworzą dwóch zlewających się formularzy.

Najważniejsze pliki tego etapu:

- `source/simulation-v2/WeatherSetup.h/.cpp`
- `source/UI/Quick/WeatherWorkspace.qml`, `WeatherTuning.qml`
- `source/UI/Quick/WindDirectionDial.qml`, `WindStationEditor.qml`, `WindSlopePreview.qml`
- `source/UI/Quick/WeatherPrecipitation.qml`, `WeatherPresetGrid.qml`, `WeatherPresetChoice.qml`
- `source/UI/Quick/icons/weather/`, `quick-ui.qrc`
- `source/UI/EditorWidgets/WeatherSetupEditor.cpp`
- `tests/simulation-v2-weather-setup-regressions.h`, `tests/simulation-v2-setup-regressions.h`
- `translations/translation_en.ts`, `translation_cs.ts`

Uwaga testowa: offscreen drag wymaga `QMouseEvent` z `Qt::LeftButton` w buttons.
Samo `QTest::mouseMove` nie odtwarzało poprawnie przytrzymania. Test naprawdę
przeciąga poza cały dial i sprawdza końcowy kąt. Wcześniej trzeba przewinąć dial
w całości do viewportu — kliknięcie niewidocznej części nie jest testem kontroli.

## Co robić następnie

1. Po sprawdzeniu końcowego statusu poniżej uznać gotowe poprawki wiatru za
   zakończone; nie implementować ich od nowa. Pusta pozycja „8.” w wiadomości
   użytkownika nie zawierała dodatkowego wymagania.
2. **Nowy playground pełnego silnika v2** — największy pozostały etap.
   Obecny `source/playground` i jego UI nadal opisują stare techniki/sektory;
   użytkownik odrzucił ten playground. Nie przepinać go pozornie i nie twierdzić,
   że jest zgodny z nowym modelem. Wymagania w `NEW_SIMULATION.md`:
   - strojenie rzeczywistych `EngineParameters`, profili, skoczni i warunków;
   - porównania przy wspólnych seedach; rozkłady odległości, punktów, miejsc,
     perspektywa konkretnego skoczka i zależności mikro/makro;
   - interaktywne krzywe szans upadku, lądowania i not względem odległości;
   - mała próba w tle po zmianach, debounce, anulowanie przestarzałych prób;
   - duże batchowe konkursy na wielu wątkach, kontrola pamięci/anulowanie;
   - CLI i eksport do analiz przez AI, konfiguracje i powtarzalność.
   To nadal praca do wykonania, nie gotowy moduł. Nie było jeszcze implementacji
   nowego playgroundu w tej sesji. Docelowy komputer: Ryzen 7 5700G, 32 GB RAM.
3. Audyt przejścia do następnego sezonu, presetów/list i edge case'ów KO/drużyn;
   zachowanie UUID, pełnych profili v2, zdjęć i powiązań zapisu.
4. Pełny audyt tłumaczeń i regresja całości. Użytkownik pozwolił odłożyć pełny
   audyt językowy; kontrola TS sama nie dowodzi kompletności źródeł.
5. Uzupełniać powyższe dokumenty i release notes, commit/push etapami.

**Automatyczny nowy rozwój formy pozostaje odłożony przez użytkownika/specyfikację.**
Ręczny generator formy v2 działa; nie mylić tych dwóch rzeczy.

## Budowanie i testy na drugim komputerze

Linux, z katalogu repo:

```bash
git branch --show-current
scripts/build-quick-ui.sh
scripts/run-quick-ui.sh
scripts/test-simulation-v2-core.sh
scripts/test-quick-ui.sh --simulation-v2-setup-only
scripts/test-quick-ui.sh --simulation-v2-manipulation-only
scripts/test-quick-ui.sh --simulation-v2-contest-weather-only
scripts/test-quick-ui.sh --simulation-v2-season-only
python3 scripts/check-translations.py
```

Windows: [`WINDOWS_SETUP.MD`](WINDOWS_SETUP.MD). Zbudować lokalnie; foldery build
nie są przenoszone między komputerami. Windows/Qt6 ostatnich etapów nie testowano
w tej sesji. Nie traktować pomyślnego lokalnego Linux/Qt5 jako takiej weryfikacji.
Nie wprowadzać wersji Qt6-only, zwłaszcza nieznanych na Qt5 importów QML.

Dodatkowe smoke flags: `--simulation-v2-only`, `--simulation-v2-competition-only`,
`--simulation-v2-measurement-only`, `--bulk-only`, `--translation-only`.
Fixture'owe testy uruchamiać kolejno, nie równolegle. Nie uruchamiać kilku buildów
w tym samym katalogu. Pierwszy build smoke jest droższy; kolejne używają cache.

## Końcowy status przy przekazaniu

**Manipulacja oraz wymienione poprawki wiatru są ukończone i zweryfikowane.**
Zapisane w dwóch kolejnych commitach na `feature/simulation-v2`; oba wypchnięte
na origin. Main nietknięty. Po pull nie ma celowo pozostawionego niedokończonego
kodu z tego etapu. Dokładne identyfikatory: `git log -3 --oneline`.

PASS po ostatnich zmianach: `--simulation-v2-setup-only` (w tym niezależne pola,
nowe losowania, kierunek/siła lokalna, realny drag poza dial, opady, 2048 skoków
identycznych przy 1 i 4 wątkach), `--simulation-v2-manipulation-only`,
`--simulation-v2-contest-weather-only`, `--simulation-v2-competition-only`,
`--simulation-v2-season-only`, `--simulation-v2-measurement-only`,
`--translation-only`, kontrola katalogów TS i produkcyjny build Qt5.
Rdzeń v2 również PASS; zwykły `--bulk-only` przeszedł przy domykaniu manipulacji.
Brak błędów QML, pętli bindingów i `Detected recursive rearrange` w tych testach.
Nie zweryfikowano ostatnich zmian na Windows/Qt6.

**Następna sesja: zacząć od zaplanowania i wdrażania nowego playgroundu v2**,
a następnie pozostałego audytu sezonów/presetów i tłumaczeń. Szczegóły powyżej.
Nie mylić domknięcia tych dwóch etapów z ukończeniem całego `NEW_SIMULATION.md`.

## Aktywne zlecenie UX — 29.10 wieczorem (po pull 61846d4)

Pracować solo na feature/simulation-v2. Każdy skończony commit wypychać.
Użytkownik zlecił cały poniższy zestaw, nie tylko laboratorium:
1. Zastąpić przewijane poziomo zakładki lewego panelu labu (Dane próby itd.)
   nawigacją, w której wszystkie sekcje widać. WIP: siatka 2×2 w BalanceLabControls.
2. Ukryć lab w menu głównym (pozostaje w ustawieniach). WIP Home.qml.
3. Ładna animacja O autorze. WIP AuthorPage: MenuEntrance + przesunięcie/opacity,
   respektuje reduced motion.
4. Czytelna nazwa paczek po migracji zamiast „przybliżenie”, także już istniejących.
5. Daty logiczne. Odpowiedzi na 3 pytania: BLOKOWAĆ zmianę daty/kolejności,
   jeśli psuje chronologię i wskazać konflikt; ten sam dzień ma ręczną kolejność,
   BEZ godzin; aktualna data automatycznie wskazuje następne wydarzenie i jest
   widoczna w przeglądzie sezonu.
6. Przebudować Zaplanuj terminy: mocna hierarchia, wyraźny wybór Ustal terminy /
   Dodaj weekend, zrozumiały wybór dotyczący treningów, wariant konkursów pt-sb-nd.
7. KO przed skokiem: porównywać do lidera GRUPY, a dopiero przy jego braku do
   lidera konkursu. Seria>1, po wagach, pomijać reset. QuickBridge.cpp currentGap.
8. 30 dodatkowych sensownych animacji: 10 wyrazistych, 20 dyskretnych funkcjonalnych.
   Nie odhaczać samego dodania animacji bez użycia i reduced-motion.
9. Skocznia przed pojedynczym konkursem: podkreślić zaawansowaną edycję i dodać
   wybór skoczni/paczki bez powrotu do Konfiguracji.
10. Komunikaty o niedokładnej migracji wyłącznie w edytorze bazy; usunąć je przed
    konkursem (CompetitionConfigWindow updateCanonicalReadiness), sprawdzić
    SingleJumpsConfigWindow i inne widoki.
11. Czasownikowe checkboxy, np. Przeprowadź kwalifikacje; znaleźć do 20 miejsc.
12. Ikony natężenia i przebiegu opadów + jasne etykiety/opisy rzeczywistego wpływu
    deszczu/śniegu na skok (nie tekst o prędkości rozbiegu). Wyjaśniono użytkownikowi:
    słaby deszcz ma mały wpływ, ulewa silny nieliniowy; śnieg narastający z natężeniem.
13. DEL usuwał tylko jednego z wielu zaznaczonych. NAPRAWIONE: RosterRow nie
    nadpisuje zaznaczenia przed removeMarked, jeśli bieżący jest zaznaczony.
    Regresja prawdziwego keypress+undo PASS --rosters-only. Commit 3c7f9c4,
    push wykonany. Nie implementować ponownie.
14. Częsty fałszywy komunikat Pusta lista startowa — DO DIAGNOZY. Kod
    CompetitionConfigWindow::validateSimulationInputs wspólnie zgłasza pustą
    listę, null oraz brak imienia/nazwiska; samo poprawienie treści nie wystarczy,
    trzeba sprawdzić synchronizację i rzeczywisty przypadek niepustej listy.
15. Pusta pozycja w wiadomości — nie dopowiadać wymagań.

Lokalna zależność Linux Qt5 uzupełniona po pull: QtQuick.Shapes. Pakiety
qml-module-qtquick-shapes i libqt5quickshapes5 pobrane apt-get download i wypakowane
w ignorowanym build-qml-preview/qt-runtime (brak sudo bez hasła). Build, rdzeń,
CLI i pełne UI nowego laboratorium z remote przeszły przed tym zleceniem.

### Rozszerzenie zlecenia: punkty 15–19

15. Nowa paczka SZKIELETOWA: wszystkie osoby z punktami w męskim PŚ 2025/26
    LUB męskim LGP 2026; dane osobowe i zweryfikowane PB; ścieżki
    CountryCode_NameSurname.png; sortowanie krajami; BEZ wpisywania OVR/formy/
    parametrów skilla. Wymaga źródeł i świadomej obsługi nieuzupełnionych danych,
    nie wstawiać wymyślonych parametrów. Research rozpoczęty: oficjalne FIS
    cup-standings, winter seasoncode=2026 cupcode=WC; letnie LGP2026 należy do
    sezonu FIS2027, cupcode=GP. Wynik wyszukiwarki był częściowy (po Courchevel),
    trzeba pobrać FINAŁOWĄ tabelę, nie uznać częściowej za kompletną.
16. Potwierdzenie przed sortowaniem paczki według krajów w edytorze bazy.
17. Proste parametry wiatru 0–10 krok0.5 (kierunek zostaje w stopniach).
18. Hierarchia, kontrast, clarity zapisane w root AGENTS.md. Nowy piękny przegląd
    sezonu: pięć następnych konkursów bez nagłówka „5 następnych”; treningi i
    próbne jako statusy w wierszu konkursu, 1–2 użyteczne dodatkowe informacje.
19. Pierwsze uruchomienie: powitanie z jednym przyciskiem tworzącym przykładowy
    sezon, także opcja menu głównego; domyślne paczki skoczków/skoczni/zasad,
    jeden kalendarz z istniejącego zimowego presetu. Bez konfiguracji przez gracza.

Postęp zlecenia wieczornego:
- 13: 3c7f9c4 push, test multi-delete+undo PASS.
- 1+2: cabd0f0 push, pełny --balance-lab-only PASS.
- 3: 140b304 push, --author-only PASS.
- 4+10: fbe542e push, --simulation-v2-only PASS. Dalszy audyt migracji poza
  konfiguracją konkursu/pojedynczych skoków pozostaje do sprawdzenia.
- 7: d30b183 push, --round-points-bulk-only PASS, fallback lidera grupy/konkursu.
- 14: kod i test GOTOWE, jeszcze commit: walidator odrzucał profil bez imienia
  i nazwiska mimo że AthleteProfile go legalnie dopuszcza. Usunięto wymaganie
  nazwiska jako wyznacznika niepustości listy; null/pusta lista dalej odrzucane.
  Test konkursu z jednym anonimowym profilem PASS --simulation-v2-competition-only.
- 9: WIP SimulationDraftWorkspace, dodany Node paczki/skoczni w advanced hill,
  build i test canonical competition PASS; warto dodać asercję otwarcia zakładki.
- 12: WIP WeatherPrecipitation +6 SVG w qrc, nowe ikony intensywności/trendu i
  proste opisy efektu opadów. Jeszcze tłumaczenia, testy i osobny commit/push.
- Reszta punktów 5,6,8,11,15–19 (poza zapisaniem zasad) nadal DO WYKONANIA.

20. Użytkownik dodał kolejny punkt (ponownie nazwany „19”): edytory danych
    skoczków/skoczni/wszystkiego w sezonie mają zawsze być nowe. Pozwolił usunąć
    stare komponenty; nie jest to nakaz kasowania bez sprawdzenia zależności.
    Sprawdzić NewSeasonConfiguratorWindow, SimulationSaveManagerWindow,
    ProfileDraft i ścieżki engine-version1/2. Nie wystarczy nowy edytor bazy,
    sezon nie ma wracać do starych formularzy.

Dalszy postęp:
- 14: 873d16c push, --simulation-v2-competition-only PASS z anonimowym profilem.
- 9: 1dfbd31 push, osobny test otwarcia Skocznia i obu inline selectorów PASS.
- 17: 47a80e6 push, opt-in NumberField.quantum, trzy podstawowe pola krok0.5,
  --simulation-v2-setup-only PASS. Kierunek dalej stopnie.
- 11: WIP zmienione 22 etykiety checkboxów w .ui i kwalifikacje w C++, jeszcze
  tłumaczenia/test/commit. Lista zmian roboczo /tmp/sj-checkbox-copy.json.
- 16: WIP QMessageBox Yes/No (default No) przed sortowaniem bazy.
- 5: WIP validateDateOrder w QuickCalendarDates + guards date plan/apply/move/
  drag/duplicate/weekend. Tylko chronologia głównych wydarzeń, oraz trening/
  próbna nie później niż rodzic. Równe dni dozwolone. Jeszcze bez daty w przeglądzie.
  Tests/calendar-dates-regressions dostosowane do blokowania błędnych zmian.
  Pierwszy przebieg użył starego fixture'a; trwa powtórka /tmp/sj-logical-dates-final.log.
- 6: WIP mocne dwa kafle trybu w CalendarDatePlanner i wyjaśnienie istniejących
  treningów; dodany wzorzec fri-sun w API/QML. Jeszcze testy/tłumaczenia/commit.
- 12: WIP jak wyżej, nie zgubić 6 nowych SVG i wpisów qrc.

21. Kolejny punkt użytkownika nazwany „20”: Przebieg/Warunki/Skok na ekranie
    konkursu mają wyraźniej wyglądać na klikalne ustawienia; poprawić miejsce
    lub oprawę. Zachować dotychczasowe wymaganie oszczędzania wysokości tabel.
15. Doprecyzowanie: użytkownik zamiast niekompletnej paczki zdecydował się na
    DOMYŚLNE OVR/formę/lądowanie/perki. Można użyć standardów modelu, jawnie nie
    udawać indywidualnej oceny zawodników. Paczka ma być normalnie używalna.
    Oficjalne finałowe tabele potwierdzone przez web:
    https://www.fis-ski.com/DB/ski-jumping/cup-standings.html?sectorcode=JP&seasoncode=2026&cupcode=WC&disciplinecode=ALL&gendercode=M
    (Planica 29.03.2026, 33/33)
    https://www.fis-ski.com/DB/ski-jumping/cup-standings.html?sectorcode=JP&seasoncode=2027&cupcode=GP&disciplinecode=ALL&gendercode=M
    (Klingenthal 24.10.2026, 7/7).
    Biogramy FIS mają Birthdate, np. Domen competitorid=186516; PB wymaga
    osobnej weryfikacji, nie wpisywać wymyślonych rekordów.

22. Lab: user chce realnie widzieć ile zostało do końca eksperymentu, nie tylko
    liczbę ukończonych porównań. DO WDROŻENIA: mianownik, procent i szacowane
    pozostałe sekundy, z uwzględnieniem faz mapy/diagnostyk/celów, bez 100% za wcześnie.
23. Podgląd ma liczyć 7 konkursów na wariant, także punkty wykresu/mapy (zamiast2).
    WIP BalanceLabModel.cpp: ustawione 7 dla bazowego podglądu, komórek i celów.
24. Nie obliczać ukrytego podglądu. WIP: backend previewEnabled, schedule/launch
    respektują flagę, QML toggle steruje flagą; pełna próba nie jest anulowana
    wyłączeniem podglądu. Włączony podgląd uruchamia się na żądanie.
    --balance-lab-only PASS w /tmp/sj-lab-preview-opt-in.log, ale po nim dopisano
    jeszcze wyłączanie flagi przy loadHistory/showFullResult; sprawdzić finalny build.
    Pełny wynik po zakończeniu wyłącza podgląd, żeby nie liczyć niewidocznych prób.
20. Doprecyzowanie użytkownika: NIE utrzymywać kompatybilności starych sezonów.
    WIP SimulationSavesWindow::openSelectedSave odrzuca engineVersion!=2 z jasnym
    komunikatem. Dane na dysku pozostają nietknięte. Nie dopisywać starego UI.

Daty i planner: f299752 + 3c52b38 push; --dates-only PASS obejmuje teraz testy
backendu i UI, także blokadę kopiowania oraz dodawania weekendu w złym miejscu,
chronologię treningów/próbnych między konkursami i datę następnego eventu w pasku.
Opady/ikony: 903478c push, konfiguracja i konkurs PASS. Reguły FIS nie są zmieniane.
Nadal niezacommitowane: 22 etykiety, potwierdzenie sortowania, przyciski opcji
konkursu, nowe tłumaczenia, ograniczenie starych zapisów, lab7/previewEnabled.
Paczka98: research /tmp/sj-2026-pack-research/; FIS zablokował szybkie zapytania429,
nie ponawiać równolegle. Tabele końcowe i20biografii zcache'owane. Opracowanie
Skijumping profiles trwa, /tmp/.../people-records.json ma źródła i potwierdzonePB.
Kod roboczy collect.py i records.py nie jest jeszcze częścią repo; przenieść
potrzebne wyniki i skrypt do wersjonowanych plików przed końcem sesji.
25. PRIORYTET lab inputów: np. Regularność wpisywane -3 samo wraca do +3;
    zbadać BalanceLabParameter/PlaygroundNumber/setParameter i odświeżanie.
    Naprawić cały wzorzec, nie tylko pojedyncze pole; sprawdzić wpisywanie minus,
    edycję w trakcie powrotu wyników i brak aktualizacji przez ukryty podgląd.

26. Nowa uwaga użytkownika (jego punkt22): opisy parametrów labu są zbyt
    ogólne i powielane. Przepisać je na podstawie rzeczywistych wzorów silnika:
    co zmienia parametr, w którą stronę działa, jednostka/skala i istotne
    ograniczenia. Nie zastępować opisu konkretnego pola opisem całej grupy.
    Źródło: source/playground/PlaygroundModel.cpp (help/opisy), modele
    source/simulation-v2/, prezentacja BalanceLabParameter.qml. Opisy są W KOLEJCE,
    nie zostały jeszcze poprawione.
27. Nowa uwaga użytkownika (jego punkt23): wyszukiwanie parametrów w sekcji
    „Reguły symulacji”. BalanceLabControls.qml już ma pole search, ale filtruje
    tylko parametry bieżącego tematu i przeszukuje nazwę/klucz. Poprawić tak,
    żeby wpisanie szukanego parametru rzeczywiście znajdowało go w pełnym
    rejestrze, bez konieczności domyślenia się przełącznika „Pokaż wszystkie”.
    Uwzględnić opisy/synonimy i czytelny brak wyników. Nie przebudowywać
    delegatów przy zmianie wartości, żeby nie gubić rozpoczętej edycji.

Aktualizacja statusu commitów: etykiety a350ca2, potwierdzenie sortowania
 e0c879f i przyciski opcji88ed939 są już commitowane i wypchnięte. Wcześniejsza
 lista niezacommitowanych plików wyżej jest w tych trzech pozycjach nieaktualna.
