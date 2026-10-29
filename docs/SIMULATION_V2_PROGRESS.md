# Simulation v2 — postęp i punkty wznowienia

**Ostatnia aktualizacja: 29.10.2026. To główny plik przekazania pracy kolejnej
sesji AI, także na innym komputerze po `git pull`. Zadanie nie jest zakończone.**

## Start kolejnej sesji

1. Sprawdź `git status --short --branch`. Pracuj na istniejącej gałęzi
   `feature/simulation-v2`; pobieraj jej zmiany bez nadpisywania lokalnej pracy.
   Nie modyfikuj maina. Nie zaczynaj implementacji silnika ponownie.
2. Przeczytaj ten plik, pełne [NEW_SIMULATION.md](NEW_SIMULATION.md)
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
- **Zwykły konkurs ma już podłączony katalog v2 i wspólną pogodę.** Wybór trzech
  paczek, nowy lokalny edytor skoczni, uproszczone Warunki; kwalifikacje i konkurs
  zachowują seed pola oraz wspólny czas.
- **Nowe sezony są domyślnie v2:** kanoniczne trzy paczki, własne pełne profile,
  nowy edytor sezonowych zawodników/skoczni, ta sama konfiguracja pogody oraz
  zegar skoków w konkursach indywidualnych i drużynowych. Format `engine-version=2`.
  To nie kończy całej specyfikacji: najnowszą kolejkę czytaj na końcu tego pliku.

### Budowanie i odtwarzanie testów

Wszystkie polecenia uruchamiaj w głównym katalogu repo:

```bash
scripts/test-simulation-v2-core.sh
scripts/build-quick-ui.sh
scripts/test-quick-ui.sh --simulation-v2-only
scripts/test-quick-ui.sh --simulation-v2-setup-only
scripts/test-quick-ui.sh --simulation-v2-season-only
scripts/test-quick-ui.sh --hill-studio-only
scripts/test-quick-ui.sh --engine-audit-only
scripts/test-quick-ui.sh --config-defaults-only
scripts/test-quick-ui.sh --save-storage-only
```

Skrypt Quick Test przygotowuje izolowane dane testowe. Pełny domyślny smoke-test
jest duży; do etapu dobieraj konkretne przypadki. `SJ_BUILD_JOBS` steruje liczbą
zadań kompilacji. Lokalnie używano Qt 5.15.3 na Linuksie; staging Qt jest
automatycznie wykrywany przez skrypty. **Nie zweryfikowano tego etapu na Qt 6
ani Windowsie.** Instrukcja Windows: [WINDOWS_SETUP.MD](WINDOWS_SETUP.MD).

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
`feature/qml-competition-config`. Wymagania: [NEW_SIMULATION.md](NEW_SIMULATION.md).
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

## Dokładne wymuszenie wiatru przy podanym polu — poprawka

`GameSimulationBridge` pomijał dokładne wiatry z `JumpManipulator`, gdy caller
przekazywał `ContinuousWeather`: zmieniały się zapisane wartości, a lot nadal
korzystał z pola. Wymuszenie teraz podmienia lokalny wiatr i pomiar dla tego skoku,
zachowując opady bieżącego pola oraz pozostałe manipulacje. Działa tak samo
w preview i pełnym skoku. Wynik zapisuje `exactWindOverride=true` i flagę manipulacji;
nie zmienia wspólnej pogody następnych skoków. `--simulation-v2-only`: PASS,
w tym przeciwne wymuszenia ±3 m/s przy wstrzykniętym polu.

## Wspólny zegar pogody w managerze konkursu — 28.10

Gotowa, przetestowana warstwa wykonawcza: `CompetitionManagerWindow` udostępnia
`setSimulationWeather` i `bindWeatherSetup`. Wszystkie pięć ścieżek skakania
(ręcznie, N, grupa, seria, cały konkurs) używa wspólnego `simulateCurrentJump`.
Pole jest niezmienne między jawnymi zmianami ustawień; seed jakości i czas kolejnego
skoku są rozłączne. Skok i DNS przesuwają czas o 30 s, a „Odczekaj 30 s” zmienia
chwilę bez niezależnego losowania nowego pola. Opady i snapshot także pochodzą z pola.
Dokładne wymuszenie wiatru nadal działa lokalnie dla jednego skoku.

Powiązany `SimulationWeatherSetup` można otworzyć przez zwykłą nawigację Quick
(`continuousWeatherEditor`). Podgląd jest wtedy liczony od aktualnej chwili
konkursu (`previewOrigin`); oglądanie innego czasu nie przesuwa zegara konkursu.
Zmiana ustawień nie zeruje zegara i nie zmienia zapisanych skoków. Gdy caller poda
tylko pole bez edytora, niedziałające przyciski edycji są wyłączone.

**Nie oznacza to jeszcze włączenia nowego wiatru w konkursie z menu.** Nadal trzeba
podłączyć kanoniczny katalog i `SimulationWeatherSetup` w konfiguracji konkursu,
przekazać je do managera oraz zachować zegar kwalifikacje → konkurs. Potem nowe
sezony i ich narzędzia. Nie podmieniać globalnych wektorów pod żyjącymi oknami.

Walidacja Qt 5.15.3: `--simulation-v2-contest-weather-only` PASS (ręcznie, DNS,
czekanie, edycja, N, grupa drużynowa, seria, cały konkurs i KO; unikalne czasy
co 30 s), `--bulk-only` PASS (stare ścieżki), `--simulation-v2-setup-only` PASS,
`--simulation-v2-only` PASS. Bez nowych ostrzeżeń QML/pętli układów.

## Uwagi UI i dalszy kierunek — 28.10

- Skocznie i skoczkowie używają wspólnego `AthleteBadge` oraz
  `PerkStrengthSelector`: trzy stale widoczne podpisy **Lekko / Wyraźnie / Skrajnie**,
  mocno kontrastujący złoty wybór, wspólne obramowania i zachowanie. Zmiana poziomu
  nie odznacza, ponowny wybór tego samego odznacza, przeciwna strona zastępuje.
  Nagłówek naszywki ma osobny obszar kliknięcia, bez nakładania na wybór poziomu.
- Lista skoczni: flaga aktualizowana z krajem i zwarty zapis **HS140**.
- Najnowsza uwaga użytkownika określa **kierunek całego UI pogody**, nie tylko
  pojedynczą poprawkę: domyślne, widoczne losowanie według charakterystyki skoczni,
  kilka opcji łącznie, bez rozbudowanego UI szczegółowego. Rozdzielić deszcz i śnieg,
  krótko wyjaśnić realny wpływ po researchu. Cisza = słaby wiatr zwykle 0,1–0,3 m/s,
  nie matematyczne zero. Dodać 5–6 scenariuszy. Po tych uwagach kontynuować
  integrację konkursu, sezonów i pozostałej specyfikacji — nie uznać ich za porzucone.

