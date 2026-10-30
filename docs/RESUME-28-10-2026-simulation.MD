# Wznowienie prac nad nową symulacją — 28.10.2026

## Aktualny punkt kontynuacji — 29.10.2026

Pracuj solo na feature/simulation-v2; commit i push po każdej ukończonej zmianie.
Cztery poziomy perków, nowy przegląd sezonu i przykład uruchamiany jednym
kliknięciem są już gotowe i wypchnięte. Szczegóły i commity w punktach18/19/31
na końcu tego pliku. Punkt32 też GOTOWY: 7f6fbe9, pakiet30animacji i testy.
Starych instrukcji WIP poniżej nie traktuj jako nakazu ponownego wdrażania.
Najnowsza decyzja: paczki konwertować AUTOMATYCZNIE, sezony pominąć.
Aktualna poprawka opisana na końcu. Nie zabijaj uruchomionej gry użytkownika.

Aktualizacja 29.10: wdrożono osobne laboratorium balansu obok starego playgroundu.
15 pytań, 617 ustawień, trzy serie, paczki i grupy, mapy, lądowanie, pełna analiza
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

### Aktualizacja 29.10 — lab i paczka gotowe, kolejne zadania

- 01d381e: podgląd wyłącznie po włączeniu, 7 konkursów/variant i punkt mapy.
- 1fcd4ff: stabilny formularz reguł, wpisy ujemne/przecinek/kropka, wyszukiwanie
  w całym rejestrze (także opisy, bez znaków diakrytycznych), brak wyników.
- 0aa179e: konkretne opisy wszystkich 607 parametrów, usunięte opisy grup jako
  zastępstwo wyjaśnienia pola; jednostki generatora poprawione. Nowe dłuższe
  opisy źródłowe PL wymagają osobnego uzupełnienia tłumaczeń EN/CS — checker
  TS sprawdza istniejące wpisy, nie wykrywa brakujących nowych kluczy.
- 12162da: etapy obliczeń, done/total etapu, procent etapów i przybliżony ETA.
  To estymata oparta na etapach, nie dokładny procent czasu CPU. 100% nie
  pojawia się w trakcie mapy/celów. Mutex/atomiki w Playground::Control.
- fd06925 + 670d383: PACZKA GOTOWA: data/packs/men-2026/pack.json, 98 osób,
  OVR70/form70/landing2/perks0; import ręczny w edytorze. Dane i źródła obok.
  Użytkownik potwierdził, że 7 osób ma prawidłowo BRAK REKORDU — nie szukać
  dalej, nie traktować tego jako niekompletnej paczki. Nieustalony birthYear0
  pozostaje opcjonalny. Import wykonany w izolowanym edytorze w teście PASS.
- d4b8471: starszy engineVersion nie otwiera edytora sezonu (decyzja usera
  „nie utrzymuj kompatybilności”); pliki pozostają nietknięte.
- b47e7e5: tłumaczenia wcześniejszych nowych etykiet i postępu labu; poprawka
  testu multi-DEL: pobierać delegat PO aktualizacji modelu, nie przed. GDB
  wskazał crash właśnie w teście forceActiveFocus na nieważnym delegacie;
  po poprawce --rosters-only PASS. Nie był to crash produkcyjnego inputa.

Testy PASS: --balance-lab-only (ujemne -3/-3,5 i stabilność odświeżania),
--athlete-studio-only (także import98), --motion-feedback-only,
--roster-inputs-only, --rosters-only, --simulation-v2-season-only (przed WIP
nowego przeglądu), CLI scripts/test-balance-lab-cli.py, check-translations.py.

28. Nowe zadanie: prostsze usuwanie skoczka z DB. Widoczna akcja dla zaznaczonego
    zawodnika, bez szukania w menu; sprawdzić Cofnij i klawiaturę. DO ZROBIENIA.
29. Nowe zadanie: OVR/forma podczas przesuwania suwaka ma pokazywać bieżącą
    wartość w podglądzie, a nie dopiero po puszczeniu. Oddzielić podgląd edycji
    od trwałego zapisu/historii, żeby jeden gest nie tworzył setek kroków Cofnij.
    DO ZROBIENIA, sprawdzić AthleteRating.qml i odbiorców podglądu.

WIP TERAZ: nowy SeasonOverview.qml (Node kind seasonOverview) + QuickBridge.cpp.
Pięć głównych nadchodzących wydarzeń; treningi/próba w obrębie wiersza, flagi,
postęp kalendarza, liczba pozostałych krajów; istniejący przycisk startu i next
Event. Nie jest jeszcze commitowany. Błąd statycznej zależności Node→overview
→Node usunięty przez bezpośredni SButton; trwa --simulation-v2-season-only
(/tmp/sj-season-overview.log). Dokończyć test z 5+ wydarzeniami i renderami.
Następne większe rzeczy: przykład sezonu/powitanie, 30 nowych sensownych
animacji (nie liczyć już istniejących jako nowych), potem28/29. Dokładna lista
pozostałych wcześniejszych zadań i decyzji znajduje się wyżej.

