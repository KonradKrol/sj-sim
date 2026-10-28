# Simulation v2 — postęp i punkty wznowienia

**Ostatnia aktualizacja: 28.10.2026. To główny plik przekazania pracy kolejnej
sesji AI, także na innym komputerze po `git pull`. Zadanie nie jest zakończone.**

## Start kolejnej sesji

1. Sprawdź `git status --short --branch`. Pracuj na istniejącej gałęzi
   `feature/simulation-v2`; pobieraj jej zmiany bez nadpisywania lokalnej pracy.
   Nie modyfikuj maina. Nie zaczynaj implementacji silnika ponownie.
2. Przeczytaj ten plik, pełne [NEW_SIMULATION.md](../NEW_SIMULATION.md)
   i [opis silnika](SIMULATION_V2_ENGINE.md). Odpowiedzi na 12 pytań są już
   rozstrzygnięte w specyfikacji, nie trzeba ich ponownie zadawać.
3. Pracuj samodzielnie, **bez subagentów** — wyraźna prośba użytkownika.
   Kontynuuj pełen zakres, nie skracaj go do prototypu. Użytkownik zgodził się
   poświęcić tyle czasu, ile wymaga poprawne wdrożenie.
4. Przy QML korzystaj z umiejętności w `.agents/skills`, szczególnie
   `qt-qml` i `qt-ui-design`. Zachowaj zgodność z Qt 5.15 i Qt 6; importy
   QML mają wersje 2.15/1.15. Nie wprowadzaj unversioned imports lub importu
   `QtQuick.Controls.Basic`, który zepsuje tutejszy Qt 5.
5. Zakończone etapy commituj i wypychaj na tę gałąź. Aktualizuj ten plik przy
   postępie i przed zakończeniem sesji. Nie polegaj na historii czatu,
   lokalnych logach `/tmp`, wygenerowanych binarkach ani niedodanych plikach.

### Co już można sprawdzić po pull

- Nowy edytor skoczni: czwarta para perków dominującego kierunku wiatru;
  stare nowe-paczki z trzema parami czytają się z neutralną czwartą.
- Edytory zawodników i skoczni otwierają się bezpośrednio. Na wyraźne żądanie
  użytkownika **usunięto przełącznik stary/nowy — nie przywracać go**.
- Samodzielny kompletny rdzeń skoku: testy ze skryptu poniżej.
- Adapter gry i migracja: test `--simulation-v2-only`.
- **Pojedyncze skoki korzystają już z nowego silnika.** Konfiguracja w tle
  wczytuje kanoniczne paczki v2, a dawne dane migruje do oddzielnych przybliżonych
  kopii. Można wybrać paczkę, zawodnika, skocznię, rodzaj skoku i edytować lokalną
  kopię pełnego nowego profilu bez zmiany źródła. Belka na wejściu jest wyliczana.
- Konkursy i nowe sezony nadal wymagają podłączenia katalogu; dawne dane w tych
  ścieżkach wciąż uruchamiają stary silnik. Nie deklaruj zakończenia całej migracji.

### Budowanie i odtwarzanie testów

Wszystkie polecenia uruchamiaj w głównym katalogu repo:

```bash
scripts/test-simulation-v2-core.sh
scripts/build-quick-ui.sh
scripts/test-quick-ui.sh --simulation-v2-only
scripts/test-quick-ui.sh --simulation-v2-setup-only
scripts/test-quick-ui.sh --hill-studio-only
scripts/test-quick-ui.sh --engine-audit-only
scripts/test-quick-ui.sh --config-defaults-only
scripts/test-quick-ui.sh --save-storage-only
```

Skrypt Quick Test przygotowuje izolowane dane testowe. Pełny domyślny smoke-test
jest duży; do etapu dobieraj konkretne przypadki. `SJ_BUILD_JOBS` steruje liczbą
zadań kompilacji. Lokalnie używano Qt 5.15.3 na Linuksie; staging Qt jest
automatycznie wykrywany przez skrypty. **Nie zweryfikowano tego etapu na Qt 6
ani Windowsie.** Instrukcja Windows: [WINDOWS_SETUP.MD](../WINDOWS_SETUP.MD).

Wyniki dotychczasowych sprawdzeń:

- Rdzeń: PASS — rozkłady i ich momenty, krzywa K, perki i proporcja wpływu
  65/35, ciągłość/korelacja/podmuchy pogody, zbieżność kroku lotu, 4000 losowych
  skrajnych konfiguracji, punktacja, DSQ/DNS, deterministyczna praca równoległa.
- Kalibracja: 20 tys. par seedów, OVR90/70, forma70, K120/HS134, belka10,
  cisza i neutralne perki → średnia różnica **15,3476 m** (cel 14–17 m).
- Rdzeń z AddressSanitizer i UndefinedBehaviorSanitizer: PASS. To sprawdzenie
  samodzielnego rdzenia, nie całej aplikacji Qt.
- Adapter `--simulation-v2-only`: PASS — paczki, migracja bez mutowania źródła,
  profile, tożsamość obiektów, ślad wiatru po serializacji, preview i manipulacja.
- Nowy edytor skoczni: PASS, rendery desktopu i małego ekranu.
- Stary silnik: wszystkie 10 przypadków `--engine-audit-only` PASS.
- Domyślne ustawienia konfiguracji oraz zapis/kolejność symulacji: PASS.

Nie traktuj czasu samego testu 40 tys. spokojnych skoków jako benchmarku gry:
nie obejmuje on tabel, JSON, archiwum ani zmiennego pola z UI.

### Mapa implementacji

