# Laboratorium balansu — projekt do uzgodnienia, 29.10.2026

## Kierunek potwierdzony przez użytkownika

Nowe laboratorium powstanie obok aktualnego playgroundu. Obecny playground
pozostaje dostępny. Inspiracją jest lokalnie przejrzane laboratorium projektu
Kariera Skoczka: pytanie/rodzaj eksperymentu, panel ustawień po lewej i wyniki obok.
Nie przenosimy modelu karier ani wyników z innej gry.

To narzędzie developerskie do pełnego balansu Sj.Sima. Gracz docelowo otrzyma
jedynie 1–5 prostych, opisowych ustawień opartych na przygotowanych wariantach
balansu. Ich wybór nastąpi po kalibracji. Publiczny import alternatywnego silnika
z JSON, modowanie i instrukcje do tego są poza obecnym zakresem.

## Układ i sposób pracy

- Strona wejściowa zawiera pytania, pogrupowane w rozszerzalne obszary.
  Nie ograniczamy systemu do sześciu zakładek.
- Po wybraniu pytania po lewej są dane próby i istotne reguły silnika;
  po prawej wykresy i interpretacja wyniku. Parametry techniczne są rozwijalne.
- Dane próby: paczka/stawka, wybrany skoczek, skocznia, warunki, belka i zasady.
  Reguły: współczynniki odpowiedzialne za badany mechanizm.
- Można zmienić jeden profil przy niezmienionych rywalach albo reguły dla całej
  skopiowanej stawki. Osobno jawna zmiana danych całej stawki, np. formy.
- Odpowiedź 6: edycja jednego skoczka, przesunięcie wartości wybranej grupy
  (np. +10 formy, z zachowaniem różnic poza jawnym ograniczeniem 1–100) oraz
  ustawienie grupie jednej wartości do kontrolowanej próby. Pokazać, ilu skoczków
  osiągnęło granicę, aby przesunięcie nie udawało zachowania wszystkich różnic.
- Trzy przełączalne serie: bieżące ustawienia, zamrożony punkt odniesienia
  i ustawienia gry. Porównania wykorzystują wspólne seedy.
- Mały podgląd przelicza się automatycznie w tle; duża próba jest uruchamiana
  świadomie. Wyniki pokazują liczebność, niepewność i aktualność konfiguracji.
- Wykresy mają opisane osie, jednostki, legendę, wskazanie wartości i wyjaśnienie
  pytania. Kliknięcie komórki mapy lub punktu umożliwia przejście do szczegółów.
- Odpowiedź 7: w lądowaniu obok siebie krzywe dla zadanej odległości i wyniki
  pełnych skoków. Pierwszy widok izoluje mechanizm, drugi uwzględnia rozkład
  rzeczywiście osiąganych odległości i warunki. Na małym ekranie układ pionowy.
- Odpowiedź 8: od początku edytowalne cele i zbiorczy raport poprawy/pogorszenia
  po zmianie balansu. Cele obejmują również prawdopodobieństwa, np. upadek przy
  konkretnej odległości, nie tylko średnie różnice między wariantami.
  Automatyczne poszukiwanie współczynników pozostaje na później.
- Profile balansu można zapisać, porównać i osobno zastosować w grze. Edycja
  eksperymentu nie zmienia gry. Zastosowanie wymaga ustalenia zakresu nowej gry,
  istniejących zapisów i trwającego konkursu przed wdrożeniem tej funkcji.

## Obszary i gotowe pytania

| Obszar | Przykładowe pytania i wyniki |
|---|---|
| OVR i forma | Ile daje +10 OVR lub formy? Mapa OVR × forma; metry, punkty, miejsca i podium na własnej stawce. Porównanie na małej, dużej i mamuciej skoczni. |
| Rozrzut jakości | Jak często skoczek zawodzi lub skacze wyjątkowo dobrze? Histogram, kwantyle, ogony i wpływ rodzaju serii. |
| Generator pogody | Jak często losuje się cisza, wiatr tylny, boczny i skrajne warunki? Rozkłady presetów i ich częstości, zależność od perków skoczni. |
| Pole wiatru | Jak wiatr zmienia się w czasie konkursu i wzdłuż zeskoku? Przekroje, mapa z legendą, korelacja i długość epizodów. |
| Wykorzystanie wiatru | Ile metrów daje konkretna siła i kierunek? Pasma wyników, szansa straty, kontrolowane porównanie z ciszą i odtwarzanie lotu. |
| Belka | Kiedy opłaca się niższa belka? Mapa belka × wiatr, metry, punkty i ryzyko lądowania; osobno decyzja trenera i jury. |
| Pomiar i punktacja | Czy rekompensata wyrównuje różnice? Surowy lot, odczuty wiatr, pomiar i końcowe punkty; porównanie metod pomiaru. |
| Geometria skoczni | Jak K, HS, wypłaszczenie i charakter skoczni wpływają na odległość i skrajne wyniki? |
| Lądowanie | Prawdopodobieństwa telemarku, dwóch nóg, podpórki i upadku względem metrów lub real HS; wpływ bocznego wiatru; krzywe i pełne próby. |
| Noty | Co daje klasa lądowania? Rozkład not, kary i końcowe punkty, osobno od prawdopodobieństwa rodzaju lądowania. |
| Perki | Brak i wszystkie poziomy obu stron; skutki na różnych K, w różnych seriach, warunkach i dla różnych profili. Interakcje parami zamiast nieczytelnej pełnej kombinatoryki. |
| Opady | Śnieg/deszcz, natężenie i przebieg podczas konkursu, wpływ na zapas i wyniki. |
| Pełny konkurs | Jak zmieniają się miejsca całej paczki, awanse, zwycięzcy i skrajne konkursy przy wspólnym zegarze? |
| Dyskwalifikacje i zasady | Jak ryzyko DSQ, limity awansu, kolejność startów i odstęp między skokami zmieniają wyniki? Oddzielne liczniki startów, skoków i klasyfikowanych wyników. |
| Cele balansu | Czy zapisane oczekiwania nadal są spełnione po zmianie parametrów? Zakresy z niepewnością i porównanie profili. |

