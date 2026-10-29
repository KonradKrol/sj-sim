# Dotychczasowy playground symulacji v2

Aktualnie obok niego jest osobne [laboratorium balansu](BALANCE_LAB.md) z pytaniami,
trzema seriami, grupami, mapami, celami i profilami. Wspólny rejestr ma teraz
607 ustawień, w tym generator pogody. Runner obsługuje również KO i drużyny.
Poniższe ustalenia z 28.10 opisują pierwszy etap; stare wejście pozostaje dostępne.

Aktualizacja 29.10.2026: silnik ma zależność udziału formy od K i losową,
spójną podczas lotu reakcję na wiatr. Wszystkie 16 nowych współczynników
jest dostępnych w obecnym playgroundzie i CLI (łącznie 215 ustawień).
Zapisane seedy z poprzedniej kompilacji nie gwarantują tych samych wyników.
Podgląd dystansu bez RNG pokazuje reakcję neutralną, nie średnią uwzględniającą
ryzyko złych zdarzeń. Próby statystyczne uwzględniają pełny model.
Projekt osobnego laboratorium: [BALANCE_LAB_DESIGN.md](BALANCE_LAB_DESIGN.md).
Obecny playground pozostaje dostępny.

## Ustalenia z użytkownikiem — 28.10.2026

- Trzy perspektywy: porównanie wariantów, konkretny skoczek i cała stawka.
- Na początek zwykłe konkursy indywidualne; KO i drużyny nie są wymagane
  w pierwszej wersji playgroundu. Nie mylić tego z obsługą tych zawodów w grze.
- Eksperymenty i zapisane konfiguracje są odizolowane od zwykłej gry.
- Pełny dostęp do współczynników silnika i ciągłego pola wiatru, w tym tablic
  perków, krzywej K oraz parametrów pomiaru. Jedna definicja parametrów dla CLI/UI.
- Dwa jawne tryby: kontrolowane warunki (każdy start o czasie 0) i przebieg
  konkursu (wspólny zegar, domyślnie 45 s między skokami). W obu warianty
  korzystają ze wspólnych seedów. Podczas samego lotu wiatr nadal jest ciągły.
- Automatyczna mała próba w tle, debounce i odrzucanie przestarzałych wyników.
  Podgląd liczy bazę i wybrany wariant, dobierając 2–16 konkursów do rozmiaru
  stawki/serii. Pozostałe warianty są liczone w pełnej próbie uruchamianej ręcznie.
- Dokładne krzywe prawdopodobieństw lądowania przy stałym bocznym wietrze,
  estymowane noty i częstości oraz osobna próba pełnego lotu przy zmiennej pogodzie.
- Osobna analiza pola: czas, przestrzeń, mapa, opady, kierunek i obie składowe;
  odczuty wiatr kontra pomiar punktowy oraz metry względem skoku bez wiatru.
- Edytowalne cele kalibracji, sprawdzane z przedziałami ufności pełnych prób.

## Architektura

`PlaygroundModel` definiuje parametry i mapuje je bezpośrednio na
`EngineParameters`, `WindParameters`, `PrecipitationParameters` oraz
`FieldWindMeasurementParameters`. Domyślne wartości pochodzą ze struktur silnika.
Konfiguracja zapisuje kopię domyślnych wartości w `defaults`, a jawne zmiany
w `parameters` i `variants[].changes`. Dane źródłowych profili są kopiowane.

`PlaygroundRunner` wywołuje bezpośrednio `SimulationV2::Engine::simulate`;
menedżer gry służy do kolejności, awansu i klasyfikacji. Nie używa dawnych technik,
liczbowej ważności skoku, losowanych sektorów ani globalnych współczynników gry.
Każdy konkurs ma osobne ziarno pogody, a każdy skoczek/seria osobne ziarno skoku.
Zmiana pomiaru nie zmienia źródła fizycznego wiatru.