| Plik/moduł | Rola |
| --- | --- |
| `source/simulation-v2/RandomSource.*` | Wstrzykiwany RNG, seedy, normalny/Laplace, brak globalnego stanu rdzenia |
| `HillSkillCurve.*`, `QualityModel.*` | Krzywa K, OVR/forma, perki, kategoria wydarzenia i asymetria wpływu |
| `Weather.*` | Ciągłe, deterministyczne pole, gusty, opady, automatyczne propozycje |
| `FlightModel.*` | Zapas, krok przestrzenny, reakcja na wiatr, rosnący koszt geometrii |
| `LandingModel.*` | Jawne krzywe ryzyka i noty; klasa lądowania wpływa wyłącznie na noty |
| `Scoring.*` | Osobna strategia pomiaru, jawne sensory, punkty, belka/trener i 95% HS |
| `Manipulation.*`, `Engine.*` | Złożenie skoku, preview i jawne wymuszenia |
| `ProfileBridge.*` | Mapowanie profili nowych edytorów na wejście rdzenia |
| `ProfileMigration.*` | Osobne przybliżone paczki, świeże ID, źródła i mapowanie zdjęć |
| `ProfilePackRepository.*` | Wspólny zapis edytorów, atomowa migracja i kopiowanie zdjęć bez nadpisywania źródeł |
| `SimulationCatalog.*` | Gotowy katalog źródeł v2, preferowanie nowych paczek, wybór/kolejność bez duplikowania profili |
| `GameSimulationBridge.cpp` | Implementacja ścieżki v2 `JumpSimulator` |
| `source/simulator/Jumper.*`, `Hill.*` | Opcjonalny pełny profil v2 obok starego modelu; stabilna tożsamość wyników |
| `source/simulator/JumpData.*` | Trwały JSON danych v2 skoku i pogody |
| `source/global/DataPack.*` | Odczyt nowych paczek, wersja katalogowa 2, pochodzenie przybliżonej migracji |
| `source/hills/*`, `source/UI/Quick/Hill*` | Czwarta para kierunku, odczyt dawnych 3 par |
| `tests/simulation-v2-*.cpp` | Testy niezależnego rdzenia |
| `tests/simulation-v2-bridge-regressions.h` | Testy adaptera przez aplikację Qt |

Pliki rdzenia i adaptera są już dodane do `sj-sim.pro`. Projekt używa qmake.

### Pułapki, których nie pomijać przy dalszej integracji

- Nowe edytory przechowują własne paczki: `AthleteStudioController` domyślnie
  w `QStandardPaths::AppDataLocation/athlete-studio/<id>/pack.json`, a
  `HillStudioController` w `.../hill-studio/<id>.json`. Oba używają już
  `ProfilePackRepository`, a `SimulationCatalog` czyta te same pliki.
  Pojedyncze skoki już używają katalogu; pozostają konkursy i sezony. Nie twórz drugiej
  niezależnej kopii edytowanych parametrów w `GlobalDatabase` lub indeksie.
- `GlobalDatabase` nadal czyta/pisze dawne `userData/GlobalDatabase` i ma kopie
  aktywnych wektorów. Przed migracją przejrzyj `loadPacks`, `writePacks`,
  `syncActivePacks`, `selectPack` oraz pozostałe load/write globalnych danych.
  Zachowaj źródła i sprawdź referencje kalendarzy/list skoczków po zmianie ID.
- Konfiguracja musi zablokować start mieszanych wersji, zanim adapter rzuci
  wyjątek. W dalszym etapie nowe profile mają być domyślne. Nie zostawiaj
  widocznych starych edytorów umiejętności, których nowy silnik nie odczytuje.
- `HillEditorWidget` zachowuje nowy profil przy tworzeniu konfiguracji. Podobne
  ścieżki kopiowania/rekonstrukcji muszą zachować profil i jego tożsamość.
- `JumpSimulator` ma lokalny zegar z krokiem 30 s i setter czasu; to jeszcze
  nie zapisany zegar konkursu. DNS i przerwy też muszą wpływać na czas.
  Batchom przypisuj seed/czas po indeksie, nie numerze pracownika.
- `setSimulationWeather` wymaga pola i strategii pomiaru razem.
  `setSimulationParameters` waliduje pełny zestaw parametrów nowego silnika.
- Dawne ratingi i ustawienia `GlobalSimulationSettings` nie sterują nowym
  OVR/formą. Nowe UI oraz playground muszą pokazywać właściwe parametry,
  a nie pozostawiać działających pozornie kontrolek starego modelu.
- Sam preview odległości jest nowy, lecz wybór „najlepszego” zawodnika do
  automatycznej belki w `CompetitionConfigWindow::suggestedGate` i dawnym
  playgroundzie nadal korzysta z technik starego modelu. Przenieś też selektor
  najlepszego zawodnika na jakość v2, nie tylko wywołanie `previewDistance`.
- Dokładny pomiar FIS nadal jest niewyjaśniony. Są źródła i punktacja, ale
  nie potwierdzone dokładne wagi/okna. Nie nazywaj eksperymentalnej strategii
  sensorów „dokładnym FIS” ani nie utożsamiaj jej z ekspozycją zawodnika.
- `ProfileMigration` jest czystą konwersją/mapowaniem; `ProfilePackRepository`
  wykonuje już atomowy zapis i kopiowanie zdjęć. `SimulationCatalog` nadaje
  zawodnikom kontekst folderu paczki. `Jumper::getImageFilePath` pilnuje granicy
  folderu; JSON zapisuje tylko `simulation-pack-id` i względną ścieżkę.
  Po wczytaniu sezonu trzeba jeszcze przywrócić kontekst assetów z ID paczki;
  domenowy `getFromJson` celowo nie wyszukuje plików na danym komputerze.
- Nowe profile są już serializowane, ale nowy format/wersjonowanie sezonu,
  komunikat o nieobsługiwanych starych zapisach i migracja domyślnego katalogu
  nie zostały wdrożone. Nie deklaruj zakończonej zgodności zapisów.

Poniżej znajduje się pełny postęp i kolejka. Najbliższe zadanie to podłączenie
`SimulationCatalog` i gotowego UX pogody do konkursów i nowych sezonów; potem playground. Nie zaczynaj od ponownego strojenia gotowego rdzenia.

### Zakończone commity tego etapu

| Commit | Zmiana |
| --- | --- |
| `8969679` | Parametry jakości i monotoniczna krzywa K |
| `b99be3e` | Ciągła pogoda i automatyczne propozycje |
| `13954d0` | Skalibrowany lot, lądowanie i punktacja |
| `eedff18` | Czwarta para kierunku wiatru w modelu i edytorze skoczni |
| `24b5224` | Jawne manipulacje zachowujące prawidłowy ślad wiatru |
| `274c68d` | Neutralność bez perka i walidacja zdegenerowanej krzywej |
| `b99afd4` | Zapisywalne rekordy skoczni niestandardowych i jasny opis migracji |
| `00e0fb2` | Adapter gry, profile/paczki v2, migracja i zapis historii pogody |