### Wdrożenie uproszczonego kierunku pogody

`WeatherWorkspace` ma jeden dropdown z domyślnym „Losowo · według skoczni”,
wyraźne „Losuj ponownie”, grafikę i jedno zdanie. Ręczne dostosowanie kończy się
na czterech opcjach. Pełny `WeatherAdvanced` nie jest ładowany; pozostaje kodem
nieużywanym w UI, a API współczynników nadal służy testom/przyszłemu playgroundowi.
Opady mają trzy wybory: rodzaj, natężenie, przebieg. Nazwy scenariuszy są wspólne
dla Quick i fallbacku Widgets (`SimulationWeatherSetup::presets`).

11 scenariuszy: pięć dotychczasowych + spokojnie pod narty, mocno w plecy,
spokojny boczny, zmiennie pod narty, zmiennie w plecy i różny na zeskoku.
**Cisza nie jest już zerowa:** bazowo 85% próbek 0,1–0,3 m/s, pozostałe
0,03–0,5, niewielkie zmiany w czasie. Dokładne zero nadal można ustawić świadomie
przez siłę, zmienność i różnice równe zero; nie utożsamiać go z nazwanym presetem.

`PrecipitationKind` rozdziela śnieg/deszcz niezależnie od nawierzchni. Typ przechodzi
przez immutable pole, preview, silnik, dokładne wymuszenia wiatru i metadane skoku.
Śnieg zachowuje dotychczasowy współczynnik; deszcz ma osobny współczynnik i próg,
poniżej którego nie jest automatycznie karany. Research i ograniczenia zapisano
w `docs/WEATHER_RESEARCH.md`. Nie dodawać twierdzeń o udowodnionym spadku
aerodynamiki mokrego kombinezonu ani nie traktować tych współczynników jak pomiarów.

Wyniki: rdzeń PASS (5500 wariantów 11 scenariuszy, statystyka Ciszy, osobne efekty
deszczu i śniegu); `--simulation-v2-setup-only` PASS (zwykły ekran, wybór scenariusza,
brak technicznego formularza, 2048 skoków z deszczem 1 vs 4 wątki, oba motywy,
360–1440 px i większy tekst); `--simulation-v2-contest-weather-only` PASS.
`--simulation-v2-only` PASS, w tym zachowanie deszczu przy wymuszeniu wiatru.
Produkcyjny build Qt 5.15.3 PASS; bez nowych ostrzeżeń QML i pętli układów.
Pozostaje dopięcie katalogu i uruchomienia zwykłego konkursu, a potem sezony,
historia korytarzy i pozostałe prace specyfikacji. Nowy kierunek obowiązuje też je.

## Zwykły konkurs podłączony do v2 — najnowszy etap 28.10

Domyślny `CompetitionConfigWindow(SingleCompetition)` asynchronicznie wczytuje
kanoniczny katalog. Użytkownik wybiera paczkę zawodników/skoczni/zasad oraz
konkretną skocznię i zasady. Snapshot żyje przez całą konfigurację: wszystkie
wektory są odłączane przed opublikowaniem pointerów, zmiana paczki nie niszczy
poprzednich zawodników, ale usuwa ich z bieżącej listy/kandydatów QuickRoster.
Żadne globalne wektory nie są podmieniane.

Skocznia ma `SimulationProfileDraft`, legacy widget jest wyłącznie ukrytym lustrem.
`configuredHill()` jest źródłem faktycznych wejść preview i startu. Warunki używają
tego samego prostego `WeatherWorkspace` i `SimulationWeatherSetup`, losowanego
po wyborze skoczni. Sugestia belki bierze to pole, a jej podpis/delta odświeżają się
z krótkim debounce. Nie ma ignorowanych parametrów starej ważności skoku.

Kwalifikacje i konkurs dostają to samo pole i root seed jakości, a główna faza
zaczyna od `qualsWindow.getSimulationSeconds()`. Nie ma ukrytego losowania przy
starcie. Rekord skoczni po kwalifikacjach jest zachowany przed konkursem.
Zapis rekordów ponownie czyta kanoniczne pliki i scala tylko większe rekordy po ID,
zachowując ewentualne nowsze zmiany pozostałych pól. Dawne paczki są nietknięte.
Importowane listy są własnością konfiguracji (wcześniej wyciekały); stare profile
są przybliżane lokalnie z ostrzeżeniem, nowe dostają osobne tożsamości kopii.
Konfiguracja otwierana z MainWindow ma czas życia ograniczony do wykonania okna.

Wewnętrzny `DataSource::LegacyCompatibility` służy istniejącym regresjom oraz
zachowaniu dawnej ścieżki bez ekspozycji przełącznika w UI. Domyślny jest Canonical.
Nie pomylić tego z przywróceniem starego edytora użytkownikowi.

`--simulation-v2-competition-only`: PASS — w tym pełne kwalifikacje → dwie serie
→ zakończenie, deszcz w metadanych, wspólny czas bez powtórzeń, seed bez rerollu,
zmiana paczek bez starych kandydatów, stabilne pointery i scalanie rekordu bez
nadpisania zmienionego na dysku nazwiska. Test dopuszcza dodatkowe awanse ex aequo.
Produkcja Qt 5.15.3 buduje się; nie testowano Qt 6 ani Windowsa.

Przy sprawdzaniu istniejącej ścieżki wykryto jeszcze `ReferenceError: model is
not defined` w obu delegatach `RosterLibrary` i `RosterOrder`. Mają teraz jawny
`required property var model`, dzięki czemu kandydaci, kolejność i drag uchwyty
dostają właściwe role także na Qt 5. `--rosters-only` PASS (w tym prawdziwe
klikanie, przeciąganie, przewijanie, cofanie i skład/rezerwy), `--config-defaults-only`
PASS. Nie usuwać tej poprawki jako niepowiązanej — lista jest częścią nowego konkursu.
Końcowy `--bulk-only`: PASS — zachowane stare ścieżki indywidualne, drużynowe,
KO, N, seria i grupa; także wcześniejsze audyty punktacji i archiwum.

### Następne konkretne zadania

1. Nowy sezon: kanoniczne trzy wybory w `NewSimulationSaveConfigurationWindow`,
   własne kopie profili w `SimulationSave`, jednoznaczny format nowego silnika.
   Przed udostępnieniem sprawdzić edycję sezonowych zawodników i ręczny generator
   formy — nadal operują dawnymi umiejętnościami i mogłyby ignorować/zgubić v2.
