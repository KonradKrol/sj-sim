# Silnik symulacji v2

Kod w `source/simulation-v2` jest niezależny od Qt i interfejsu użytkownika.
Do czasu zakończenia integracji gra nadal uruchamia dotychczasowy silnik.
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