Szczegóły każdego etapu są w `git show <commit>`. Ten wykaz nie zastępuje
checklisty prac pozostałych poniżej.

Gałąź: `feature/simulation-v2`, utworzona z `6095d31` gałęzi
`feature/qml-competition-config`. Wymagania: [NEW_SIMULATION.md](../NEW_SIMULATION.md).
Stan: gotowy i przetestowany rdzeń v2 oraz adapter nowych profili do gry.
Wytyczne i wszystkie 12 odpowiedzi zapisano w specyfikacji. **Pełna migracja
nie jest zakończona**: dotychczasowe dane nadal używają starego silnika,
nowy wybierany jest dla obiektów z profilami v2. Do przełączenia domyślnego
pozostają katalog, konfiguracja, sezony, UX pogody/manipulacji i playground.

## Ustalenia z kodu przed integracją

- `AthleteProfile` zawiera OVR, formę, klasę lądowania i trzy pary perków z siłą
  1–3. `HillProfile` ma K, HS, punktację i trzy pary perków; wyprowadza real HS,
  preferencję wybicia/lotu i zmienność wiatru. Oba modele służą obecnie wyłącznie
  do tworzenia danych w nowych edytorach.
- Konkursy i zapisy nadal używają `Jumper`, `JumperSkills` i `Hill`.
  `DataPack` oraz `SimulationSave` zapisują te klasy. Samo przełączenie edytora
  nie podłączy nowych modeli do gry.
- `JumpSimulator` łączy losowanie jakości wybicia i lotu, opady, odległość,
  sektorowy wiatr, ograniczanie bardzo dalekich skoków, lądowanie, noty,
  rekompensaty i punktację. Czyta ustawienia globalne oraz cechy starego modelu.
- Silnik jest wywoływany w konkursach, pojedynczych skokach i playgroundzie.
  Podgląd odległości służy też do automatycznego doboru belki; te ścieżki muszą
  zostać przeniesione razem i korzystać ze spójnej konfiguracji.
- Obecny playground uruchamia stary silnik i sam losuje pięć sektorów wiatru.
  Nowy playground musi korzystać z tego samego nowego silnika i pogody co gra.
- Aktualny generator formy operuje na starych umiejętnościach. Automatyczny
  rozwój formy w sezonie został w aktualnej specyfikacji odłożony: nie wdrażać
  go na podstawie wcześniejszych pomysłów.

## Rozkłady używane obecnie

Źródła: `source/global/MyRandom.cpp`, `source/simulator/JumpSimulator.cpp`,
`source/simulator/wind-generation/WindsGenerator.cpp`,
`source/simulator/InrunSnowGenerator.cpp` i `source/form-generator/JumperFormGenerator.cpp`.

| Rozkład / mechanizm | Faktyczne zastosowanie |
| --- | --- |
| Jednostajny, liczby całkowite i zmiennoprzecinkowe | Wybory losowe, DSQ, wybór strony odchylenia i rodzaju lądowania |
| Normalny | Noty sędziowskie, wiatr i opady |
| Jednostronny / ucięty normalny; przy zerowej bazie półnormalny | Dodatnie i ujemne odchylenia jakości, niestabilność lądowania, formy i ustawień wiatru |
| Lognormalny | Dodatkowe skracanie bardzo dalekiego skoku po przekroczeniu real HS |
| Ważony wybór kategorii | Telemark, dwie nogi, podpórka, upadek |

Funkcja losowania gamma istnieje, ale nie znaleziono jej aktywnego wywołania
w tych ścieżkach. Pomocniczy rozkład wykładniczy w samplerze uciętego normalnego
jest szczegółem algorytmu losowania, a nie osobnym czynnikiem jakości skoku.