2. Sezonowy `CompetitionConfigWindow` i `SimulationSaveManagerWindow`: to samo
   uproszczone losowanie, kontekst daty/kraju, przekazanie pola/rodzaju opadów do
   managera. Sezony nie są jeszcze przełączone domyślnie na nowy model.
3. Po wczytaniu zapisu odtworzyć katalog zdjęć z przenośnego `simulation-pack-id`,
   bez zapisu absolutnej ścieżki do pliku symulacji.
4. Podgląd korytarzy konkretnego skoku w konkursie i archiwum: użyć zapisanych
   `exposures`, nie bieżącej pogody/replay zależnego od nowszych współczynników.
5. Weryfikacja sposobu uśredniania FIS i reszta manipulacji/playgroundu/specyfikacji.
   Aktualna polityka pomiaru pozostaje zgodnością z ustawieniami gry, nie deklaracją FIS.

## Kafelki scenariuszy — ostatnia korekta UI

Użytkownik odrzucił dropdown scenariuszy. Nowy `WeatherPresetGrid` pokazuje wszystkie
11 wyborów jako zwarte kafelki w czterech grupach: Pod narty, W plecy, Boczny,
Pozostałe. W grupach krótkie nazwy; pełna nazwa jest w dostępności i podglądzie.
Wybrany kafelek ma ciemne tło, jasne obramowanie i znacznik; osobno znajduje się
„Według skoczni” oraz „Losuj ponownie”. Układ ma 4/2/1 kolumny zależnie od miejsca
i skali tekstu. Nie przywracać dropdowna w Quick jako uproszczenia.

`--simulation-v2-setup-only` PASS: rzeczywiste kliknięcie kafelka, trwałe zaznaczenie,
powrót do losowania według skoczni, wszystkie 11 kafelków, oba motywy i rendery
360–1440 px. Bez nowych ostrzeżeń QML/pętli układów. Stan integracji sezonów
pozostaje jak w kolejce powyżej; prace nad generatorem formy są osobną zmianą.

## Generator formy przygotowany do nowych sezonów

`JumperFormGenerator` czyta i zapisuje nową `AthleteProfile::form`, gdy zawodnik
ma v2. Nie zmienia OVR, ID, perków ani zdjęcia; dawne techniki nie są źródłem formy.
Nowa forma jest całkowita 1–100. Ustawienia i walidacja tabeli respektują minimum 1
dla nowych profili; dotychczasowe profile zachowują swój zakres i ścieżkę.
Usunięto dzielenie przez zero przy zmienności 0 i przy spadku formy do zera oraz
niezainicjalizowane pointery generatora. Nie dodaje to automatycznego rozwoju
formy w sezonie — ten pozostaje odłożony zgodnie ze specyfikacją.

`--audit-form-only` PASS: ręczna zmiana faktycznej nowej formy, bonus i granice,
2000 kolejnych generacji bez utraty parametrów/ID i bez NaN niestabilności,
oraz poprzednie audyty ustawień. Teraz można bezpiecznie podpinać to narzędzie
do nowych sezonów; pozostałe zadania sezonowe (katalog, edycja, format, pogoda)
nie zostały jeszcze zakończone.

## Dostosowanie kierunku i punktów — najnowsze uwagi użytkownika

- Kafelki scenariuszy są **domyślnie ukryte**, ujawnia je „Scenariusze”. Główna
  akcja „Losuj warunki” zawsze losuje według skoczni i perków. Wariant ręcznego
  scenariusza można wylosować osobno po rozwinięciu. Usunięto napis „Wylosowane
  dla skoczni”; tytuł domyślnego podglądu to „Wiatr na zeskoku”.
- `WindDirectionDial` daje natywny Dial 360°, klawiaturę co 1° i wpisywanie stopni.
  Działa na Qt 5.15 bez zależności Canvas2D/Shapes. Zastosowano go globalnie
  i w edycji lokalnej. 0° to pod narty, 180° w plecy.
- Sekcja dostosowania ma wyraźny nagłówek, kontrastową ramkę i osobny przycisk.
  „Dostosuj w wybranym miejscu” otwiera `WeatherLocalWind`: wskazanie miejsca
  na miniaturze zeskoku lub wpisanie metra, kierunek, siła, usunięcie punktu.
- Backend: `referenceAt`, `putReference`, `removeReference`. Edytują bazowe
  referencje **ciągłego** pola. Wstawienie na np. 113 m nie tworzy sztywnego sektora;
  interpolacja jest płynna. Wahania/podmuchy nadal zależą od ustawionej zmienności.
  Edycja nie losuje seeda i nie zmienia immutable snapshotów bieżących jobów.
  Parametry techniczne pozostają schowane.

`--simulation-v2-setup-only` obejmuje kliknięcie scenariuszy/ukrywanie, kierunek
37° → 38° klawiaturą, lokalne UI 113 m / 2,4 m/s, zgodność rzeczywistego pola,
ciągłość wokół nowego punktu, brak duplikacji, usunięcie i zachowanie seeda.
Wynik: PASS, produkcyjny build Qt 5.15.3 PASS; bez nowych ostrzeżeń QML.

**Następna uwaga do zbadania:** użytkownik podejrzewa silnie destrukcyjny wpływ
deszczu na skoki. Po tym UI wrócić do źródeł i kalibracji mocnego deszczu.
Nie uznawać obecnego łagodniejszego współczynnika za wynik badań. Kolejka
sezonów pozostaje aktywna i jest opisana powyżej.

## Format sezonu przygotowany — bez przełączania tworzenia sezonów

`SimulationSave` ma jawne `engine-version` 1/2. Nieznana wersja i zapis oznaczony
jako v2 z dawnym zawodnikiem/skocznią są odrzucane. Zapis v2 nie może utrwalić
mieszanego grafu. Dotychczasowe zapisy bez znacznika nadal są czytane jako 1.
Nowe tworzenie sezonu trzeba dopiero przełączyć na kanoniczne źródła i jawne 2.

Po odczycie profilu v2 katalog zdjęcia jest odtwarzany z `simulation-pack-id`
według kanonicznego katalogu aktualnego komputera. Absolutne ścieżki pozostają
wyłącznie kontekstem w pamięci. Dodano odczyt tego kontekstu dla przyszłego
edytora sezonowych zawodników. `--simulation-v2-only`: PASS (round trip formatu,
kompletne perki, przenośna ścieżka, nieznana wersja i mieszany profil).

