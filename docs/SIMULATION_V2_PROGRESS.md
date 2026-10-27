# Simulation v2 — postęp i punkty wznowienia

Gałąź: `feature/simulation-v2`, utworzona z `6095d31` gałęzi
`feature/qml-competition-config`. Wymagania: [NEW_SIMULATION.md](../NEW_SIMULATION.md).
Stan: zakończono wstępną analizę i otrzymano odpowiedzi na wszystkie 12 pytań.
Odpowiedzi i wytyczne „Wiatr w UX” zapisano w specyfikacji. Rdzeń jest w budowie;
dotychczasowy silnik pozostaje aktywny do zakończenia integracji.

## Ustalenia z kodu

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
- [ ] Rozszerzyć skocznie o nową parę kierunku wiatru; stare zestawy trzech par
  odczytywać z neutralnym nowym perkiem. Wpływ kierunku i loteryjności skoczni
  stosować do automatycznych propozycji, bez ukrytych zmian ręcznych ustawień.
- [ ] Przygotować niezależny od UI rdzeń: wejście skoku, kontekst wydarzenia,
  konfiguracja współczynników i wynik z danymi do analizy.
- [ ] Oddzielić przez interfejsy źródło losowości i warunki pogodowe od rdzenia;
  unikać mutowalnych globalnych ustawień w pracy równoległej.
- [ ] Zaimplementować OVR, niezależną formę, perki zawodnika i skoczni oraz
  ważność wydarzenia jako kategorię, zamiast starej liczby.
- [ ] Zaimplementować odległość, belkę i skończony zapas lotu z konfigurowalnymi
  współczynnikami, bez rzeczywistej trajektorii fizycznej.
- [ ] Dodać ciągłą pogodę w przestrzeni i umownym czasie konkursu, korelacje,
  lokalne różnice i gwałtowne podmuchy. Ręczne i masowe symulowanie nie może
  zależeć od szybkości klikania ani czasu renderowania UI.
- [ ] Zweryfikować rzeczywistą metodę uśredniania pomiarów wiatru i oddzielić
  ją od wpływu wiatru na lot oraz przeliczania rekompensaty.
- [ ] Zapisać przybliżony podgląd korytarzy konkretnego skoku, możliwy do
  odtworzenia w konkursie i archiwum także po zmianie algorytmu.
- [ ] Dodać opady 0–10, zmiany w konkursie i pięć prostych scenariuszy.
  Kandydat na piąty scenariusz: narastające opady. Sugestie daty/kraju mają być
  edytowalne przed konkursem; nie traktować ich jako dokładnej prognozy pogody.
- [ ] Przebudować lądowanie i noty: płynne ryzyko w zależności od real HS,
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
powtarzalność równoległą. Moduł jest testowany samodzielnie i nie został jeszcze
podłączony do gry ani aktualnego projektu qmake.

## Jakość i krzywa K

Zaimplementowano `HillSkillCurve` (edytowalne punkty, monotoniczna interpolacja
PCHIP, jawne zachowanie poza zakresem) i `QualityModel`: OVR/forma, trzy pary
perkó​​w, kategorie ważności oraz parametryzowany rozkład normalny/Laplace'a.
Podział 65/35 dotyczy siły wpływu, nie częstości losowania. Klasa lądowania
nie wpływa na jakość. Oba moduły nadal są oddzielone od aktywnej gry.

Testy przechodzą, w tym porównania po 200 tys. próbek. Opis wzorów i początkowych
współczynników: [SIMULATION_V2_ENGINE.md](SIMULATION_V2_ENGINE.md).