Paczki są pobierane asynchronicznie z `SimulationCatalog`, wspólnego z nowymi
konkursami i sezonami. Nie jest to lista samych dawnych paczek GlobalDatabase.
Uwagi o migracji i problemach wczytywania są dostępne przy wyborze danych.
Źródłowe pliki i ustawienia aktywnych paczek pozostają poza eksperymentem.

Historia v2 jest osobna od dawnego playgroundu. Schemat 1 jest jawnie odrzucany.
Raport zawiera skrót kompilacji, efektywne parametry każdego wariantu i wejście.
Dokładne odtworzenie liczb wymaga tej samej kompilacji; starszy raport zachowuje
swoje zapisane wyniki, a ponowne uruchomienie używa aktualnego silnika.

`PlaygroundAnalysis` wylicza dokładne krzywe lądowania, estymuje średnie noty
z zaokrąglaniem/ograniczaniem modelu oraz pobiera deterministyczne przekroje pola.
Rozdzielczość i horyzont są edytowalne, siatka ma limit 200 tys. próbek.
Mapa PNG jest jedną teksturą, nie tysiącami obiektów QML. Mapa ma jawną skalę.
Próba lotu porównuje te same wejścia i seedy z pełnym wiatrem oraz z zerowym
wiatrem przy zachowanych opadach. To nie jest obietnica idealnej rekompensaty.
W tej próbie analizujemy oddane skoki, bez losowania DSQ. Pełne konkursy
respektują zarówno ustawione ryzyko DSQ, jak i przełącznik zasad.

## Statystyka i ograniczenia

Histogramy i agregaty używają wszystkich ukończonych porównań. Próbki szczegółów
to pierwsze konkursy do ustawionego limitu; nie reprezentują losowego podzbioru.
Przedziały średniej odległości/punktów uwzględniają grupowanie skoków w konkursy.
Różnice sparowane to wariant minus baza, a dodatnia zmiana miejsca oznacza gorzej.
Miejsca nieklasyfikowane są poza średnią, z osobnym licznikiem.

Cel kalibracji jest spełniony dopiero, gdy cały przedział 95% mieści się
w oczekiwanym zakresie. Rozłączność oznacza niespełnienie, pozostałe przypadki
są nierozstrzygnięte. To kontrola zachowania modelu, nie potwierdzenie realizmu.
Krzywe częstości z małych prób i rzadkich przedziałów odległości są szumne.
Mapa pola jest próbkowana; krótsze podmuchy wymagają odpowiednio gęstej siatki.

Duże próby mają osobną pulę wątków, limit liczby pracowników zależny od oszacowania
pamięci, anulowanie między skokami i zachowywanie wyłącznie pełnych porównań.
Liczba próbek szczegółowych jest ograniczona niezależnie od rozmiaru batcha.

## CLI

```text
sj-sim --playground-example experiment.json
sj-sim --playground --config experiment.json --output results/run-01 --threads 8
sj-sim --playground --config experiment.json --replay 1:42 --output replay.json
sj-sim --playground --config experiment.json --analysis 1 --output analysis.json
```

Na Windows: `sj-sim.exe`. Indeksy od zera. Istniejące wyniki nie są nadpisywane.
`analysis.json` zawiera wejście, krzywe, pełną siatkę, przekroje i próbę lotów,
w tym odczuty/pomierzony wiatr, rekompensatę i efekt na odległość.
Raport główny eksportuje też CSV dla skoczków, próbek i porównań.

## Weryfikacja

Nowe regresje sprawdzają zgodność numeryczną z bezpośrednim wywołaniem silnika,
sparowane seedy, identyczność próbek/histogramów przy 1 i 4 wątkach, oba zegary,
walidację, anulowanie, eksport oraz działające widoki UI. Wyniki uruchomień
są dopisywane do `SIMULATION_V2_PROGRESS.md` po zakończeniu testów.