**Priorytet po ostatniej uwadze:** najpierw kalibracja deszczu, potem poprawa UX
kierunku i prostych zmian na strzałkach **głównego** schematu wiatru.
Użytkownik odrzucił obecny duży formularz punktów i nieczytelną orientację tarczy.
Sprawdzić cofające się inputy. Dopiero po tych uwagach wrócić do kolejki sezonów.

## Deszcz — kalibracja wykonana przed kolejną poprawką UX

Research uzupełniony o wytyczne FIS odpływu wody oraz bezpośrednie relacje z
Oberstdorfu i Klingenthal. Materiały potwierdzają rozróżnienie niewielkiego deszczu
i trudnych ulew, ale **nie dają izolowanej krzywej deszcz → metry**. Nie odtwarzamy
fizyki mokrego kombinezonu z badań samej przepuszczalności.

Do `FlightParameters` dodano osobny koszt ulewy powyżej 5/10, rosnący kwadratowo
do dodatkowych 15% zapasu przy 10/10 (łącznie z poprzednią karą 17,4%). Wszystko
regulowane. Lekki/umiarkowany deszcz pozostaje bez nowej kary. Śnieg osobno.
To **balans gry**, nie współczynniki FIS. Szczegóły i źródła: `WEATHER_RESEARCH.md`.

`scripts/analyze-rain-v2.sh 10000` odtwarza 300 000 parowanych skoków; dane:
`docs/data/simulation-v2-rain-calibration.csv`. K120: średnia przy deszczu 0/2/5/8/10
to 120,849 / 120,849 / 119,759 / 112,146 / 99,822 m. Poprzednio przy 10 było
117,950 m. Zachowane seedy jakości, ta sama belka, brak wiatru. Testy rdzenia,
`--simulation-v2-setup-only`, `--simulation-v2-only`: PASS; kalibracja OVR pozostaje
15,3476 m przy suchych warunkach.

Teraz wykonać ostatnią uwagę użytkownika: czytelne kierunki względem skoczka/nart,
sprawdzić inputy i **usunąć duży formularz lokalnych punktów**, przenosząc prostą
edycję na sześć strzałek głównego `WindSlopePreview`. Dopiero potem wrócić do sezonów.

## Bezpośrednia edycja głównego schematu — wdrożenie

Usunięto `WeatherLocalWind` i jego duży formularz. Każda z sześciu strzałek
`WindSlopePreview` otwiera mały edytor **w tej samej karcie**: kierunek i siła
wyjściowa. Jeden obszar naraz; ponowny klik/× zamyka. Nie ma listy ani dokładania
punktów. `station`/`setStation` mapują tę obsługę na sześć referencji ciągłego pola;
pozostałe wartości w węzłach są zachowane, seed nie zmienia się. API dokładnych
dowolnych punktów pozostaje dla eksperymentów, bez ekspozycji w Quick.

`WindDirectionDial` ma teraz własną, zgodną mapę wskaźnika i myszy: narty/skoczek
pośrodku, pod narty u góry, w plecy u dołu, boczny z boków. Strzałka pokazuje skąd
wieje, a obok jest nazwany kierunek w odpowiednim kolorze i konkretne stopnie.
Poprzedni natywny Dial miał inną geometrię pozycji niż rysowana wskazówka —
nie przywracać go bez zgodności obu mapowań. Klawiatura i wskaźnik działają co 1°.

Inputy wiatru akceptują kropkę oraz przecinek. `NumberField.flexibleDecimal`
jest opcjonalne, domyślnie wyłączone dla pozostałych pól. Przy zmianie wybranej
strzałki zmienia się `recordKey`, aby draft nie przechodził do innego miejsca.
Zerowy wiatr pamięta wybrany kierunek przed dodaniem siły. Pozostają cztery główne
ustawienia, a scenariusze są schowane domyślnie.

Walidacja obejmuje rzeczywiste kliknięcia góry/dołu tarczy, klawiaturę,
wpisywanie 2.4 i 2,6, edycję siły na strzałce, stałe sześć punktów oraz zachowanie
sąsiednich węzłów i seeda. `--roster-inputs-only` i `--config-defaults-only` PASS.
Końcowy `--simulation-v2-setup-only`: PASS, także wpisanie wartości na strzałce
i przejście do następnej bez utraty poprzedniej. Produkcyjny build Qt 5.15.3 PASS,
brak nowych ostrzeżeń QML i pętli układów. Zmiany gotowe do ręcznego testowania.
Po ukończeniu tej poprawki wrócić do nowych sezonów według kolejki wyżej.


## Nowe sezony domyślnie v2 — ukończona integracja

`NewSimulationSaveConfigurationWindow` asynchronicznie wczytuje własny snapshot
`SimulationCatalog`, bez `syncActivePacks` i bez podmieniania globalnej bazy.
Trzy wybory, liczebności i zapamiętywanie zachowane; dawne zapamiętane ID mogą
wskazać odpowiadającą przybliżoną kopię. Ostrzeżenia migracji są jawne. Podczas
wczytywania nie można utworzyć niekompletnego zapisu. `createSave()` jest jedyną
fabryką używaną przez normalny przycisk tworzenia: własne profile, kontekst zdjęć,
reguły, rok/daty, stabilne ID, jawne `engine-version=2`, pusty kalendarz.
`LegacyCompatibility` służy wyłącznie wewnętrznym testom zgodności dawnego UI;
nie ma przełącznika dla użytkownika.

W sezonie v2 listy zawodników i skoczni otwierają nowe edytory profili (jeden
klik). Poprawne zmiany od razu trafiają do istniejącego obiektu zapisu, zachowując
jego adres, UUID, OVR/formę/perki i ścieżki zdjęć. Źródłowa paczka nietknięta.
„Cofnij edycję” przywraca stan z wyboru pozycji. Błędne wartości nie nadpisują
danych sezonu. Nowe pozycje z + też mają pełny profil v2, dostępny już podczas
`rowsInserted`; dotychczas widoki dostawały sygnał przed zmianą kontenera.
Po generatorze formy i zakończeniu zawodów draft jest przeładowywany, aby następna
edycja nie cofnęła nowej formy/rekordu. Ukryto dawne opcje pokazywania formy/
niestabilności w ustawieniach zapisu v2. Qt Widgets pozostają w repo i obsługują
wewnętrzną zgodność dawnych danych.

`CompetitionConfigWindow::setupWeather()` jest wspólne dla zwykłego konkursu
i sezonu v2. Losuje dla skoczni po ustawieniu daty wydarzenia; bez dat używa
nieznanego miesiąca, nie daty komputera. Ukrywa stare generatory i ważność skoków.
Kategoria wynika z wydarzenia kalendarza. W sezonie manager otrzymuje kontroler
przez `bindWeatherSetup`: manualne i automatyczne skoki używają tej samej pogody,
rodzaju opadów i zegara. Sugestia belki jest buforowana i odświeżana po zmianach
pogody/listy; wyświetlana jest konkretna liczba. Oddzielne wydarzenia kalendarza
mają oddzielne konfiguracje pogody — nie obiecujemy wspólnego pola całego weekendu.