Do nowego modelu warto porównać zwykłe odchylenia normalne z Laplace'em oraz
z mieszaniną bazowego odchylenia i osobnych zdarzeń negatywnych/pozytywnych.
Ostatecznego wyboru nie dokonywać wyłącznie na podstawie wyglądu histogramu.
Cauchy nie jest domyślną rekomendacją dla jakości skoku: nie ma skończonej
średniej i wariancji; ograniczenie jego ogonów tworzyłoby osobny, wymagający
kalibracji model. Może pozostać kandydatem do eksperymentu lub testów skrajnych.
[Cauchy — Boost](https://www.boost.org/doc/libs/latest/libs/math/doc/html/math_toolkit/dist_ref/dists/cauchy_dist.html),
[Laplace — Boost](https://www.boost.org/doc/libs/latest/libs/math/doc/html/math_toolkit/dist_ref/dists/laplace_dist.html).

## Plan wykonania

- [x] Przeczytać aktualną specyfikację i rozpocząć mapowanie kodu.
- [x] Utworzyć osobną gałąź bez zmieniania maina.
- [x] Zidentyfikować rozkłady i główne punkty podłączenia silnika.
- [x] Zadać 12 pytań po wstępnej analizie.
- [x] Przygotować i przetestować niezależne źródło losowości dla nowego rdzenia.
- [x] Zapisać odpowiedzi i nowe wytyczne dotyczące konfiguracji wiatru.
- [x] Zapisać automatyczne propozycje, prosty/zaawansowany wybór wiatru,
  opcjonalny podgląd oraz czwartą parę perków dominującego kierunku.
- [x] Uzgodnić migrację paczek, gotowych nowych profili oraz zapisów sezonów.
- [x] Rozszerzyć skocznie o nową parę kierunku wiatru; stare zestawy trzech par
  odczytywać z neutralnym nowym perkiem. Wpływ kierunku i loteryjności skoczni
  stosować do automatycznych propozycji, bez ukrytych zmian ręcznych ustawień.
- [x] Przygotować niezależny od UI rdzeń: wejście skoku, kontekst wydarzenia,
  konfiguracja współczynników i wynik z danymi do analizy.
- [x] Oddzielić przez interfejsy źródło losowości i warunki pogodowe od rdzenia;
  unikać mutowalnych globalnych ustawień w pracy równoległej.
- [x] Zaimplementować OVR, niezależną formę, perki zawodnika i skoczni oraz
  ważność wydarzenia jako kategorię, zamiast starej liczby.
- [x] Zaimplementować odległość, belkę i skończony zapas lotu z konfigurowalnymi
  współczynnikami, bez rzeczywistej trajektorii fizycznej.
- [x] Dodać ciągłą pogodę w przestrzeni i umownym czasie konkursu, korelacje,
  lokalne różnice i gwałtowne podmuchy. Ręczne i masowe symulowanie nie może
  zależeć od szybkości klikania ani czasu renderowania UI.
- [ ] Zweryfikować rzeczywistą metodę uśredniania pomiarów wiatru i oddzielić
  ją od wpływu wiatru na lot oraz przeliczania rekompensaty.
- [ ] Zapisać przybliżony podgląd korytarzy konkretnego skoku, możliwy do
  odtworzenia w konkursie i archiwum także po zmianie algorytmu.
- [ ] Dodać opady 0–10, zmiany w konkursie i pięć prostych scenariuszy.
  Kandydat na piąty scenariusz: narastające opady. Sugestie daty/kraju mają być
  edytowalne przed konkursem; nie traktować ich jako dokładnej prognozy pogody.
- [x] Przebudować lądowanie i noty w rdzeniu: płynne ryzyko w zależności od real HS,
  wpływ klasy lądowania tylko zgodny ze specyfikacją, kalibracja wysokich not.
- [ ] Przenieść punktację, DSQ/DNS, podgląd odległości i automatyczną belkę.
- [ ] Podłączyć nowe modele do bazy, konfiguracji i konkursów indywidualnych,
  drużynowych, KO, pojedynczych skoków oraz sezonów; zachować poprawne wyniki
  historyczne według uzgodnionej migracji.
- [ ] Uprościć ustawienia pogody i przebudować manipulację skoku zgodnie
  z uzgodnionym zakresem; zweryfikować małe ekrany, motywy i Qt 5/Qt 6.
- [ ] Zbudować nowy playground: porównanie wariantów, analiza zawodnika,
  rozkładów miejsc/odległości/not, upadków, wiatru i rekompensaty; parametry
  silnika dostępne do strojenia bez rozsypywania zwykłych ustawień gry.
- [ ] Zapewnić wielowątkowe serie, anulowanie, postęp, pamięć ograniczoną
  wielkością prób i agregatów oraz możliwość eksperymentów przez CLI.
- [ ] Dodać testy granic, migracji i integracji, porównania statystyczne,
  powtarzalność przy różnej liczbie wątków i pomiary wydajności.
- [ ] Udokumentować końcowe wzory, rozkłady, jednostki, współczynniki,
  format danych i procedurę strojenia; commitować zakończone etapy.

## Zatwierdzone decyzje

Nowy silnik dla nowych sezonów, bez wymogu zgodności zapisów z 1.3.0-beta2.
Konwersja starych paczek ma być wyraźnie opisana jako niedokładne przybliżenie;
nie nadpisywać gotowych nowych paczek. Zachować trzy poziomy perków.
Początkowy udział OVR/formy 70/30 jest regulowany. 65/35 dotyczy łącznego
ujemnego/dodatniego wpływu losowości, nie częstości ani procentowej straty
odległości. Wstępny cel różnicy OVR 90/70 na K120: 14–17 m.

Bez skracania wyniku po fakcie, ale z narastającą trudnością uzyskania metrów
w okolicach real HS i ograniczeniem wynikającym z końca użytecznego zeskoku.
Krzywe lądowania/upadku i not muszą być dostępne do analizy. Udziały wybicia/lotu
mają przechodzić przez punkty podane przez użytkownika w `NEW_SIMULATION.md`.
Typ opadów zgodny z nawierzchnią, manipulacje zgodne z nowym modelem zachowane,
szybki automatyczny podgląd playgroundu plus duże serie na żądanie.

## Pierwszy element rdzenia: losowość

`source/simulation-v2/RandomSource.h/.cpp` udostępnia interfejs źródła losowości
oraz lokalny sampler z seedem. Rdzeń może otrzymać inne źródło, np. kontrolowane
w testach, bez odwoływania się do globalnego generatora lub obiektów UI.
`deriveSeed` pozwala wydzielać niezależne strumienie dla eksperymentów i zdarzeń.
Sam moduł nie narzuca jeszcze przypisywania streamów do zawodników i pogody.

Pierwsza implementacja używa SplitMix64, otwartego przedziału (0, 1), Boxa–Mullera
dla rozkładu normalnego oraz odwrotnej dystrybuanty dla Laplace'a. Dostępne są
także rozkład jednostajny, zdarzenie o podanym prawdopodobieństwie i wybór
ważony; duże wagi są skalowane przed sumowaniem. Błędne parametry są odrzucane,
a zerowa skala zwraca wartość stałą bez zużywania losowania. Funkcje rozkładów
nie ustalają jeszcze, którego rozkładu będzie używać gotowy model skoku.

Generator nie ma stanu globalnego; osobne instancje można przydzielać zadaniom.
Testy potwierdzają takie same próbki przy zmianie kolejności odbierania wyników
ośmiu zadań. To nie obietnica identyczności ostatnich bitów obliczeń `log`, `cos`
i innych funkcji matematycznych między różnymi platformami. Powtórki muszą
uwzględniać wersję silnika i zapisane wyniki/historyczne pomiary.

Weryfikacja: `scripts/test-simulation-v2-core.sh` — PASS. Obejmuje otwarte granice,
walidację, zerowe skale, wagi zerowe i bardzo duże, momenty 200 tys. próbek oraz
powtarzalność równoległą. Moduł był początkowo testowany samodzielnie;
obecnie jest też częścią projektu qmake i adaptera gry opisanego poniżej.

## Jakość i krzywa K

Zaimplementowano `HillSkillCurve` (edytowalne punkty, monotoniczna interpolacja
PCHIP, jawne zachowanie poza zakresem) i `QualityModel`: OVR/forma, trzy pary
perków, kategorie ważności oraz parametryzowany rozkład normalny/Laplace'a.
Podział 65/35 dotyczy siły wpływu, nie częstości losowania. Klasa lądowania
nie wpływa na jakość. Adapter gry korzysta z nich dla nowych profili.

Testy przechodzą, w tym porównania po 200 tys. próbek. Opis wzorów i początkowych
współczynników: [SIMULATION_V2_ENGINE.md](SIMULATION_V2_ENGINE.md).

## Pole pogody

Gotowy niezależny interfejs `WeatherField` oraz `ContinuousWeather`: referencje,
korelacje czasu/przestrzeni, przemieszczające się podmuchy i ograniczenie siły.
Odczyty są niezmienne i deterministyczne. Gotowe pięć scenariuszy opadów 0–10
oraz propozycje warunków silnie zależne od obu perków pogodowych skoczni.
Perki nie są argumentem pola, więc nie nadpisują ręcznie wybranych warunków.

Testy granic, ciągłości, korelacji, odczytów równoległych i rozkładu propozycji
przechodzą. Adapter gry potrafi przyjąć wspólne pole oraz strategię pomiaru;
UI konfiguracji nie ustawia jeszcze nowego modelu. Dokładny algorytm FIS
pomiaru do rekompensaty wymaga dalszej weryfikacji.

## Gotowy samodzielny rdzeń skoku

`Engine` łączy jakość, lot, pogodę, lądowanie, noty i punktację. Zawiera DSQ/DNS,
deterministyczny podgląd odległości, osobne obniżenie belki jury/trenera i próg
95% HS. Wynik zawiera opcjonalny ślad ekspozycji w sekcjach; adapter zapisuje
go już w `JumpData`, a pokazanie w UI pozostaje do zrobienia. Strategia pomiaru jest
oddzielna, dostępna wersja z jawnymi stanowiskami/wagami jest eksperymentalna,
nie potwierdzona jako dokładny algorytm FIS.

`scripts/test-simulation-v2-core.sh` — PASS, m.in. 4000 losowych skrajnych
konfiguracji, zbieżność kroku, brak wpływu nieosiągniętego podmuchu, geometria,
landing skill tylko w notach oraz zgodność wyników przy pracy równoległej.
Cel OVR90/70 na K120: **15,35 m** średniej różnicy w 20 tys. sparowanych seedów.
Rdzeń jest w projekcie qmake i ma adapter gry; domyślna migracja danych
oraz przebudowa konfiguracji i playgroundu pozostają do zrobienia.

## Czwarta para perków skoczni

Nowy edytor skoczni ma parę „Dominuje wiatr przedni / tylny”, z trzema siłami,
spójnymi ikonami i opisem dotyczącym wyłącznie automatycznych propozycji.
`HillProfile` zapisuje cztery pary; odczytuje też trzy, zachowując identyfikator
i wszystkie dawne wybory, z neutralnym kierunkiem. Kontroler rozszerza stary
zestaw przy pierwszej edycji. Qt Quick używa liczby par z katalogu.

`scripts/test-quick-ui.sh --hill-studio-only` — PASS na Qt 5.15.3, w tym
serializacja obu formatów, poziomy, zapis/undo i responsywne rendery.
Sprawdzono wygląd renderu 1440 px. Perki będą działały w grze po integracji
nowych profili oraz konfiguracji pogody; obecnie działają w propozycjach rdzenia.

## Adapter gry i paczki v2

- `Jumper` i `Hill` mogą przenosić pełny nowy profil, zachowując dotychczasową
  tożsamość używaną przez wyniki, KO, drużyny i archiwum. Serializacja zapisuje
  nowy profil; odczyt paczek i sezonów sprawdza jego poprawność oraz zgodność ID.
- `DataPack` przyjmuje gotowe paczki z nowych edytorów i zapisuje zawartość v2
  z wersją 2. Dotychczasowe paczki nadal są czytane jako wersja 1. Same nowe
  edytory oraz katalog v2 używają już tych samych plików; okna gry wymagają
  przełączenia źródła danych na `SimulationCatalog`.
- `JumpSimulator` kieruje nowe profile do `Engine`, bez odczytywania starych
  technik zawodnika. Kategorie wydarzenia są przekazywane z konkursu. Mieszane
  nowe/stare profile są odrzucane, nie przeliczane potajemnie. Przy przełączeniu
  katalogu trzeba zapewnić zgodny wybór obu paczek i walidację przed startem.
- Adapter przyjmuje niezmienne pole pogody i strategię pomiaru. Do czasu nowego
  UX potrafi też zbudować gładkie pole z dawnych ustawień korytarzy, zachowując
  dotychczasowy sposób pomiaru do rekompensaty. To przejściowa ścieżka.
- Podgląd belki używa tego samego nowego modelu. Przeniesiono zgodne wymuszenia
  odległości, opadów, pomiaru wiatru, lądowania, not i DSQ. Osobne dawne
  ratingi wybicia/lotu nie mają odpowiedników; zastąpi je korekta jakości w nowym UX.
- `JumpData` zapisuje wersję silnika, seed jako tekst, czas/kategorię, rozkład
  jakości, zapas i surową odległość, opady, rzeczywistą ekspozycję i odczyty
  przed/po. Odczyt nie odtwarza pogody z seeda. Reset usuwa te dane.
- `ProfileMigration` tworzy osobną paczkę i nowe ID, zachowuje pochodzenie,
  przelicza OVR/formę i przybliża lądowanie, zeruje perki. Wynik zawiera wyraźne
  ostrzeżenie o niedokładności i mapowanie zdjęć do unikalnych ścieżek w paczce.
  Gotowe profile v2 nie są migrowane ponownie. Funkcja nie zapisuje plików ani
  nie kopiuje zdjęć — tę część wykonuje już `ProfilePackRepository`.
- Zakres zapisu rekordów w nowych profilach rozszerzono do 1000 m, żeby legalna
  duża niestandardowa skocznia/eksperyment nie tworzyły nieserializowalnego profilu.

Test `--simulation-v2-only`: profil/paczka, migracja bez zmiany źródła,
tożsamość wyniku, historia pogody po serializacji, preview, wymuszenia,
przekazanie pola/czasu i konfiguracja skoczni zachowująca nowy profil.
Testy rdzenia z AddressSanitizer/UndefinedBehaviorSanitizer również przechodzą.

## Następne zadania — wznowienie

1. **Serwis katalogu i wspólny zapis już są gotowe.** Podłączyć `SimulationCatalog`
   do konfiguracji konkursu i nowego sezonu. Pojedyncze skoki są już podłączone. Nie zaczynać nowego katalogu od zera. Wyraźnie
   pokazać ostrzeżenie o przybliżeniu oraz pochodzenie w edytorze i wyborze paczki.
2. Przełączyć bazę i konfigurację nowych konkursów, sezonów oraz pojedynczych
   skoków na zgodne paczki v2. Dopilnować, żeby dawne widgety nie edytowały
   ignorowanych umiejętności i nie traciły nowego profilu. Przekazać wszystkie
   parametry oraz seed i logiczny czas po indeksie zadania w batchach.
3. Nowy format sezonu/wersja silnika i jasne odrzucenie nieobsługiwanych starych
   zapisów bez modyfikacji źródła. Zachować poprawne relacje i historię.
4. Nowy UX pogody/opadów: propozycja przed konkursem, dominujący kierunek/siła,
   loteryjność i podobieństwo zeskoku, pięć scenariuszy opadów, zaawansowane
   referencje. Przechowywać ustawienia i zegar konkursu; DNS także przesuwa czas.
5. Zweryfikować dokładne ważenie i okno pomiarów FIS. Do tego czasu nie deklarować
   zgodności eksperymentalnego pomiaru stanowiskowego z pełnym algorytmem FIS.
6. Podgląd sektorów na życzenie w karcie skoku/archiwum. Domyślnie tylko średnia
   wiatru. Nowa analiza jakości zastępuje nieaktualne ratingi wybicia/lotu.
7. Nowy UX manipulacji. Następnie pełny nowy playground/CLI, wszystkie współczynniki,
   porównania, rozkłady miejsc, krzywe lądowania/not, automatyczny anulowalny
   podgląd w tle, duże równoległe próby z ograniczoną pamięcią.
8. Testy całości indywidualne/drużyny/KO, seriami ręcznie/automatycznie, zapisem
   i ponownym odczytem, małe ekrany/motywy/Qt 5 i docelowe Qt 6/Windows.

## Wspólny zapis i katalog — ukończona warstwa danych

`ProfilePackRepository` jest wspólną warstwą odczytu/zapisu nowych edytorów.
Przechowuje istniejące formaty i dodatkowe metadane, waliduje ID/nazwę/profile,
pilnuje 20 MB, zapisuje przez `QSaveFile`. Ścieżka szybkiego zapisu perków nie
waliduje ponownie wszystkich zawodników, ale zawsze kontroluje nagłówek i ID.
Pustej nazwy paczki nie można już zapisać tak, by paczka znikła po restarcie.

`migrate` jest idempotentne po źródłowym ID. Wykorzystuje blokadę między procesami,
folder/plik tymczasowy i publikację przez rename. Nie nadpisuje gotowych nowych
paczek ani źródeł. Zdjęcia kopiuje do folderu nowej paczki, z unikalnym
podfolderem zawodnika; odrzuca pliki poza dozwolonym folderem źródłowym. Brak
opcjonalnego zdjęcia zgłasza bez usuwania zawodnika. Uszkodzony plik nie ukrywa
poprawnych paczek i nie jest automatycznie nadpisywany.

`SimulationCatalog::load(legacyPacks, maxSkills, legacyActiveIds)`:

- Wczytuje istniejące nowe paczki, przelicza nieprzeniesione stare do oddzielnych
  paczek, zachowuje gotowe nowe profile i kompatybilne paczki zasad.
- Preferuje gotowe nowe paczki przed przybliżonymi, ale zachowuje jawny wybór
  użytkownika. Odwzorowuje dawne aktywne paczki na ich przybliżone kopie.
- Zwraca `CatalogSnapshot` z własnymi wartościami: zmiana pliku w edytorze
  wpływa na następne wczytanie, bez podmieniania danych trwającej sesji.
- Zapisuje tylko kolejność i ID w `.../simulation-v2/catalog.json`, bez drugiej
  edytowalnej kopii profili. Uszkodzonego indeksu nie nadpisuje; poprawne paczki
  pozostają dostępne. Błędy/uwagi zwraca w `snapshot.issues`.
- Zawodnikom nadaje `simulation-pack-id` i nietrwały kontekst folderu zdjęć.
  Identyfikator można zapisać, lokalnej ścieżki katalogu nie zapisuje się w JSON.

**Jest już wywoływany przez `SingleJumpsConfigWindow`; konkursy i sezony jeszcze go nie używają.** Testy
korzystają z jawnych tymczasowych ścieżek. `CatalogPaths::defaults()` odpowiada
obecnym folderom nowych edytorów; ustal organizację/nazwę aplikacji przed jego
wywołaniem (przyszły CLI powinien używać tych samych ścieżek co gra albo przyjąć
je jawnie). Gotowa nowa paczka zaimportowana dawniej do `GlobalDatabase` zostaje
przeniesiona bez przybliżania; jeśli nieznany jest folder jej zdjęć, katalog
zwraca uwagę o konieczności sprawdzenia plików zamiast kopiować przypadkowe zdjęcia.

Testy `--simulation-v2-only`: PASS również dla repozytorium i katalogu, w tym
atomowa migracja, ponowienie bez duplikatów, zdjęcia o tych samych nazwach,
brak/wyjście poza folder zdjęć, edycja przez studio widoczna w repozytorium,
ochrona źródeł i uszkodzonego indeksu, stabilny wybór i brak drugiej kopii profili,
nowe importy bez ponownego przybliżania, odświeżanie snapshotu i przenośny JSON
kontekstu zdjęć. `--athlete-patches-only` i `--hill-studio-only` również PASS.

## Pojedyncze skoki — podłączone do nowego modelu (28.10.2026)

- `SingleJumpsConfigWindow` wczytuje snapshot `SimulationCatalog` w tle przez
  QtConcurrent. Do ukończenia odczytu start jest zablokowany; zamknięcie okna nie
  pozostawia callbacku do martwego UI. Testy przekazują własne `CatalogPaths`.
- Wybór paczki i jej pozycji jest jawny. Źródło migracji i niedokładność są
  wyświetlane, gdy wybrano przybliżoną paczkę. Liczne uwagi o brakujących zdjęciach
  nie zalewają formularza — jest liczba uwag i szczegóły na życzenie.
- `SimulationProfileDraft` przechowuje lokalną kopię zawodnika/skoczni i oryginał
  do przywrócenia. Ścisła walidacja, pełne nowe parametry/perki, zdjęcia względne,
  żadnych zapisów do paczek. Przełączenie wyboru odświeża cały profil.
- Quick korzysta z `SimulationDraftWorkspace`, `AthletePatches` i `HillEditor`.
  Zachowany frontend Widgets ma mały `SimulationProfileEditor` z tym samym
  backendem; stare komponenty pozostały w repo, ale nie edytują ignorowanych ratingów.
- `MainWindow` przekazuje managerowi skonfigurowane kopie v2, nie rekonstrukcję
  z dawnych pól. Rodzaj skoku to trening/próbna/kwalifikacje/konkurs; stara liczbowa
  ważność zniknęła z UI tej ścieżki. Auto-gate wylicza liczbę przy otwarciu.
- `JumpSimulator::setSimulationSeed` pozwala podać seed zamiast trzech losowań
  globalnych. `SingleJumpsManager` przypisuje seed oraz czas przez indeks skoku,
  niezależnie od pracownika, i przekazuje rodzaj wydarzenia.
- **Uzupełnienie:** pojedyncze skoki mają już nowy panel pogody i przekazują wspólne
  ciągłe pole do managera. Dokładny stan i nowe testy opisano w rozdziale poniżej.
  Dawny generator pozostał dla legacy/API bez przekazanego pola.
- Nowe testy `--simulation-v2-setup-only`: wybór gotowych paczek, lokalna edycja,
  walidacja i przywracanie, przełączanie, synchronizacja K z wiatrem, ochrona
  źródłowych plików/globalnej bazy, rodzaj skoku, auto-gate, rendery małych i dużych
  ekranów, identyczność 2048 skoków ze stałymi warunkami na 1/4 wątkach i czas per indeks.

### Najbliższy konkretny krok

1. Backend/UX pogody w pojedynczych skokach już działa. Podłączyć go do konkursów:
   automatyczna propozycja z perków, proste kierunek/siła/loteryjność/spójność,
   opady, zaawansowane referencje. Przekazywać immutable `ContinuousWeather`
   i jawny pomiar do managera; wspólny seed/czas niezależny od wątków. Nie zostawiać
   kontrolek starego generatora, które nowa ścieżka ignoruje.
2. Podłączyć ten sam katalog do `CompetitionConfigWindow`/nowych sezonów. Zachować
   tożsamość obiektów i wskaźniki list/archiwum. Nowy format i wersja zapisu sezonu.
3. Uporządkować szczegóły skoku v2 (stare ratingi nie mają już znaczenia), dodać
   podgląd historii pogody na życzenie i nową manipulację. Pełny playground/CLI.
4. `DatabaseEditorWindow` nadal powinien przygotowywać katalog przed odczytem paczek,
   aby przybliżone dawne dane były od razu widoczne w studiach, nawet zanim użytkownik
   otworzy konfigurację skoków. Nie nadpisywać oryginałów ani blokować GUI migracją.

### Uzupełnienie wiatru podczas kontynuacji 28.10

Użytkownik podkreślił, że zmieniać ma się samo ogólne pole/tendencja: inny wiatr
przez kilka skoków oraz okresy niemal ciągłych gwałtownych zmian. Nie wszystkie
konkursy mają być loteryjne — charakter ma być ustawiany. Zapisano to też w specyfikacji.

Rdzeń ma teraz parametry tendencji utrzymywanych przez wiele skoków oraz
okresów aktywności: `trendAmplitude`, `trendPeriodSeconds`,
`trendTransitionSeconds`, `activityVariation`, `activityCorrelationSeconds`,
`fastCorrelationSeconds`. Szczegóły i początkowe propozycje są w opisie silnika.
Domyślne zerowe amplitudy pozostawiają konfiguracje stałe rzeczywiście stałymi.
Nowy panel i przekazanie pola są już podłączone do pojedynczych skoków;
konkursy/sezony nadal czekają na migrację katalogu i pogody.

Wszystkie trzy odpowiedzi otrzymano. Użytkownik wybrał scenariusze losujące
parametry w zakresach, prosty podgląd graficzny i jedno zdanie o loteryjności,
parametry dopiero po rozwinięciu, możliwość odwróceń kierunku oraz zachowanie
tego samego wariantu przy starcie. Podkreślił niezależność siły i loteryjności.
Pełny zapis jest w NEW_SIMULATION.md. Nie zadawać tych pytań ponownie.

Commit `d27eb28` z podłączeniem pojedynczych skoków jest już na remote.

Testy po dodaniu tendencji: `test-simulation-v2-core.sh` PASS (w tym trwałość,
odwrócenia, ciągłość, epizody szybkich zmian); pełny build Qt 5.15 i
`--simulation-v2-only` PASS. Parametry zerowe pozostawiają kalibrację
OVR90/70 bez zmiany: 15,3476 m.


## Scenariusze i UI pogody — praca 28.10.2026

Nowe moduły:

- `WeatherPresets.*`: pięć jawnych scenariuszy, losowanie parametrów w zakresach.
  Mocny wiatr nie otrzymuje automatycznie większej loteryjności. Cisza jest zerowa.
- `WeatherSetup.*`: QObject konfiguracji; automatyczna propozycja z perków skoczni,
  ręczne scenariusze, dostosowanie, dokładne punkty, opady, podgląd innej chwili.
  Ręczna edycja nie zapisuje paczki i nie dostaje ukrytych mnożników od perków.
  Widoki dostają bieżące dane; batch dostaje `shared_ptr<const WeatherField>`.
- `FieldWindMeasurement.*`: pomiar w czasie lotu z kolejnych stanowisk oraz
  snapshot przed skokiem dla dotychczasowych widoków. Zachowuje wybierane stare
  sposoby uśredniania i uwzględniania odległości. **To polityka kompatybilności,
  nie zweryfikowany dokładny pomiar FIS.**
- `WeatherWorkspace`, `WindSlopePreview`, `WeatherPresetChoice`, `WeatherTuning`,
  `WeatherAdvanced`, `WeatherNumber`: nowy panel Qt Quick, proste scenariusze,
  schemat zeskoku i ujawnianie parametrów na życzenie. Opady osobno, pięć wariantów.
- `WeatherSetupEditor`: mały fallback Widgets z tym samym backendem. Stare
  komponenty pozostały w projekcie. Nie ma dwóch działających modeli wiatru w UI.

`SingleJumpsConfigWindow` ma teraz `getWeatherSetup()`, a `MainWindow` przekazuje
jego pole do managera. `suggestedGate()` korzysta z tego samego pola; etykieta
sugestii reaguje na zmianę warunków przez krótki debounce, zamiast przeliczać
lot przy każdym odświeżeniu UI. Zmiana czasu podglądu nie zmienia czasu startu.

`SingleJumpsManager` dla nowych profili i podanego pola pomija dawny generator,
przypisuje czas i seed przez indeks, współdzieli immutable pogodę między wątkami.
Pomiar i snapshot pochodzą z tego pola. Fallback dla starych wywołań bez pola
pozostaje. Nowy panel jest na razie **tylko w pojedynczych skokach**, nie deklarować
przeniesienia go już do konkursów i sezonów.

Testy rozszerzono o zakresy scenariuszy, siłę niezależną od loteryjności, dokładną
ciszę i statyczne lokalne różnice, odwracanie kierunku, zachowanie podglądu i snapshotu,
walidację, opady niezależne od wyboru scenariusza, pomiar bez obcinania kątów,
klikanie UI i identyczność batcha w zmiennej pogodzie na 1 i 4 wątkach.
**Wynik: PASS** — `scripts/test-simulation-v2-core.sh`, produkcyjny build Qt 5.15.3,
`--simulation-v2-setup-only` (także rendery 1440/1024/520/360 px, oba motywy i większy
tekst), `--simulation-v2-only`, `--config-defaults-only`, `--single-jumps-only`.
Ten ostatni obejmuje też wcześniejsze audyty silnika i podstawowe regresje konkursu.
Nie testowano etapu na Windowsie ani Qt 6. Brak nowych pętli układów/ostrzeżeń QML.
Schemat używa SVG i zwykłych elementów Quick, bez nowej zależności od Qt Quick Shapes.

Drobne uzupełnienia wynikające z testów:

- Przy pustym wyborze skoczni jest empty state z powrotem do konfiguracji.
- Zmiana nazwy/kraju/HS aktualizuje nagłówek bez ponownego losowania.
- Przy zerowym wietrze można najpierw wybrać kierunek, a potem dodać siłę.
- Duże opady pokazują faktyczne minimum 8, zamiast przyjmować w UI np. 2 i używać 8.
- Pomiar ma dodatkowy, domyślny wariant składowej wzdłuż zeskoku. Pozostałe dwa
  dawne warianty zostały zachowane; dokładny algorytm FIS nadal wymaga weryfikacji.
- Po przerwanym równoległym batchu zegar/seed startują za największym ukończonym
  indeksem, a nie od liczby zwróconych wyników. To chroni przed powtórzeniem
  skoków/czasu, jeśli anulowanie zostawiło luki między ukończonymi zadaniami.

### Następny krok po tym etapie

`CompetitionConfigWindow` w trybie pojedynczego konkursu musi otrzymać własny
snapshot katalogu v2 (bez podmieniania aktywnych wektorów GlobalDatabase w trakcie
życia innych okien). Podłączyć gotowy `SimulationWeatherSetup` oraz `WeatherWorkspace`.
Następnie `CompetitionManagerWindow::setupSimulator` ma dostać pole i pomiar
oraz wspólny logiczny zegar; nie pokazywać nowego panelu dla ścieżki, która nadal
wywołuje stary generator. Dalej migracja nowych sezonów/format zapisów, manipulacje,
podgląd historii wiatru w wynikach i playground. Nie zaczynać UI wiatru od zera.

## Przygotowanie konfiguracji konkursu do nowych profili — 28.10

`CompetitionConfigWindow::suggestedGate` wybiera zawodnika według tego samego
nowego baseline OVR/formy/perków/K/rodzaju wydarzenia co preview silnika, gdy
skocznia ma profil v2. Dawne techniki nie zmieniają tego wyboru. Mieszany model
zwraca brak sugestii, a walidacja startu odrzuca go przed wejściem do managera.

Wykryto przy tym crash: domyślne pole belki konkursu dopuszczało wartości ponad
200, a wyszukiwanie binarne przekazywało je do ścisłej walidacji v2. Sugestia v2
ma teraz limit 200; start z wyższą belką jest odrzucany, a sterowanie belką w oknie
nowego konkursu ograniczone do 0–200. Stara ścieżka zachowuje swoje zakresy.

`--simulation-v2-competition-only`: PASS — odwrócone stare umiejętności nie
wpływają na wybór najmocniejszego nowego skoczka, preview nie rzuca dla mieszanych
wersji, a start jest blokowany czytelnym komunikatem. To jeszcze nie podłącza
kanonicznego katalogu do wyboru zwykłego konkursu.

Dalszy research: nowy sezon jest tworzony w
`NewSimulationSaveConfigurationWindow` (trzy wybory paczek) i kopiowany przez
`SimulationSavesWindow` (~104) do własnych obiektów `Jumper`/`Hill`. Ten punkt
można podłączyć do snapshotu katalogu bez podmieniania GlobalDatabase. Potem
sezonowy `CompetitionConfigWindow` dostaje już własne stabilne profile z zapisu.
`CompetitionManagerWindow` nadal ma pięć ścieżek wykonywania skoku (ręczna,
N, grupa, seria, cały konkurs) oraz osobne DNS. Przy podłączaniu nowego pola trzeba
ujednolicić zegar we wszystkich ścieżkach i zachować ciągłość kwalifikacje → konkurs.