30. Zmiana lądowania bez fade'a całego edytora — GOTOWE eb98941. Powodem był
    trigger var zależny od całego profilu; osobny typed string selectedProfileId
    izoluje zmianę zawodnika. Test kliknięcia lądowania sprawdza stałą instancję
    edytora i brak animacji. 28 GOTOWE19a6973; 29 GOTOWEa500c9f (przywracanie
    bindingu uwzględnia slider.pressed; jeden gest to jedna pozycja Cofnij).
18. Przegląd GOTOWY81631ee; 5 wydarzeń, treningi/próba w wierszu, flagi,
    postęp, najbliższy typ wydarzenia i akcja. Test 7 przyszłych wydarzeń
    sprawdza dokładnie5 w projekcji; --simulation-v2-season-only PASS.

31. GOTOWE: cztery poziomy siły perków: Lekko1/Wyraźnie2/Mocno3/Skrajnie4.
    Użytkownik doprecyzował: wpływ poziomu2 TAKŻE ma się zmienić. Cała stara
    krzywa jest próbkowana w punktach 0/.75/1.5/2.25/3, czyli wszystkie poziomy
    1–3 mają przeliczone wpływy, a4 zachowuje dawne ekstremum. eventDeviation
    nadal ma4 elementy (rodzaje wydarzeń), współczynniki perków mają5 (0–4).
    Commity1fc6533,6e4670e,e2251ce,125ec44; wszystkie wypchnięte.
    Profile, eksperymenty i balans wymagają perkLevels4; brak silnika3poziomów.
    Konwersja jednorazowa: scripts/convert-four-perk-levels.py, instrukcja
    docs/PERK_LEVELS_4.md. Stare1→1,2→2,3→4; współczynniki przeskalowane.
    Backup, idempotencja, mieszane zagnieżdżone eksperymenty oraz odrzucenie
    uszkodzonych danych są przetestowane. NIE konwertowano danych użytkownika
    przy otwartej grze. Po zamknięciu starszej instancji uruchomić:
      python3 scripts/convert-four-perk-levels.py --app-data --apply
    Testy core, katalogu, sezonu, obu edytorów, labu i CLI617parametrów PASS.

19. GOTOWE b6e8d1c +125ec44: powitanie oraz przycisk przykładowego sezonu
    w menu głównym. Automatyczny wybór domyślnych paczek, zimowy kalendarz
    PŚ2024-25 pod ogólną nazwą Puchar Świata, lokalne skocznie, wyczyszczone
    wyniki, nowy unikalny zapis i otwarcie edytowalnego sezonu. Test dwóch
    kolejnych uruchomień, zajętej nazwy, lokalnych profili i zachowania źródeł
    --example-season-only PASS. Bieżący etap nie odtwarza tego od nowa.

32. GOTOWE 7f6fbe9: 30 miejsc animacji nawigacji (10 wyraźniejszych i20 subtelnych).
    Lista docs/UI_MOTION_2026-10-29.md; ViewArrival.qml i PopupFade.qml.
    Skala animowana tylko na nieklikalnych nagłówkach; pola mają stałe położenie.
    NumberAnimation utrzymuje opacity/skale wątku GUI spójne z renderingiem
    także w QQuickWidget/Qt5; Animator powodował utratę kliknięć. Początek .3/.65.
    Zmiany lądowania/OVR/formy nie uruchamiają wejścia całego widoku.
    Test --classifications-only PASS po naprawie09323c5: zmiana sposobu
    punktowania niszczyła dropdown podczas jego activated/focusOut. Wywołania
    CalendarClassificationScoring.updateScoring wykonują się przez Qt.callLater.
    GDB potwierdził crash QtQml::qmlExecuteDeferred/QQuickComboBox::focusOutEvent.
    Pełny log /tmp/sj-class-deferred.log. Interakcje w czasie animacji poprawione
    przez NumberAnimation zamiast Animator; dodany rzeczywisty test kliknięcia.
    PASS: motion-feedback, dates, classifications, ko-groups, archive-history,
    race-options, simulation-v2-setup, athlete-patches, balance-lab, translation.
    Logi /tmp/sj-motion-*-final.log oraz /tmp/sj-class-deferred.log.
    Bez recursive rearrange, Binding loop, ReferenceError i TypeError.
    Wyszukiwanie KO czyści pole przed wejściem, żeby nie kasować wczesnego tekstu;
    test czeka na koniec zamykania popupu, nie na sztywne100ms.
    Test języków inicjalizuje PL przed zmianą, niezależnie od języka w skopiowanych
    ustawieniach; lokalny build/userData miał English. Danych gry nie zmieniano.
    Build gotowy w build-quick-ui/sj-sim. Wszystkie feature-commity wypchnięte.
    Test konwertera czterech poziomów i kontrola istniejących TS też PASS.
    Odświeżyć dokumentację kolejki przed kolejnym dużym zadaniem; nie wdrażać
    ponownie gotowych punktów na podstawie starszych historycznych sekcji.