Okna konfiguracji i runtime sezonu mają teraz ograniczoną własność RAII, zwalnianą
po zakończeniu wydarzenia, zamiast pozostawać dziećmi managera całego sezonu.
Naprawa referencji skoków po konkursie obejmuje wszystkie kalendarze i pomija
puste, zamiast dereferencji brakującego aktywnego kalendarza starego sezonu.

Usunięto pętlę bindingu podpisu przypisanej naszywki: pomiar i podpis korzystają
z zapamiętanej prezentowanej wartości. Draft nie emituje zmian dla identycznej
wartości. Opóźnione odtworzenie scrolla tabeli sprawdza, czy widok nadal istnieje.

**Walidacja Qt 5.15.3:** produkcyjny build i `--simulation-v2-season-only` PASS.
Test używa normalnej fabryki, renderuje nowe edytory i tworzenie sezonu w obu
motywach/małych rozmiarach, sprawdza walidację/reset/źródła/tożsamość/dodawanie,
otwiera normalną konfigurację i kończy konkurs indywidualny (trening) oraz
drużynowy. Każdy ma 8 skoków i 240 s zegara; zapisane Rain i kategoria skoku;
pełny zapis po rozegraniu ponownie się wczytuje. Osobny pusty poprzedni sezon
nie powoduje crasha. Brak błędów QML, pętli bindingów i recursive rearrange.
Ponadto PASS: `--onboarding-only`, `--simulation-v2-competition-only`,
`--simulation-v2-only`, `--simulation-v2-setup-only`, `--audit-form-only`.
Nie zweryfikowano tego etapu na Windows/Qt 6.

### Aktualna kolejka po sezonach — nie powtarzać gotowej integracji

1. Podgląd korytarzy **konkretnego zapisanego skoku** w konkursie i archiwum:
   użyć utrwalonych `exposures`, nigdy aktualnego pola/replay. Przed skokiem
   opcjonalna prognoza obecnego pola; średni wiatr pozostaje podstawową informacją.
2. Zweryfikować uśrednianie wiatru według źródeł FIS. Aktualny pomiar zachowuje
   ustawienia gry; nie deklarować pełnej zgodności z FIS bez dowodów.
3. Dalszy audyt przejścia do następnego sezonu, list/presetów i przypadków KO,
   z zachowaniem kompletnych profili v2. Automatyczny nowy model formy jest
   **odłożony** w specyfikacji; gotowy jest ręczny generator zgodny z v2.
4. Nowy UX manipulacji oraz pozostały zakres `NEW_SIMULATION.md`, w tym playground
   i strojenie pełnego silnika. Nie oznaczać całego zadania jako zakończonego.

## Opcjonalny podgląd wiatru przed skokiem i zapisanych korytarzy

`RaceAthlete` ma dyskretne „Wiatr ↗” w istniejącym wierszu statusu. Otwiera
sześć przybliżonych próbek **obecnego pola i czasu konkursu**, bez losowania,
zmiany pogody, uruchamiania lotu ani przewidywania końcowej rekompensaty.
Snapshot jest liczony w `refreshWindSnapshot`, a nie przy każdym bindingu Quick.
Zmiana czasu zamyka ten podgląd; nie pokazuje się w trakcie animacji live ani
po zakończeniu serii/konkursu.

W karcie wybranego skoku klikalne „Wiatr ↗” zachowuje średni wiatr jako główną
wartość. Na żądanie otwiera schemat w fazach **Przed / W locie / Po**.
`QuickWindProfile::fromMetadata` czyta wyłącznie utrwalone `exposures`: pole przed,
średnią po faktycznie przebytych odcinkach oraz pole w chwili zakończenia lotu.
Odcinki nieprzebyte mają „—”, nie udają bezwietrznego lotu. Przy więcej niż sześciu
odcinkach grupowanie ma wagi długości (w locie długości rzeczywiście przebytej).
To schemat, nie precyzyjna trajektoria. Brak/nieprawidłowe metadane, DNS/DSQ
oraz niedokończony live nie udostępniają fałszywego podglądu. Wspólny komponent
obsługuje także kartę w archiwum i zmianę wybranej serii.

Podgląd jest tylko do odczytu, bez kontrolek edycji, i nie zmienia podstawowego
układu karty. Popup mieści się w małym oknie i ma wewnętrzny scroll dla dużego
tekstu. Edytowalny główny schemat warunków pozostaje dotychczasowym narzędziem.

Build Qt5 i `--simulation-v2-contest-weather-only` PASS: rzeczywiste kliknięcia,
trzy fazy, najmniejsze okno, bieżący czas prognozy bez jego przesunięcia, archiwum
oraz manualne/automatyczne ścieżki konkursu. `--simulation-v2-only` PASS: zapis/
odczyt profilu, ważone grupowanie, odcinki nieprzebyte i uszkodzone dane.
`--simulation-v2-setup-only` i `--simulation-v2-season-only` również PASS.
Brak błędów QML i pętli układów.

**Najnowsze polecenie użytkownika — kolejny etap:** dodać 3–7 metod uśredniania
z różnym realizmem, w tym oficjalne warianty FIS. Pomiar ma być **nieidealny**,
nie odwracać idealnie fizycznego wpływu lotu. Zapisane w `NEW_SIMULATION.md`.
Badanie już rozpoczęto: bieżące ICR 415.3 odsyła do osobnych zasad; katalog
Swiss Timing 2025/26 wymienia 8 urządzeń / 12 na lotach (to wyposażenie, nie dowód
liczby aktywnych czujników w algorytmie). Praca Pietschnig 2020 opisuje historyczne
5/7/10, wagi i zmianę w 2016; nie podaje kompletu wag i okien. Nie podpisywać
arbitralnego układu jako „oficjalny”. Dalsze szczegóły w `WIND_MEASUREMENT_RESEARCH.md`.
Dopiero po metodach wrócić do manipulacji, audytu następnego sezonu i playgroundu.

## Wybór sześciu metod pomiaru — wdrożony

Użytkownik zaakceptował **„FIS — przybliżenie”** po wyjaśnieniu braku pełnej
technicznej receptury. Nie blokować dalszej pracy poszukiwaniem dokładnych wag;
nie przedstawiać naszych liczb jako oficjalnych. `NEW_SIMULATION.md`,
`WIND_MEASUREMENT_RESEARCH.md` i opis silnika zawierają tę decyzję.

