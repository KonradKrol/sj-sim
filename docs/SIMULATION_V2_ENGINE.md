# Silnik symulacji v2

Rdzeń w `source/simulation-v2` jest niezależny od Qt i interfejsu użytkownika.
Osobne pliki `ProfileBridge`, `ProfileMigration` i `GameSimulationBridge`
integrują go z Qt oraz dotychczasowymi obiektami gry.
Dotychczasowe dane nadal używają starego silnika. Adapter uruchamia v2 dla
obiektów z nowymi profilami; domyślna migracja katalogu i przebudowa UI
nie są jeszcze zakończone.
Współczynniki poniżej są punktem wyjścia do kalibracji, nie modelem
potwierdzonym na danych rzeczywistych zawodów.

## Jakość zawodnika

`QualityModel` otrzymuje profil zawodnika, K, preferencję skoczni,
kategorię wydarzenia i wstrzyknięte źródło losowości.

- Jakość bazowa: `overallWeight × OVR + (1 − overallWeight) × forma`.
  Domyślnie 70/30; oba parametry zawodnika pozostają niezależne, 1–100.
- Nie ma dwóch ukrytych umiejętności wybicia i lotu. Zawodnik bez perka
  specjalizacji zachowuje tę samą jakość na skoczniach premiujących
  wybicie lub lot.
- Udział wybicia przechodzi dokładnie przez osiem punktów użytkownika
  z `NEW_SIMULATION.md`. Interpolacja PCHIP zachowuje monotoniczność,
  ciągłość wartości i pochodnej wewnątrz zakresu oraz brak nadstrzałów.
  Domyślnie poza K50–250 utrzymuje skrajne wartości. Alternatywnie można
  wybrać liniowe przedłużenie z ograniczeniem udziałów do 0–1.
- Preferencja skoczni przesuwa udział wybicia o
  `bias × hillPreferenceStrength × min(wybicie, lot)` z ograniczeniem 0–1.
  Dodatni bias premiuje wybicie. Lotnik otrzymuje
  `specialistBias[poziom] × (1 − 2 × udział wybicia)`; specjalista małych
  skoczni przeciwny znak. Domyślne siły poziomów: 5, 9, 14 punktów jakości.
- Błyszczy na treningach: +1,5/+3/+5 jakości na treningu i próbie,
  połowa bonusu w kwalifikacjach oraz −0,75/−1,5/−2,5 w konkursie.
  Oszczędza siły: −3/−5,5/−8 na treningu/próbie, odchylenie ×1,15/1,35/1,6;
  brak bonusu konkursowego.
- Klasa lądowania nie uczestniczy w obliczaniu jakości skoku.

## Losowość jakości

Rozkład bazowy jest normalny lub Laplace'a o tej samej wariancji.
Odchylenie w punktach jakości: trening 7, próba 5,8, kwalifikacje 4,2,
konkurs 3,5. Wszystkie wartości są konfigurowalne.

Symetryczny wynik losowania `z` jest mnożony przez `2p` dla ujemnego
odchylenia i `2(1−p)` dla dodatniego. Domyślne `p=0,65` oznacza udział
oczekiwanej bezwzględnej siły ujemnego wpływu w całkowitym wpływie,
**nie prawdopodobieństwo zdarzenia ani procent utraty odległości**.
Przy neutralnych perkach połowa odchyleń jest dodatnia, połowa ujemna.

Regularny zmniejsza ujemne odchylenia do 80/60/40%, dodatnie do
92/84/76%. Nieprzewidywalny zwiększa ujemne do 120/145/175%, dodatnie
do 107/115/125%. Zmienia to również średnią w pożądanym kierunku.
Jakość po losowaniu nie jest arbitralnie obcinana do zakresu OVR.

## Weryfikacja

`scripts/test-simulation-v2-core.sh` sprawdza granice i błędne parametry,
dokładność punktów krzywej, jej monotoniczność, znaczenie perków,
proporcję wpływu 65/35 i częstość stron w dużej próbie, alternatywę
Laplace'a oraz powtarzalność generatorów w zadaniach równoległych.

## Pogoda ciągła

`WeatherField` oddziela silnik skoku od dostawcy warunków. `ContinuousWeather`
jest niezmienny i bezpieczny do równoległego odczytu. Zwraca składową przednią
i boczną w m/s dla odległości w metrach oraz umownego czasu konkursu w sekundach.
Czasu nie bierze z zegara systemowego: tę samą sekwencję skoków można rozegrać
ręcznie albo masowo bez wpływu wydajności UI na pogodę.

Pole składa się z:

- referencyjnych wektorów interpolowanych gładko, bez nadstrzałów składowych;
  pierwszy/ostatni wektor obowiązuje również przed/za zakresem punktów;
- wspólnej zmiany w czasie i lokalnego, skorelowanego pola przestrzeń–czas;
  wartości na siatce są wyprowadzane z seeda i współrzędnych, z interpolacją
  smoothstep, zamiast ponownie losowane przy każdym odczycie;
- krótkich podmuchów o czasie życia, szerokości i prędkości przemieszczania;
  ich obwiednia płynnie narasta i zanika w czasie i przestrzeni;
- jawnego ograniczenia maksymalnej siły wektora.

Referencje, amplitudy, skale korelacji oraz siła, częstość, szerokość i prędkość
podmuchów są parametrami. Zerowe amplitudy pozwalają uzyskać stałe warunki.
Duże różnice i odwrócenie kierunku na zeskoku są dozwolone. Identyczne
współrzędne i seed dają identyczny odczyt niezależnie od kolejności zapytań.

`proposeWeather` tworzy edytowalny punkt wyjścia. Na III poziomie stabilności
proponuje spokojny charakter w 85% losowań, loteryjności w 8%. Silny perk
kierunku daje 94% szans preferowanego znaku początkowej składowej przedniej.
To początkowe współczynniki, nie statystyka rzeczywistej skoczni. Gotowe pole
nie otrzymuje perków skoczni, więc ręczne warunki nie są nimi potajemnie zmieniane.

Opady są w zakresie 0–10: brak, stałe, zmienne, śnieżyca/ulewa, narastające.
Intensywność, zmienność, korelacja i tempo narastania są edytowalne. Rodzaj
wynika z nawierzchni. Miesiąc i kraj służą wyłącznie do zgrubnej, edytowalnej
propozycji; to nie model klimatu. Sam wpływ opadów na skok wdraża moduł lotu.

Testy obejmują ciągłość, korelację pobliskich odczytów, ewolucję, powtarzalność
przy odwróconej kolejności i ośmiu wątkach, skrajne parametry, wszystkie warianty
opadów oraz po 10 tys. propozycji dla przeciwstawnych perków.

### Status pomiaru do rekompensaty