Pytania są konfiguracjami wspólnego mechanizmu eksperymentów, nie osobnymi
kopiami silnika. Każdy panel otrzymuje istotne parametry i zależności, np.
lądowanie udostępnia również geometrię i wiatr boczny, a nie tylko grupę landing.
Rozróżniamy analityczne krzywe modelu, małe próby i statystyki pełnych konkursów.
Wynik miejsca zależy od stawki, awansu i punktacji; nie zastępuje wpływu w metrach.

## Podstawy silnika wdrażane przed nowym UI

### Forma zależna od K

Udział formy = clamp(1 − overallWeight + A × tanh((K − K0) / S), 0, 1).
Domyślnie K0=120, S=100 m, A=0,04. OVR ma pozostały udział.
Na K90 udział formy wynosi około 28,8%, K120 30%, K220 33%.
Zmiana jest płynna, ograniczona i wyłączalna amplitudą 0.

### Wykorzystanie wiatru

Raz na skok losujemy oddzielny, deterministyczny strumień reakcji:
z=tanh(N(0,1)), kwantyl złego zdarzenia U(0,1), siłę U(0,5;1).
Te same wartości obowiązują podczas całego lotu i manipulacji dystansu.
Nie losujemy ponownie przy odczycie pola ani przy zmianie kroku całkowania.

Rozrzut wykorzystania rośnie jako v²/(v²+s²); przy słabym wietrze zanika.
Osobne siły rozrzutu dotyczą wiatru przedniego, tylnego i bocznego.
Pomoc przedniego wiatru nasyca się jako v/sqrt(1+(v/saturation)²).
Prawdopodobieństwo złego zdarzenia rośnie płynnie jako pMax*r^e/(1+r^e),
gdzie r=v/onset. Oddzielne maksymalne szanse i straty dla trzech kierunków.
Strata zużywa zapas podczas lotu, nie odejmuje arbitralnie metrów na końcu.

Pomiar do rekompensaty nadal czyta rzeczywiste pole. Wylosowana reakcja nie
fałszuje odczytu anemometrów. Boczne ryzyko lądowania nadal zależy od rzeczywistego
wiatru. Podgląd bez RNG pokazuje reakcję neutralną, a nie średnią rozkładu zdarzeń.
Losowanie reakcji ma identyczny koszt RNG w ciszy i w wietrze; odtworzenie wymaga
tej samej wersji programu. Dawny seed nie gwarantuje dawnego wyniku po zmianie kodu.

### Cauchy z Kariery Skoczka

Przejrzany kod random.ts stosuje x/(1+d*x²). Dla dodatniego d wartość ma maksimum
bezwzględne 1/(2*sqrt(d)); coraz większe |x| za maksimum wraca w kierunku zera.
To tłumienie nie jest monotonicznym ograniczaniem ogonów. Dlatego na początek
wiatr używa ograniczonego rozrzutu i jawnej szansy strat. Damped Cauchy można
później porównać jako alternatywę, jeśli rozkłady wyników pokażą taką potrzebę.

## Kolejność dalszej pracy

1. Zatwierdzić zakres pytań i paneli; obecny dokument jest propozycją, nie gotowym UI.
2. Dodać osobne wejście i wspólny układ pytanie → ustawienia → wykresy.
3. Uruchomić pionowe przekroje: OVR/forma na paczce, lądowanie i wiatr.
4. Uzupełnić obszary oraz ekspozycję generatora presetów pogody i wszystkie zależności.
5. Dodać profile/stosowanie, cele kontrolne i przeprowadzić kalibrację.

Wymagania przekrojowe: pełne PL/EN/CS, małe laptopy, płynna edycja, anulowanie,
zapis odtwarzalnych eksperymentów, zgodność z grą oraz CLI. Żaden wynik z małej
próby nie powinien wyglądać jak potwierdzony balans. KO i drużyny wymagają osobnego
ustalenia zakresu nowego laboratorium; obecny playground obsługuje indywidualne.