Sześć metod: FIS — przybliżenie, Pomiar przez czas lotu, Gęsta sieć czujników,
Odczyt przed skokiem, Wiatr na trasie lotu, Dotychczasowy Sj.Sim. Jedna kontrolka
w zasadach i opcjach pojedynczych skoków, pomoc w tooltipie. Dawne przełączniki
kierunków i odległości ujawniają się tylko dla zgodności. Nowy ekran pojedynczych
skoków/reset zasad wybiera przybliżenie FIS, a istniejący preset zachowuje swoją
metodę. Brak pola w dawnym pliku = zgodność. ID są walidowane w paczkach i
zapisach (także zagnieżdżonych kopiach zasad kalendarza).

Metoda jest niezależna od lotu: ten sam seed daje tę samą odległość/lądowanie/
noty, a inną średnią do punktacji. Nawet średnia po trasie nie „odwraca” modelu
lotu. Nowe modele są deterministyczne, z walidowanymi współczynnikami i limitami
kosztu. Wymuszone dokładne wiatry używają wybranego pomiaru także na polu zastępczym.
Metadane zapisują ID/wersję; niestandardowa wstrzyknięta strategia ma ID -1.

Pasek aktualnego wiatru i prognozy „do prowadzenia/awansu” korzystają z buforowanej
średniej obecnego pola dla wybranej metody, bez podglądania przyszłego pola.
Nie stosują dawnych progów kierunku ani wpływu odległości do nowych metod.
W dawnej matematyce usunięto artefakt: czysty boczny wiatr przez drobny błąd
cos(90°) i ceil dawał +0,01 m/s zamiast 0. Zgodność poza tym zachowana.

`--simulation-v2-measurement-only`: PASS (6 metod, pole stałe/boczne/zmienne,
ważenie, podmuch między czujnikami, brak zmiany lotu przy zmianie pomiaru,
różne punkty, zapis, nieznane ID, konfiguracja, dokładne wymuszenie i DNS).
`--simulation-v2-contest-weather-only` ćwiczy nowe metody w każdej ścieżce,
KO/drużynówce, plus poprawkę zmiany języka na otwartych zawodach: PASS.
Ponadto PASS: konfiguracja v2, zwykły konkurs, sezon, adapter/zapis, audyt silnika
oraz `--translation-only`. Qt5.15.3; Windows/Qt6 nadal bez weryfikacji tego etapu.

### Najnowszy punkt wznowienia

1. Metody pomiaru i podgląd korytarzy są gotowe; **nie implementować ich od nowa**.
2. Kontynuować nowy UX manipulacji z `NEW_SIMULATION.md`, następnie pełną
   integrację strojenia/playgroundu i pozostający audyt przejść sezonów/presetów.
   Automatyczny nowy model formy pozostaje odłożony przez użytkownika.
3. Użytkownik zgłosił tłumaczenie „Zakończ konkurs”. Naprawiono odświeżanie
   runtime PL/EN/CS i pętlę stopki; dodano 40 wpisów na język dla nowych kontrolek.
   **Pełny audyt tłumaczeń może poczekać**, zgodnie z jego zgodą. Kolejka i testy:
   `TRANSLATION_AUDIT_2026-10-28.md`. Nie deklarować pełnej kompletności języków.
4. Całe zadanie `NEW_SIMULATION.md` nadal trwa. Main nietknięty; pracować solo,
   commitować i wypychać kolejne ukończone etapy na `feature/simulation-v2`.

## Manipulacja pojedynczego skoku — nowy edytor

`SimulationJumpDraft` i `JumpManipulationWorkspace` zastępują dawny formularz
przy konkursie z profilem v2. Trzy sekcje: **Lot / Lądowanie i noty / Warunki**.
Każda korekta jest opcjonalna. Jakość, odległość, opady i pomiar mają bonus oraz
opcjonalny zakres; równe granice wymuszają wartość. Lądowanie, pojedyncze noty
sędziów i ryzyko DSQ są niezależne. Noty normalizują się do pół punktu, ryzyko
jest czytelnym procentem; DSQ wyłączone w zasadach nie da się wymusić.
Nie ma już fikcyjnych suwaków dawnej techniki wybicia/lotu dla nowego modelu.

Wiatr ustawia się przez te same sześć strzałek i edytor kierunku względem nart.
Zatwierdzenie tworzy niemutowalną, ciągłą lokalną kopię pola wiatru. Nie zmienia
wspólnego pola konkursu, typu deszcz/śnieg ani jego zegara. Osobno można zmienić
sam pomiar do punktacji: taki zabieg nie zmienia fizycznego lotu. Bieżący podgląd
wiatru respektuje przyjętą lokalną korektę i wraca do pola konkursu po skoku.

Zmiany zaczynają działać dopiero po „Zastosuj”, tylko na najbliższy skok.
Ponowne otwarcie zachowuje przyjęty draft; Wstecz nie nadpisuje go. „Wyczyść”
usuwa wszystkie korekty, a zatwierdzenie pustego draftu zdejmuje wskaźnik.
Niepoprawne minimum/maksimum pozostaje do poprawienia bez cofania wpisywanych
wartości, blokuje zatwierdzenie i ma komunikat przy stałym przycisku na dole.

Pozostawiono Widgets, z osobnym natywnym widokiem tego samego draftu v2.
Stary formularz pozostaje dla starego silnika. Zabezpieczono też pusty wskaźnik
ustawień wiatru w dawnym formularzu. Katalogi EN/CS obejmują nowe kontrolki.

Usunięto źródło `Detected recursive rearrange` w nowej siatce sędziów: liczba
kolumn wynika z szerokości viewportu, nie z implicitWidth tej samej siatki.
Testy mają bogatszą identyfikację ekranu/rozmiaru przy podobnych ostrzeżeniach.

### Następna praca po tym etapie

1. **Playground pełnego silnika v2**. Dotychczasowy `source/playground` nadal
   opisuje stare techniki i sektory — nie podpinać go do v2 pozornie i nie
   deklarować ukończenia. Nowe profile/parametry EngineParameters, wspólne seedy
   porównań, ciągła pogoda, krzywe lądowania/not, statystyki indywidualne i makro,
   praca w tle/anulowanie/debounce/multithreading, CLI i eksport. Wymagania
   szczegółowe są w `NEW_SIMULATION.md`; dawny playground użytkownik odrzucił.
2. Audyt przejścia do następnego sezonu i presetów/list przy zachowaniu UUID,
   profili v2, powiązań i zdjęć; regresje KO/drużyn oraz danych zapisanych.