Pole pogody nie narzuca sposobu uśredniania do punktacji. W publicznych materiałach
FIS i producenta nie potwierdzono jeszcze dokładnych wag i okien czasowych;
nie deklarować pełnej zgodności z FIS. Producent potwierdza osobną aplikację
pomiaru i przetwarzania danych wszystkich anemometrów dla systemu Wind/Gate,
ale karta produktu nie publikuje pełnego algorytmu.
[Swiss Timing — system konkursowy](https://www.swisstiming.com/fileadmin/Resources/Data/Datasheets/DOCM_SJ_Competition-System_1215_EN.pdf).

## Zapas lotu, belka i zeskok

`Engine` składa `QualityModel`, `FlightModel`, `LandingModel` i osobną strategię
pomiaru wiatru. Przyjmuje wyłącznie wartości wejściowe, lokalny RNG i pogodę.
Nie modyfikuje profili, UI, globalnych ustawień ani pola pogody. Jedną instancję
można bezpiecznie współdzielić między zadaniami z oddzielnymi RNG.

Początkowy zapas ma jednostki umowne odpowiadające metrom spokojnego lotu:

```
baza = K × 0,95 + (jakość − 70) × 1,107142857 × K/120
belka = (belka rzeczywista − 10) × 2,5 × K/120
        × (1 + 0,04 × max(0, −wiatr przy starcie))
zapas = max(0, (baza + belka) × (1 − 0,008 × opady))
```

Wszystkie współczynniki są w `FlightParameters`. Dozwolony zakres zapasu
ma dodatkowy techniczny limit miliona jednostek dla skrajnych eksperymentów.
Nie jest to późniejsze skracanie wyniku. Wyższa belka może nieco mocniej pomagać
przy tylnym wietrze; ten wpływ można wyzerować.

Lot jest całkowany domyślnie krokami 1 m, z dokładnym domknięciem ostatniego
częściowego kroku. Wiatr jest odczytywany na początku osiągniętego kroku,
więc nieosiągnięty podmuch nie może wpływać na odległość. Reakcja na składową
przednią narasta/zanika wykładniczo ze skalą 5 m. Wpływ rośnie od 0,55 do
1,1 wzdłuż K. Boczny wiatr zwiększa zużycie oraz osobno ryzyko lądowania;
nie ma nieciągłego przeskoku przy przejściu przez kierunek boczny.

Trudność zdobywania metrów rośnie już od `0,94 × realHS`. Użyteczny zeskok
kończy się przy `realHS + 0,12 × K`; oba współczynniki są edytowalne.
Jeśli `p` jest stopniem przebycia odcinka od początku wypłaszczenia do końca,
koszt bazowy metra wynosi `1 + 1,8 × p² / (1−p)`. Wpływ jest częścią lotu,
przed ustaleniem odległości. Nie ma losowego skracania gotowego skoku.
Wiatr nie może pokryć więcej niż 65% kosztu metra. Limit domeny zeskoku,
dodatnie zużycie i ograniczona liczba kroków gwarantują koniec skoku.

Czas lotu wynika z umownej skali 4 s na K, służącej wyłącznie do odczytu
zmiennej pogody. Nie wyliczamy fizycznej wysokości ani prędkości zawodnika.

Kalibracja początkowa: 20 tys. sparowanych seedów na K120/HS134, forma 70,
belka 10, cisza, neutralne perki, konkurs: OVR90–OVR70 daje średnio **15,35 m**.
To test założonego celu 14–17 m, nie potwierdzenie realizmu całego silnika.

## Lądowanie i noty

`LandingModel::probabilities` udostępnia jawne krzywe do analizy bez losowania.
Gładkie funkcje logistyczne zależą od `odległość / realHS` i bezwzględnej
składowej bocznej. Najpierw wyznaczają upadek, potem podpórkę wśród pozostałych,
potem dwie nogi; reszta to telemark. Parametry środka, szerokości, minimum
i wzrostu każdego ryzyka są niezależne. Klasa lądowania zawodnika nie zmienia
tych prawdopodobieństw ani lotu; wpływa wyłącznie na noty.

Początkowe średnie klas to 16,5 / 17,25 / 18 / 18,75 / 19,5. Wspólne
odchylenie stylu ma σ=0,35, odchylenie pojedynczego sędziego σ=0,25.
Dodatkowe kary: dwie nogi 2,5, podpórka 5, upadek 7,5. Płynna kara trudności
wynosi maksymalnie 1,2. Noty są zaokrąglane do 0,5 i ograniczone do 0–20.
Każdy parametr jest edytowalny. Wysokie noty nie wymagają osobnego rzadkiego
zdarzenia; stylista po dobrym lądowaniu regularnie uzyskuje 19,5 lub 20.

## Wynik, pomiar i ślad pogody

Odległość pomiarowa ma dokładność 0,5 m. Spośród pięciu not odrzucana jest jedna
najwyższa i jedna najniższa. Pomiar wiatru jest zaokrąglany do 0,01 m/s,
rekompensaty i wynik do 0,1 pkt. Dodatni wiatr odejmuje punkty według
współczynnika przedniego, ujemny dodaje według tylnego. Obniżenie jury liczy
się niezależnie od progu trenera; dodatkowe obniżenie trenera ma próg 95% HS.
Składniki punktacji i regułę progu można niezależnie wyłączyć.

`WindMeasurement` jest wstrzykniętą strategią. `SensorWindMeasurement` przyjmuje
**jawne** stanowiska, wagi, okno przed startem/po lądowaniu, liczbę odczytów
i opcję uwzględniania tylko osiągniętych stanowisk. To konfigurowalny model
eksperymentalny; brak tu domniemania dokładnej zgodności z FIS. Pomiar nie jest
średnią osobistej ekspozycji i nie odwraca matematycznie efektu na lot.

Wynik lotu może zawierać kompaktowy zestaw sześciu sekcji: granice, przebytą
długość, średni doświadczony wektor i min/max składowej przedniej oraz odczyt
przed i po skoku. Nieprzebyte odcinki mają przebytą długość 0. W batchach
można wyłączyć zbieranie sekcji. Adapter gry zapisuje te dane w `JumpData`;
sam wynik rdzenia nie zapisuje plików. Podgląd w UI pozostaje do wdrożenia.

DNS kończy obliczenie przed losowaniem, DSQ przed lotem i punktacją, oba mają
0 punktów. `previewDistance` nie pobiera losowości, ale używa tego samego
modelu jakości bazowej, opadów, belki, pogody i geometrii co pełny skok.

Testy dodatkowo obejmują uratowanie krótkiego skoku przez osiągnięty podmuch,
brak wpływu nieosiągniętego wiatru, malejący przyrost długich skoków, zbieżność
kroku 1 m do 0,1 m, prawidłową sumę ekspozycji, niezależność stylu od długości,
przykład −3,6 pkt za +0,39 m/s, próg trenera, pełny silnik na kilku wątkach
oraz 4000 losowych skrajnych konfiguracji.

## Jawne manipulacje

`JumpManipulation` ma niezależne korekty jakości, odległości, opadów i pomiaru
wiatru: bonus oraz opcjonalny zakres. Osobno można wymusić typ lądowania,
poszczególne noty sędziów (także 0) i prawdopodobieństwo DSQ. Puste ustawienia
nie zmieniają obliczeń. Przeciwny kierunek/siłę faktycznego wiatru ustawia się
przez pole pogody, niezależnie od samego pomiaru punktowego.

Wymuszenie odległości dostosowuje zapas i ponownie przelicza lot przez te same
warunki, dzięki czemu ślad wiatru odpowiada rzeczywiście osiągniętym odcinkom.
Nie dokleja się historycznych warunków krótszego skoku do zmienionej liczby
metrów. Ta opcjonalna ścieżka jest celowo droższa od zwykłego skoku: poszukuje
zapasu bisekcją. Żądanie poza użytecznym zeskokiem zatrzymuje się na jego końcu
i ustawia `distanceLimitedByGeometry`. To jawne wymuszenie użytkownika, oddzielne
od normalnego modelu; standardowy skok nie jest skracany po fakcie.

Podgląd odległości uwzględnia te same korekty. Wynik zapisuje zarówno fakt
manipulacji, jak i jej wpływ na jakość. Nowy UX manipulacji pozostaje do wdrożenia.

## Integracja z grą i migracja paczek

`ProfileBridge` mapuje nowe profile do wejść rdzenia; nie przelicza dawnych
technik na ukryte umiejętności. `Jumper` i `Hill` przechowują opcjonalny nowy
profil wraz z dotychczasową tożsamością. Gettery profilu odzwierciedlają aktualne
dane wspólne (np. nazwę, rekord i HS), zapobiegając rozjechaniu dwóch kopii.
Paczka w formacie katalogu z nowymi profilami ma wersję 2. Gotowe paczki nowych
edytorów można odczytać bez przybliżania ich parametrów. Walidacja kontroluje
również zgodność ID nowego profilu z obiektem przechowywanym przez grę.

`GameSimulationBridge` implementuje ścieżkę v2 istniejącego `JumpSimulator`.
Nowa para profili kieruje obliczenia do nowego silnika. Łączenie nowego profilu
z dawnym jest błędem konfiguracji; integracja katalogu musi uniemożliwić taki
start i pokazać wyjaśnienie przed otwarciem konkursu. Przejściowo dotychczasowe
pary danych mogą nadal wywoływać dawny silnik. Nie jest to zakończona migracja.

Adapter otrzymuje kategorię wydarzenia i potrafi przyjąć wspólne pole pogody
z własną strategią pomiaru. Przy braku takiego pola tworzy stałe w czasie,
gładko interpolowane referencje z dawnych korytarzy, z ich dotychczasowym
pomiarem do punktacji. Nowy UX pogody ma zastąpić tę przejściową konfigurację.
Domyślny logiczny odstęp wynosi 30 s; kontroler może jawnie podać czas.
Batchom należy przydzielać czas po indeksie zdarzenia, niezależnie od wątków.

Wynik zachowuje wskaźniki zawodnika/skoczni używane przez obecną logikę wyników.
W JSON `JumpData` ma dodatkowy obiekt `simulation-v2`: wersję silnika, seed
jako tekst (bez utraty precyzji JSON), logiczny czas, kategorię wydarzenia,
składniki jakości, zapas, surową odległość, opady, ekspozycję i przekroje wiatru.
Po ponownym wczytaniu korzystamy z zapisanych odczytów, nie ponownego losowania.

`ProfileMigration::approximateLegacyPack` tworzy nową, oddzielną paczkę z nowymi
ID i informacją o pochodzeniu. OVR = średnia dawnych technik × 100/dawna skala,
forma = dawna forma × 100/dawna skala, z ograniczeniem 1–100. Lądowanie jest
przybliżane do jednej z pięciu klas; wszystkie perki zaczynają od zera. Funkcja
zwraca wyraźne ostrzeżenie o niedokładnym przybliżeniu, nie modyfikuje źródła
i odrzuca ponowne migrowanie gotowych profili. Mapowanie zdjęć ma unikalny
podfolder zawodnika, aby identyczne nazwy plików nie nadpisywały innych zdjęć.
Fizyczne kopiowanie wykonuje `ProfilePackRepository`, a wspólny serwis źródeł
implementuje `SimulationCatalog`. Podłączenie serwisu do okien gry nadal trwa.

Testy integracji: `scripts/test-quick-ui.sh --simulation-v2-only`.
Obejmują migrację i jej pochodzenie, zapis/odczyt modeli i pogody, tożsamość
wyników, niezależność od dawnych technik, preview, jawny czas/pole, wymuszenia
oraz zachowanie profilu skoczni przy odczycie konfiguracji.

## Jedno źródło danych profili

Nowe edytory korzystają z `ProfilePackRepository`. Te same pliki czyta
`SimulationCatalog`, który zwraca własny snapshot gotowy do konfiguracji gry.
Indeks katalogu zapisuje tylko ID/kolejność, nie kopiuje wszystkich profili.
Przybliżone migracje i ich zdjęcia są publikowane atomowo jako osobne paczki,
idempotentnie według źródłowego ID. Gotowe paczki i pliki źródłowe nie są
nadpisywane. Brak zdjęcia nie blokuje zawodnika; problemy są zwracane do UI.

Formaty są osobne:

- `sj-sim-athlete-pack` / `sj-sim-hill-pack`, wersja 1: pliki edytorów;
  profil skoczni obsługuje trzy dawne lub cztery obecne pary perków.
- `sj-sim-pack`, wersja 2: reprezentacja katalogowa nowych profili w `DataPack`.
  Kompatybilne paczki zasad pozostają w swoim formacie wersji 1.
- `sj-sim-simulation-v2-catalog`, wersja 1: tylko metadane wyboru/kolejności.
- `JumpData.simulation-v2.engineVersion = 2`: zapis wyników/warunków skoku.
- Nowy format sezonu: jeszcze do wdrożenia.

`Jumper` otrzymuje nietrwały kontekst folderu swojej paczki. Zdjęcie rozwiązuje
względem tego folderu, z kontrolą, czy plik pozostaje w jego obrębie. W JSON
zapisuje tylko ID paczki i względną ścieżkę. Po odczytaniu sezonu kontekst
plików trzeba przywrócić przez warstwę danych; domenowy parser nie zakłada
lokalizacji plików konkretnego komputera. Brak pakietu zdjęć oznacza pusty
portret, bez wpływu na obliczenia.

## Zmiany tendencji i okresy niestabilności — 28.10.2026

`ContinuousWeather` ma dodatkową, opcjonalną ewolucję dominującego wiatru:

- `trendAmplitude` (m/s, domyślnie 0) — zakres przesunięcia ogólnego wektora
  względem referencji. Może odwrócić kierunek, gdy przekroczy wyjściową siłę.
- `trendPeriodSeconds` (domyślnie 180 s) — orientacyjny czas jednej tendencji.
  Granice są deterministycznie przesunięte o maksymalnie 25% okresu; pojedyncza
  tendencja może zatem utrzymywać się przez różną liczbę skoków.
- `trendTransitionSeconds` (15 s, maks. 30% okresu) — płynne przejście między
  tendencjami, z dłuższym stałym poziomem pomiędzy przejściami.
- `activityVariation` (0..1, domyślnie 0), `activityCorrelationSeconds` (180 s)
  oraz `fastCorrelationSeconds` (8 s) — okresy spokojnych i szybkich wahań.
  Mieszają wolny przebieg z szybszym oraz zmieniają jego natężenie. Nie wyliczają
  czasu przez mnożenie bezwzględnego zegara przez zmienną częstotliwość, co
  powodowałoby sztuczne skoki fazy pod koniec długiego konkursu.

Zmiana ogólnej tendencji jest wspólna dla skoczni, ale nie usuwa różnic lokalnych.
Lokalne podmuchy nadal mogą przejść przez jedynie fragment zeskoku. Referencje są
punktem wyjścia funkcji od miejsca i bezwzględnego czasu, a nie niezmiennym wiatrem.
Implementacja pozostaje bezstanowa i powtarzalna; odczyt UI lub inna kolejność
pracy wątków nie przesuwa pogody. Stałe ustawienia z zerowymi amplitudami nie
uruchamiają nowych warstw.

Automatyczne propozycje mają różne wartości dla spokojnego/zmiennego/loteryjnego
charakteru: amplituda tendencji 0,15/1,2/2,8 m/s, okres 300/150/60 s, przejście
30/15/5 s, zmienność aktywności 0,15/0,5/0,95. To parametry początkowe do strojenia,
nie statystyki rzeczywistej pogody. Perki wpływają na wybór propozycji; ręcznie
podane parametry nie otrzymują dodatkowego ukrytego mnożnika od skoczni.

**Ta warstwa jest gotowa w rdzeniu i propozycjach, ale UI gry nie przekazuje
jeszcze wspólnego nowego pola.** Pojedyncze skoki nadal używają adaptera dawnych
ustawień wiatru. Nie deklarować, że nowe tendencje są już aktywne w zwykłym konkursie.
