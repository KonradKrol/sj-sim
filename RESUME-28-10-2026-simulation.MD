# Wznowienie prac nad nową symulacją — 28.10.2026

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
2. **Końcówkę** [`docs/SIMULATION_V2_PROGRESS.md`](docs/SIMULATION_V2_PROGRESS.md)
   — szczegółowy aktualny stan, ukończone etapy, testy i kolejka. Starsze sekcje
   opisują wcześniejsze etapy, czasem już zastąpione nowszymi.
3. [`docs/SIMULATION_V2_ENGINE.md`](docs/SIMULATION_V2_ENGINE.md) — model, wzory,
   parametry i integracja.
4. [`docs/WIND_MEASUREMENT_RESEARCH.md`](docs/WIND_MEASUREMENT_RESEARCH.md) oraz
   [`docs/WEATHER_RESEARCH.md`](docs/WEATHER_RESEARCH.md) — źródła i granice
   założeń dotyczących pomiaru oraz deszczu.
5. [`docs/TRANSLATION_AUDIT_2026-10-28.md`](docs/TRANSLATION_AUDIT_2026-10-28.md)
   — naprawy tłumaczeń i świadomie pozostawiony szerszy audyt.
6. [`docs/RELEASE_NOTES_VS_MAIN.md`](docs/RELEASE_NOTES_VS_MAIN.md) — dopisywać
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