3. Pełny audyt tłumaczeń, końcowa regresja i spójny raport względem maina.
   Automatyczny nowy rozwój formy nadal odłożony. Całe zadanie nadal trwa.

### Nowe uwagi użytkownika — zrobić zaraz po manipulacji, przed playgroundem

- Kierunek wiatru: wyraźniejszy skoczek i lepsza ikona, usunąć dwie pomocnicze
  strzałki; naprawić przerwanie obrotu po wyjściu kursorem poza okrąg.
- Rozdzielić „zmienność w czasie” i „różnice na zeskoku”: edycja jednej wartości
  nie powinna przestawiać drugiej. Sprawdzić zarówno model, jak i drafty inputów.
- Ten sam kierunek/siła/zmienność/różnice ma dawać różne realizacje po ponownym
  losowaniu. Zachować ustawienia, zmieniać seed, nie mylić z wyborem scenariusza.
- Opady: jeden wybór **Bez opadów / Śnieg / Deszcz**, czytelne ikony. Nie pokazywać
  rodzaju opadów dla wariantu bez opadów.
- Ikony scenariuszy wiatru nie mogą przypominać jakości połączenia sieciowego.
- Jednoczesne rozwinięcie edycji lokalnego wiatru i dostosowania całości nie może
  się zlewać: kontrast, hierarchia, wyraźne zakresy oddziaływania, balans.
- Wiadomość kończy się pustym „8.”; żadnego dodatkowego wymagania nie dopowiadać.

Weryfikacja manipulacji: `--simulation-v2-manipulation-only` PASS, w tym
prawdziwe wejście przez opcje konkursu, ponowne otwarcie/anulowanie, pierwszy
skok z korektą i drugi bez niej, zachowany deszcz i zegar. Natywna prezentacja
tego samego draftu również sprawdzona. Render obu motywów 1440/1024/520/360,
powiększony tekst, wpisywanie przecinka/kropki i not częściowych. Brak błędów
QML, pętli bindingów i recursive rearrange. PASS również: rdzeń v2, pomiary,
adapter/zapis, konfiguracja, konkurs kanoniczny, pogoda konkursowa, sezony,
stare ścieżki bulk i zmiana języków. Windows/Qt6 tego etapu bez weryfikacji.

## Najnowszy stan — poprawki wiatru po manipulacji ukończone

Punktem wejścia na innym komputerze jest teraz
[`RESUME-28-10-2026-simulation.MD`](RESUME-28-10-2026-simulation.MD).
Opisuje gotowe etapy, konkretne pliki, testy i pozostałą kolejkę.

Wszystkie nowe uwagi wiatrowe z listy powyżej zostały obsłużone:

- Cztery ręczne parametry nie są ponownie estymowane po każdej edycji. Kierunek,
  siła, zmienność i różnice przestrzenne zachowują wpisane wartości. Lokalny
  obrót referencji zachowuje siłę; sprawdzono wiele kątów 0–359°.
- `basePreview` podczas konfiguracji pokazuje ustawione referencje bez podmuchów.
  Zwykły podgląd nadal pokazuje rzeczywiste pole. Nie mylić obu sił wektorowych.
- Jawne „Losuj nowy układ” zachowuje cztery wartości własnego wariantu i losuje
  nowe referencje/seed. Tooltip uprzedza o zastąpieniu ręcznych zmian miejsc.
- Duża ikona skoczka/nart, bez dwóch bocznych pomocniczych strzałek; utrzymany
  grab myszy także poza dialem, bez przejęcia gestu przez ScrollView.
- Jeden wybór Bez opadów / Śnieg / Deszcz z ikonami; bez opadów ukrywa dalsze
  pola/opis. Natywna prezentacja również ma ten flow.
- Scenariusze mają rysowane ikony wiatru zamiast słupków „zasięgu”.
- Tuning całej skoczni i edycja lokalna są zamienne; zamykają siebie nawzajem,
  mają precyzyjne nagłówki i odmienny kontrast. Oszczędza to również wysokość.
- Uzupełnione tłumaczenia całego widocznego modułu konfiguracji wiatru EN/CS.

PASS: produkcyjny build Qt5 i `--simulation-v2-setup-only` (w tym realne drag
poza dial i UI opadów oraz identyczne 2048 skoków przy 1/4 wątkach), manipulacja,
pogoda konkursowa, zwykły konkurs kanoniczny, sezony, metody pomiaru, runtime
języków i kontrola TS. Brak błędów QML/bindingów/recursive rearrange.

**Następnie: nowy playground pełnego silnika v2**, potem audyt następnego sezonu,
presetów/list i szersze tłumaczenia. Poprzednie kolejki „po manipulacji poprawić
wiatr” są już wykonane. Nowy playground nie był jeszcze implementowany.

## 28.10.2026 — nowy playground v2 i audyt następnego sezonu (weryfikacja w toku)

Ustalenia użytkownika: wszystkie trzy perspektywy analizy, na początek konkursy
indywidualne bez KO, pełny dostęp do współczynników silnika i wiatru, osobna pełna
analiza pola, tryb kontrolowany i przebieg konkursu, odizolowane eksperymenty,
automatyczny mały podgląd oraz edytowalne cele kalibracji. Szczegóły i architektura:
`PLAYGROUND_V2.md`. Automatyczny rozwój formy nadal odłożony.

Zaimplementowano nowy schemat 2, bezpośrednie wywołanie Engine v2, registry
parametrów, ciągłą pogodę, asynchroniczne kanoniczne źródła, widoki lądowania,
wiatru, kalibracji i całej stawki oraz ograniczony koszt podglądu z debounce.
Zachowano statystykę konkursową, porównania, historię, replay i eksporty.
CLI `--analysis` zapisuje krzywe, przekroje, siatkę i porównania lotu bez/z wiatrem.

Audyt znalazł lukę w NewSeasonConfiguratorWindow: nadal pokazywał dawne edytory
profili i nie oznaczał list do dodawania danych jako v2. Dodano wspólne drafty
profili, zachowanie zdjęć/UUID/perków oraz właściwy tryb wstawiania nowych pozycji.
Trwa weryfikacja regresją draftów i normalnego sezonu.

Zweryfikowano dotąd na Windows/Qt 6.8.3:
- pełny build stanu pobranego z remote;
- build pierwszej implementacji playgroundu;
- CLI: sparowany batch i eksport raportu;
- CLI: analiza z 81 punktami krzywych, siatką 11809 odczytów i 512 porównaniami lotu;
- synchronizacja źródeł lupdate oraz EN/CS, brak unfinished i rozbieżności argumentów.