Pozostałe ograniczenia do uczciwego przekazania:
- Długie nowe opisy parametrów labu wymagają jeszcze tłumaczeń EN/CS.
  check-translations.py sprawdza istniejące klucze, nie brakujące nowe źródła.
- Pełny stary smoke ma założenie o Qt Widgets database table; nowy studio
  używa własnej listy. Nie przywracać starego edytora dla spełnienia testu.
  Ukierunkowane testy nowego studia przechodzą.

## Kolejna poprawka: automatyczna konwersja paczek

Użytkownik zgłosił brak wszystkich paczek po przejściu na cztery poziomy.
Przyczyna: wymagany perkLevels4 i pozostawienie migracji jako kroku ręcznego.
Wyraźna decyzja: paczki konwertować automatycznie, sezonów dalej nie zmieniać.
Mapowanie 1→1,2→2,3→4; aktualne profile4 zachowują swoje poziomy, także3.

ProfilePackRepository::read konwertuje i waliduje cały pakiet; dla własnych
kanonicznych plików robi dokładny backup i atomowy zapis pod blokadą. Importy
zewnętrzne przelicza tylko w pamięci. HillStudioController::importFrom używa
tej samej ścieżki co import skoczków. Nieprawidłowe i przyszłe formaty nie są
zgadywane, pierwsza kopia nie jest nadpisywana, nowe i mieszane paczki są
obsługiwane bez ponownego przeliczenia aktualnych profili.

Lokalne dane przy zamkniętej grze przeliczono narzędziem ręcznym przed tą
poprawką: 4 paczki skoczków (248 profili),1 skoczni (126 profili); wszystkie
mają teraz perkLevels4 i kopie oryginałów. Narzędzie objęło też lab i1zapis
przed późniejszą decyzją o pominięciu sezonów. Nie jest to mechanizm startowy
nowej poprawki — gra konwertuje paczki natywnie w C++, bez Pythona.
Instrukcja docs/PERK_LEVELS_4.md zaktualizowana: ręczny krok dla paczek zbędny.

Weryfikacja tej poprawki: produkcyjny build Qt5, --simulation-v2-only (bridge,
repozytorium, katalog i scenariusze automatycznej konwersji), --athlete-studio-only,
--hill-studio-only oraz kontrola tłumaczeń EN/CS — PASS. Logi
/tmp/sj-auto-pack-conversion.log i /tmp/sj-auto-pack-*-studio.log.

## Kolejka 30.10 — nowe doprecyzowania

1. PRIORYTET: audyt wszystkich617opisów labu. Jasno co zmienia parametr,
   kierunek wpływu i prosty przykład. Bez pustych wstawek. 122nowe teksty
   nie miały EN/CS; najpierw uprościć PL, potem uzupełnić tłumaczenia.
   Obecne effectDescription w PlaygroundModel.cpp:193wystąpienia tekstów,
   w tym martwe duplikaty gate; field.perk dziedziczy sprzeczne ograniczenie
   do analizowanego zawodnika. Sprawdzić przykłady względem wzorów silnika.
2. Potem layout naszywek: tuż po przejściu do dwóch kolumn karty nachodzą
   na siebie. Sprawdzić próg i minimum szerokości, mały ekran, oba motywy.
3. Przy okazji: poziomy perków2i3 delikatnie odróżnić obramowaniem/wypełnieniem,
   widoczna różnica stopnia, ale znacznie słabsza od kontrastu Skrajnie4.

30.10 — audyt polskich opisów labu GOTOWY: effectDescription ma krótsze
wyjaśnienia z przykładami; wszystkie617definicji sprawdzone testem na obecność
przykładu w widocznym opisie. Wzory sprawdzone w QualityModel, FlightModel,
LandingModel oraz runnerze stawki. Nie podajemy dokładnej liczby metrów tam,
gdzie wynik jest nieliniowy: przykłady rozróżniają początkowy zapas i odległość.
Usunięto martwe duplikaty opisów belki, sprzeczność field.perk i zbędne zdania.
Test --balance-lab-only PASS (/tmp/sj-lab-copy-20261030.log).
Nadal uzupełnić EN/CS nowych opisów. Następne: layout i subtelne poziomy2/3.