Pełna przebudowa smoke po zmianie gałęzi była droga. Po końcowych zmianach nagłówków
trzeba wykonać przyrostowy build przed uznaniem testów runtime za miarodajne.
**Nie jest to jeszcze końcowy raport PASS nowych testów/UI.** Wyniki zostaną
uzupełnione po weryfikacji. Main bez zmian, praca bez subagentów.

## Zakończenie etapu — 28.10.2026, Windows/Qt 6.8.3

Playground v2 i opisany audyt zostały ukończone oraz zweryfikowane. Poprzednia
adnotacja „weryfikacja w toku” jest historycznym zapisem przebiegu tego etapu.
199 ustawień obejmuje wszystkie publiczne współczynniki modeli jakości, lotu,
lądowania, pola wiatru i pomiaru oraz wejścia profili i ustawienia analizy.
Tooltipy opisują skutki, jednostki i wartości domyślne. Kopia wartości domyślnych
w obliczeniach jest buforowana; metadane tekstowe nie powstają dla każdego konkursu.

PASS:
- `--playground-only`: zgodność z bezpośrednim silnikiem v2, wspólne seedy,
  histogramy/próbki 1 i 4 wątki, oba zegary, DSQ zgodne z zasadami, anulowanie,
  eksport, rzeczywiste run/replay, historia i wszystkie nowe widoki w obu motywach,
  małe okna i powiększony tekst; podgląd wznowiony po zakończeniu batcha;
- `--simulation-v2-only`: adapter, repozytorium, katalog, migracja, idempotencja,
  zdjęcia, ochrona uszkodzonych danych i zachowanie gotowych paczek;
- `--simulation-v2-competition-only`, `--simulation-v2-manipulation-only`,
  `--simulation-v2-contest-weather-only`, `--simulation-v2-setup-only`,
  `--simulation-v2-season-only`, `--simulation-v2-measurement-only`;
- `--bulk-only`, `--workflow-only`, `--rosters-only`, `--dates-only`,
  `--packs-only`, `--save-storage-only`, `--appearance-only`,
  `--athlete-studio-only`, `--hill-studio-only`, `--translation-only`;
- cztery niezależne zestawy rdzenia na MSVC: random, quality, weather, engine;
- końcowy build produkcyjny oraz katalogi EN/CS: 0 unfinished, 0 błędów argumentów.

CLI dodatkowo zweryfikowano na niejednolitym, zmiennym polu z podmuchami:
identyczna analiza powtórzona dwa razy, stabilność przy 1/4 wątkach, niezmieniony
fizyczny lot przy zmianie metody pomiaru i działający cel kalibracji OVR90–70.
512 konkursów kontrolnych dało średnią przewagę **15,357421875 m** (cel 14–17 m).
Niezależny test rdzenia pod MSVC uzyskał 15,3476 m w większej próbie.

Audyt ujawnił dodatkowy błąd Windows w publikacji migrowanej skoczni:
statyczny QFile::rename nie obsługiwał prawidłowo uchwytu QTemporaryFile.
Własna metoda rename oraz setAutoRemove(false) po sukcesie zachowują nowy plik.
Regresja obejmuje odczyt opublikowanego pliku i ponowną migrację po edycji paczki.

Test dawnego zarządzania paczkami odwoływał się do nieobecnego już menu w nowym
studio. Testuje teraz oryginalne akcje zachowanego kontrolera Widgets i czeka
na wykonanie akcji kolejkowanej przez QuickBridge. Nie przywrócono przełącznika
starego/nowego edytora. Nowy katalog i oba studia mają osobne regresje.

Następnie: strojenie/kalibracja według oceny użytkownika, bez automatycznego
nadpisywania normalnej gry. Automatyczny rozwój formy nadal odłożony.
# 29.10.2026 — przygotowanie do pełnej kalibracji

- Uporządkowano główny katalog: dokumenty przeniesione do docs, poprawione linki;
  niczego nie skasowano. README pozostaje w root.
- Przejrzano źródła laboratorium Kariera Skoczka z archiwum użytkownika.
  Projekt osobnego laboratorium i odpowiedzi na osiem pytań zapisano w
  [BALANCE_LAB_DESIGN.md](BALANCE_LAB_DESIGN.md). Dotychczasowy playground zostaje.
- Użytkownik potwierdził rozszerzalne obszary, dane i reguły po lewej, wykresy obok,
  badanie jednego skoczka i całej stawki, trzy serie, pełną analizę pogody/lotu/
  rekompensat z odtwarzaniem oraz profile z osobnym zastosowaniem w grze.
- Docelowo gracz ma otrzymać tylko 1–5 prostych ustawień. Publiczny import silnika
  i system modowania są poza obecnym zakresem.
- Wdrożono płynny udział formy zależny od K (domyślnie 28,8/30/33% dla
  K90/120/220) oraz losową reakcję na wiatr: rozrzut zależny od siły,
  nasycenie pomocy pod narty i osobne szanse/siły strat dla trzech kierunków.
- Ograniczona reakcja jest losowana raz na skok, we własnym strumieniu;
  manipulacja dystansu używa tej samej reakcji. Przejście złego zdarzenia jest
  ciągłe także dla pojedynczego skoku. Rzeczywiste pole i pomiar pozostają jawne.
- 16 nowych współczynników jest w wspólnym rejestrze UI/CLI (łącznie 215).
  Eksporty szczegółów zawierają wylosowaną reakcję. EN/CS uzupełnione.
- Damped Cauchy referencji x/(1+d*x²) nie został skopiowany do wiatru: największe
  losowania wracają do zera. Użyto tanh(normalnego) plus jawne ryzyko strat.
- Weryfikacja rdzenia Windows/MSVC: wszystkie cztery zestawy PASS, w tym nowe
  próby populacyjne i zbieżność kroku. Kontrola tłumaczeń: zero unfinished i
  błędów placeholderów. Wyniki budowy/integracji dopisane po zakończeniu poniżej.
- Końcowa kompilacja Windows/Qt 6.8.3 aplikacji i testów PASS; aktualny exe
  pozostaje w build-win/release. Natywne --playground-only,
  --simulation-v2-only i --translation-only PASS na końcowym kodzie.
- CLI PASS: powtarzalna analiza, porównania przy 1/4 wątkach, pomiar niezależny
  od fizycznego lotu, cel kalibracji OVR90–70 (15,3574 m) oraz 215 ustawień.
  Ziarno reakcji wiatrowej jest wydzielane przed losowaniem jakości: zmiana jej
  rozkładu lub wyzerowanie odchylenia nie zmienia reakcji na wiatr (nowa regresja).
