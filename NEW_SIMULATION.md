> **Wznowienie pracy w nowej sesji:** najpierw przeczytaj
> [docs/SIMULATION_V2_PROGRESS.md](docs/SIMULATION_V2_PROGRESS.md), potem
> [docs/SIMULATION_V2_ENGINE.md](docs/SIMULATION_V2_ENGINE.md).
> Ten plik jest specyfikacją i zawiera także już wykonane polecenia startowe.
> Gałąź `feature/simulation-v2` już istnieje. Nie twórz jej ponownie, nie cofaj
> wykonanych zmian i nie pracuj na mainie. Implementacja całości trwa;
> opis rdzenia nie oznacza zakończenia migracji gry.

Czas zmigrować na nowy system symulacji i formy. Utwórz nowy branch od aktualnego. Nazwij go `feature/simulation-v2`.

Przejdź w pełni na nowy system skoczków i skoczni, także w bazie danych.

# Nowy silnik symulacji

Teraz będzie wszystko nowe. Możliwe że inne rozkłady prawdopodobieństwa. Podaj mi zarazem od razu, potem kontynuuj pracę, jakie rozkłady prawdopodobieństwa są obecnie używane? Czy Cauchy miałby gdzieś sens albo Laplace?
Kod od początku ma być czysty i ma mieć abstrakcji trochę, Dependency Inversion principle zastostuj w miarę możliwości w paru miejscach.

Zadaj mi 10-15 pytań zanim zaczniesz, żeby pokryć wszystko o co można zapytać. Pewnie będzie wiele niejasności.

### Czynniki symulacji
##### Skoczek
+ OVR - im wyższy, tym ogólnie lepsza odległość
+ Forma - im wyższa, tym ogólnie lepsza odległość. Forma jest niezależna od poziomu skoczka, tzn. OVR i Forma są rozłączne, Forma często waha się po całym przekroju od 1 do 100.
+ Skill lądowania - wpływa tylko na noty od sędziów
+ "real hs" skoczni - wpływa na noty za styl i możliwość upadków
+ Perk regularności. "Regularny" skoczek ma mniej zepsutych skoków, średnia jest trochę wyższa, ma mniej wystrzałów pozytywnych ale ich ilość nie zmniejsza się tak bardzo jak zepsutych skoków. "Nieprzewidywalny" ma więcej zepsutych skoków, trochę więcej wystrzałów pozytywnych ale także tutaj jest więcej zepsutych skoków niżw ięcej wystrzałów pozytywnych, średnia się też troszkę obniży.
+ Perk odpuszczania skoków. Zawodnik z perkiem odpuszczania skoków mniej ważnych, ma ogólny niższy i mniej regularny poziom skoków na treningu i seriach próbnych, nie zyskuje w seriach ważniejszych. Zawodnik z perkiem przeciwnym - król treningów czy jakoś tak - zyskuje na jakości podczas treningów i serii próbnych, także ma bonus (o 50% mniejszy) w kwalifikacjach, ale w konkursach ma lekkie osłabienie.
##### Skocznia
+ K skoczni - im wyższy, tym większe bonusy/straty mają zawodnicy z perkiem lotnika/specjalisty od małych skoczni.
##### Pogoda
+ Wiatr. Trochę zmieni się obecny model wiatru i jego wpływ. Natomiast dalej są korytarze, kierunki i siły wiatru.
+ Śnieg. Śnieg też jest do przemodelowania chyba, szczególnie od strony UX i jego ustawiania. Śnieg po prostu pogarsza skoki i tyle.
##### Inne
+ Belka, tak jak teraz. Np. przy wietrze tylnym, jeśli tak nie jest zaimplementowane, raczej pomaga trochę barzdiej wyższy najazd, więc tu liniowość troszkę może być naruszona. 
+ Funkcja manipulacji skoku. TUTAJ TRZEBA TOTALNIE PRZEBUDOWAĆ TE OCHYDNE UI bezwartościowe.

+ <JEŚLI coś pominałem, daj do pytań>

### Kwestia ważności skoków.
Na razie upraszczamy system.
- Treningi - najmniej ważne skoki
- Serie próbne - tylko trochę bardziej ważne od treningów
- Kwalifikacje - już raczej ważne skoki
- Konkurs - najważniejsze skoki
Uwaga: im mniej ważna seria, tym ogólnie większa rozbieżność jakości skoków, ale 65% tej rozbieżności dotyczy psucia skoków, a tylko 35% dotyczy pozytywnych bonusów.
Usuń parametr liczbowy ważności skoków.

Noty za styl niech działają podobnie obecnie, z podobnym rozstrzałem i mechanizmem, ale obecnie za trudno dostać barzdo wysokie noty czyli przynajmniej 19,5.
W dodatku pewnie całość jest zbyt sztywna, a granice powinny być dużo luźniejsze.
Chcę w playgroundzie mieć dokładny widok na to jak zmienia się szansa na upadki czy jakie noty na jakich odległościach itd i żeby to się interaktywnie zmieniało w zależności od wielu parametrów.

Dużo mówię ogólnie, np. "lekkie osłabienie", dlatego te rzeczy mają być do zmierzenia i zparametryzowania i symulacji w playgroundzie, żeby to potem dostosować!

Niech playground pozwala KOMPLEKSOWO cały silnik symulacji przetestować. Aktualny playground nadaje się do śmietnika, nic tam wartego uwagi nie ma.

Cały silnik ma być udokumentowany gdzieś w pliku .MD.
Przed zadaniem pytań poeksploruj wpierw bazę kodu.

## Ustalenia po odpowiedziach na pytania

1. Nowy silnik obowiązuje dla nowych sezonów. Nie wymagać wstecznej kompatybilności z Sj.Sim 1.3.0-beta2 ani automatycznie przepisywać wyników starych sezonów do nowego silnika. Nie niszczyć źródłowych plików; nieobsługiwany zapis ma otrzymać czytelny komunikat.
2. Stare paczki można przybliżenie przekonwertować: OVR ze średniej dawnych technik wybicia i lotu, zachowana/przeliczona forma, przybliżona klasa lądowania i perki od zera. **Bardzo wyraźnie zaznaczać, że to niedokładna, przybliżona migracja**, nie wierne odtworzenie możliwości dawnego zawodnika.
3. Zachować gotowe nowe paczki. Stare przeliczać jako osobne paczki bez nadpisywania nowych danych.
4. Zachować trzy poziomy perków skoczków i skoczni.
5. Początkowo przyjąć około 70% wpływu OVR i 30% formy na jakość bazową. To parametr do zmiany w playgroundzie, a nie stała konstrukcyjna silnika.
6. Podział 65/35 oznacza udział **łącznego wpływu losowych strat i bonusów** po obu stronach: około 65% wpływu ujemnego i 35% dodatniego. Nie oznacza 65% słabszych odległości ani narzuconej proporcji liczby zdarzeń. Wpływ, rozkład, asymetria i różnice ważności serii mają być parametryzowalne oraz mierzalne w playgroundzie.
7. Nie skracać gotowego skoku po fakcie. Już w okolicach real HS dalsze metry mają być coraz trudniejsze do uzyskania, a wypłaszczający się zeskok ma ograniczać dalszy lot. Osobno wzrasta ryzyko trudnego lądowania i upadku. Nie dopuścić do absurdalnych lądowań daleko za końcem użytecznego zeskoku; granice i tempo wzrostu trudności muszą być związane ze skocznią i dostępne do strojenia. Playground ma pokazywać krzywe szans upadku, lądowania i not w zależności od odległości oraz parametrów.
8. Preferencję wybicia/lotu skoczni powiązać z Lotnikiem i Specjalistą małych skoczni; zawodników bez tej pary traktować neutralnie. Bazowy udział wybicia i lotu zależy od K zgodnie z poniższą krzywą. Punkty i siła wpływu mają być edytowalne.
9. Typ opadów wynika z nawierzchni: zimą śnieg, na igelicie deszcz. Data i kraj mogą sugerować scenariusz/intensywność; użytkownik zawsze może go zmienić.
10. Zachować wszystkie możliwości manipulacji zgodne z nowym symulatorem, przebudować ich UX i dodać użyteczne nowe możliwości, jeśli pasują do modelu.
11. Playground automatycznie przelicza małą próbę w tle po zmianie parametru; duże serie uruchamia się osobno. Uwzględnić anulowanie zastąpionych prób i ograniczenie częstotliwości przeliczania przy szybkiej edycji.
12. Wstępny cel kalibracji: OVR 90 wobec OVR 70 na K120, przy tej samej formie, belce i spokojnych warunkach, daje przeciętnie **14–17 m** przewagi. Cel dotyczy porównania dużych prób, nie każdego pojedynczego skoku.

### Bazowy udział wybicia i lotu zależnie od K

| K | Wybicie | Lot |
| ---: | ---: | ---: |
| 50 | 85% | 15% |
| 80 | 72% | 28% |
| 95 | 62% | 38% |
| 110 | 50% | 50% |
| 125 | 40% | 60% |
| 140 | 30% | 70% |
| 200 | 15% | 85% |
| 250 | 8% | 92% |

Między punktami zastosować ciągłą, monotoniczną krzywą bez niezamierzonych ekstremów. Zachować podane wartości w punktach. Zachowanie poza zakresem tabeli oraz modyfikacja przez perk skoczni mają być jawne i parametryzowalne; nie tworzyć nowego ukrytego, niezależnego skilla zawodnika.

# Nowy silnik formy

<na razie forma jest taka jaka jest, za niedlugo zrobimy auto-zmianę formy w sezonie>

# Zaktualizowany system opadów (śnieg lub deszcz).
Niech opady po prostu pogarszają skoki i tyle.
Niech będzie 10 poziomów, od 0 do 10.
Niech to się samo zmienia w trakcie konkursu, np na koniec konkursu większy deszcz. Dynamicznie tak jak obecnie jest bodajże.
Ale pod względem UX to ma być dużo prostsze. Niech będzie 5 opcji do wyboru ze śniegiem. Albo brak śniegu, albo stabilne opady śniegu, albo bardzo zmienne opady i nie wiadomo czego się spodziewać, albo śnieżyca absolutna. i wymyśl jeszcze jakąś opcję.
Tutaj ma być bardzo ładne kafelkowe UI z wyborami fajnymi. Satysfakcjonujące ładne UI.
W trybie sezonów jeśli są włączone daty konkursów, to w zależności od daty wymyśl algorytm wyboru opadów do konkursu, pewnie w różnych porach trochę inna szansa jest, nie wiem nawet. Weź pod uwagę kraj skoczni również. Uzytkownik może to sobie zmienić przed konkursem zawsze.

# Nowy system wiatru

## Wiatr w UX

### Automatyczne propozycje i rola perków skoczni

- Po wejściu do konfiguracji konkursu gra ma już proponować wylosowany wiatr i wariant opadów. Użytkownik widzi gotowy punkt wyjścia i może go łatwo zmienić lub świadomie wylosować ponownie.
- Perk loteryjności/stabilności skoczni ma **duży wpływ na losowanie automatycznie sugerowanego scenariusza**. Skocznia loteryjna znacznie częściej proponuje naprawdę gwałtowne i zróżnicowane warunki; stabilna częściej spokojne i podobne. Nie oznacza to zawsze identycznego scenariusza na tej samej skoczni.
- Dodać czwartą przeciwstawną parę perków skoczni: **„Dominuje wiatr przedni” / „Dominuje wiatr tylny”**. Zmienia ona prawdopodobieństwo dominującego kierunku w automatycznej propozycji, a nie gwarantuje go we wszystkich skokach ani wszystkich miejscach.
- Wpływ obu par na warunki dotyczy automatycznego proponowania/losowania. Po ręcznym ustaleniu warunków przez użytkownika perki nie mogą ukrycie ponownie podbijać loteryjności lub narzucać kierunku. Sam wybrany scenariusz nadal dynamicznie ewoluuje według jawnie ustawionego charakteru.
- Przykład oczekiwanego charakteru: Engelberg K140 może zwykle proponować mało loteryjny wiatr tylny. Wynika to z ustawień/perków konkretnej skoczni; nie kodować rozpoznawania skoczni wyłącznie po nazwie ani nie przedstawiać tego przykładu jako zweryfikowanej statystyki pogodowej.
- Rozszerzenie modelu skoczni o parę kierunku ma czytać dotychczasowe paczki z trzema parami jako brak nowego perka, zachowując istniejące wybory i identyfikatory.

### Proste ustawienie warunków

Przebudować aktualny widok wiatru i korytarzy, dopasowując go do ciągłego modelu. Główny ekran ma być nowoczesny, wyrazisty, kontrastowy i przyjemny w użyciu, z hierarchią Sj.Sima i konserwatywnymi, krótkimi etykietami. Działać na desktopie i małym laptopie, w obu motywach.

Użytkownik przede wszystkim wybiera ogólny charakter warunków: dominujący kierunek, typową siłę oraz to, czy wiatr jest raczej stabilny/loteryjny i podobny/różny wzdłuż zeskoku. Nie musi wpisywać kąta ani siły w każdym punkcie. Wybory mają tworzyć zrozumiały opis i podgląd, np.:

- Średnio mocny wiatr boczny, raczej podobny na całym zeskoku.
- Mocny, loteryjny wiatr tylny, z typową siłą około 1,8 m/s.

Dokładne wartości, kierunki i punkty odniesienia udostępnić w opcjach zaawansowanych. Proste ustawienia mają być nadrzędnym, wygodnym sposobem przygotowania konkursu, a nie dodatkowym krokiem przed obowiązkową szczegółową konfiguracją. Nie wprowadzać wielu drobiazgowych parametrów na głównym ekranie.

### Informacja podczas konkursu i po skoku

- **Domyślnie po skoku pokazywać uśredniony wiatr tak jak obecnie.** Nie zastępować tej informacji dużym widokiem sektorów i nie zwiększać obowiązkowej ilości danych.
- Dodać opcję uproszczonego podglądu kierunków i sił wiatru w poszczególnych sektorach przed skokiem i po nim; jeżeli będzie to czytelne i wykonalne, także podczas skoku. Podgląd w trakcie jest opcjonalny, nie warunek podstawowej funkcjonalności.
- Warunki mogą zmienić się między odczytem przed skokiem, przebiegiem lotu i odczytem po skoku. Przy loteryjnej pogodzie różnice mają być czasem wyraźne; obraz przed skokiem nie jest gwarancją tego, czego zawodnik doświadczy.
- Zachować czytelne rozróżnienie czasu pokazywanych danych: odczyt przed skokiem, warunki z lotu i aktualny stan po skoku. Zmiana pogody po lądowaniu nie przelicza ponownie już oddanego skoku ani jego oficjalnej wartości wiatru.
- Nierówność warunków i niedoskonałe wyrównanie przez pomiar/rekompensatę mogą naturalnie tworzyć niesprawiedliwe sytuacje. Ma to wynikać z przebiegu pogody oraz rzeczywistego sposobu uśredniania, a nie osobnego losowania krzywdzącej korekty po poznaniu wyniku.

## Ciągły model wiatru i lotu

Doprecyzowanie użytkownika 28.10.2026: **zmienia się także ogólna tendencja pola**,
nie tylko drobne podmuchy wokół stałej bazy. Mogą występować okresy kilku skoków
z innym dominującym wiatrem, a także odcinki konkursu, w których warunki zmieniają
się niemal co skok. Nie każdy konkurs ma być loteryjny: charakter ustala użytkownik,
a osobne parametry silnika kontrolują siłę zmian, ich czas i podmuchy. Spokojne
ustawienia mają rzeczywiście dawać spokojny przebieg, stałe — brak zmian w czasie.
Pytania o główny wybór zmienności oraz dopuszczanie odwróceń ręcznie wybranego
kierunku zostały zadane 28.10; przed zamknięciem UX zapisać odpowiedzi.


### Założenia

Wiatr jest **ciągłym polem warunków wzdłuż skoczni, zmieniającym się w czasie**. Skoczek odczuwa warunki napotkane w czasie swojego lotu, a nie jedną średnią dla całego skoku. To nadal ma być szybka symulacja oparta na losowości i wzorach możliwych do dostrojenia — bez konieczności obliczania rzeczywistej wysokości, prędkości, trajektorii i pełnej aerodynamiki.

Korytarze, kierunki i siły wiatru pozostają zrozumiałe dla użytkownika. Wartości w wybranych miejscach są punktami odniesienia; silnik wyznacza pomiędzy nimi płynne warunki. W UI trzeba jasno pokazać, co oznaczają te wartości i gdzie obowiązują, bez wymagania ręcznego ustawiania wielu dodatkowych parametrów.

### Warunki na skoczni

- Wiatr **zmienia się przez cały konkurs** i ma pamięć między skokami: następny zawodnik często startuje przy warunkach podobnych do poprzedniego, a siła i kierunek stopniowo się zmieniają. Czas między skokami i przerwy mogą wpływać na wielkość zmiany. Pamięć nie może jednak wygładzać wszystkiego: czasami warunki zmieniają się bardzo gwałtownie, także w trakcie skoku albo między dwoma kolejnymi zawodnikami. Dopuszczać również szybkie odwrócenie kierunku i nagłe podmuchy.
- Sąsiednie miejsca bywają powiązane, ale nie muszą mieć niemal identycznego wiatru. Istnieje ogólny wiatr na skoczni oraz lokalne odchylenia, np. bardziej osłonięty albo wystawiony fragment. Czasami jednocześnie różne fragmenty skoczni mają bardzo różne siły lub nawet przeciwne kierunki. Natężenie lokalnych różnic oraz nagłych zmian powinno wynikać z warunków, losowości i charakteru skoczni, a nie być stale takie samo. Nie losujemy niezależnie zupełnie nowego wiatru dla każdego metra.
- Podmuch ma siłę, kierunek, zasięg i czas trwania. Może przemieszczać się wzdłuż skoczni; zawodnik może trafić na niego w środku dotychczasowego korytarza i odczuć go tylko przez część lotu.
- Warunki między punktami odniesienia zmieniają się płynnie, bez sztucznych skoków na granicach. Interpolacja nie powinna sama tworzyć niezamierzonych ekstremów. Warunki w ostatnim obszarze działają aż do lądowania także przy bardzo dalekim skoku.
- Losowość ma być spójna w czasie i przestrzeni: ponowny odczyt wiatru w tym samym miejscu i chwili nie może dawać nowego, niezależnego wyniku. Ten sam seed i konfiguracja powinny pozwalać powtórzyć eksperyment.

### Dynamiczny wpływ na skok

1. Najpierw obliczamy bazową jakość skoku bez wiatru z uwzględnieniem OVR, formy, belki, skoczni, perków i losowości. Można ją przedstawić jako umowny **zapas lotu** lub inną prostą wartość wewnętrzną.
2. Przetwarzamy lot kolejno wzdłuż skoczni. Przebycie odcinka zużywa zapas, a lokalny wiatr może go stopniowo zwiększać lub zmniejszać. Gdy zapas się kończy, zawodnik ląduje — warunki dalej nie mogą wpływać na odległość.
3. Wcześniejsze warunki zmieniają zapas, z którym skoczek dociera do następnego miejsca. Dobry podmuch może przedłużyć lot i pozwolić dotrzeć do dalszych warunków; wcześniejszy tylny wiatr może sprawić, że zawodnik tam nie doleci.
4. Reakcja na podmuch może narastać i wygasać stopniowo. Krótki impuls i stały wiatr nie muszą dawać identycznego efektu. To umowne uproszczenie, nie fizyczna wysokość ani prędkość.
5. Siła wpływu może płynnie zależeć od fazy lotu i zawodnika, bez dodatkowych losowych „jackpotów” niezwiązanych z przebiegiem skoku. Ewentualne różnice między zawodnikami mają być mierzalne i nie powinny wymagać wielu nowych parametrów w UI.
6. Kierunek rozkładamy płynnie na składową wzdłuż lotu i boczną. Pierwsza wpływa przede wszystkim na odległość; boczna może wpływać na stabilność i lądowanie. Przejście przez kierunek boczny nie może powodować nagłego skoku efektu.

Współczynniki, wpływ faz lotu, bezwładność reakcji i ograniczenia skutków są do dobrania w playgroundzie. Skok musi kończyć się także przy ekstremalnych wartościach wiatru; obliczenia nie mogą produkować nieskończonego lotu ani nieprawidłowych liczb.

### Przykład: słaby skok uratowany przez podmuch

To **wyłącznie poglądowy przykład**, nie gotowe współczynniki ani deklaracja typowych warunków. Skoczek ma bazowy zapas 95 jednostek. Metr lotu normalnie zużywa 1 jednostkę, a 1 m/s wiatru zmienia zużycie o 0,1 jednostki na metr. Dodatni wiatr pomaga, ujemny szkodzi.

| Odcinek | Wiatr | Zużyty zapas | Pozostały zapas |
| --- | ---: | ---: | ---: |
| Start | — | — | 95,0 |
| 0–20 m | −2,0 m/s | 24,0 | 71,0 |
| 20–40 m | −2,5 m/s | 25,0 | 46,0 |
| 40–60 m | −3,0 m/s | 26,0 | 20,0 |
| 60–70 m | −2,0 m/s | 12,0 | 8,0 |

Gdyby wiatr −2,0 m/s pozostał taki sam, zawodnik lądowałby około 76,5 m. Jeśli na 70. metrze trafia na utrzymujący się podmuch +4,0 m/s, zapas wystarcza w tym rachunku do około 83,5 m. Podmuch uratował około 7 m, ale nie cofnął wcześniejszych strat. Gdyby zaczynał się dopiero na 80. metrze, zawodnik nie doleciałby do niego i nie miałby on wpływu. Przykład pomija stopniowe narastanie reakcji, aby pokazać mechanizm.

### Podgląd korytarzy podczas skoku

Użytkownik ma móc na życzenie zobaczyć **przybliżony obraz warunków w poszczególnych korytarzach dla konkretnego skoku** — w podglądzie skoku podczas konkursu i później w archiwum. Ma być jasne, że ciągłe pole wiatru zostało tu zagregowane do czytelnych odcinków, a pokazane liczby są przybliżeniem, nie pełnym zapisem każdej zmiany w czasie.

- Dla odcinków pokazać ich zakres, kierunek i siłę wiatru; jeśli w obrębie odcinka była duża zmiana lub podmuch, umożliwić jej zauważenie bez mylenia z jedną stałą wartością.
- Rozróżnić odcinki faktycznie przebyte przez skoczka, częściowo przebyte i te poza miejscem lądowania. Te ostatnie nie mogą być przedstawiane jako wpływające na odległość.
- Osobno pokazać wynik pomiaru użytego do rekompensaty i same punkty, aby przybliżony podgląd odczutych warunków nie wyglądał na oficjalny pomiar punktowy. Kolory kierunku uzupełnić liczbami i czytelnymi oznaczeniami.
- Zapisać wystarczająco informacji, by ten sam podgląd odtworzyć po zamknięciu gry i w archiwum. Nie zakładać, że samo ponowne użycie seeda będzie trwałym zapisem po zmianie wersji algorytmu; dobrać reprezentację z uwzględnieniem rozmiaru zapisów i kompatybilności.

### Pomiar i rekompensata zgodne z rzeczywistymi zasadami

**Sposób uśredniania wiatru z pomiarów ma odpowiadać rzeczywistemu systemowi w skokach**, według właściwych przepisów dla danego konkursu. Chodzi przede wszystkim o to, jak z pomiarów powstaje wyświetlana wartość wiatru używana do punktacji; samo mnożenie przez współczynnik rekompensaty jest osobnym krokiem. Rzeczywisty wpływ wiatru na skok oraz pomiar używany do punktów to oddzielne rzeczy: dwóch zawodników z podobnym pomiarem może odczuć inne warunki podczas lotu. Rekompensata nie ma idealnie odwracać obliczonego przez symulator wpływu na odległość.

- Dla pomiaru wiatru wzdłuż zeskoku stosować odpowiedni współczynnik skoczni: przy korzystnym wietrze pod narty punkty są odejmowane, przy tylnym dodawane; przy zerze nie ma punktów za wiatr. Uwzględnić właściwe współczynniki dla obu kierunków i zasady zaokrąglania. Przykład oficjalny: pomiar +0,39 m/s i współczynnik 9,26 pkt/(m/s) daje −3,6 pkt. [Zasady zawodów olimpijskich 2026](https://wmr-static-assets.scd.dgplatform.net/wmr/static/_PDF/OWG2026/SJP/2026-02-02/SJP-------------------------------__N02A_1.0.pdf).
- Uwzględnić także rzeczywiste zasady rekompensaty za belkę, w tym rozróżnienie decyzji jury i obniżenia przez trenera oraz próg 95% HS tam, gdzie wymagają tego przepisy. [Przepisy FIS, art. 422.1](https://assets.fis-ski.com/f/252177/x/c67426c343/icr-ski-jumping-2024_e_clean.pdf).
- Przed implementacją zweryfikować w aktualnych oficjalnych przepisach i dokumentacji technicznej dokładny układ punktów pomiarowych dla systemu rekompensaty, okna czasowe, ważenie, sposób uwzględniania odległości i zaokrąglanie. Nie wymyślać ich ani nie zakładać, że trzy ogólne stanowiska pomiaru warunków opisane w ICR są automatycznie całym układem używanym przez system wind/gate; ICR odsyła tu do osobnych zasad. [FIS ICR, art. 415.3](https://assets.fis-ski.com/f/252177/x/c67426c343/icr-ski-jumping-2024_e_clean.pdf).
- **Stan obecny Sj.Sim:** istnieje już wyliczanie średniego wiatru i rekompensaty z osobnymi współczynnikami skoczni dla wiatru przedniego i tylnego, a także rekompensata za belkę oraz logika progu 95% HS. Dostępne są różne sposoby uśredniania i uwzględniania odległości; sam fakt, że kod istnieje, nie potwierdza jeszcze pełnej zgodności z aktualną metodą FIS. Przed migracją porównać te mechanizmy z przepisami, zachować lub świadomie zmigrować ustawienia i stare zapisy oraz dodać przykłady i testy zgodności.

Wyświetlać czytelnie zmierzony wiatr, rekompensatę za wiatr, rekompensatę za belkę i sumę. Nie zakładać, że rzeczywisty wpływ wiatru na wynik musi być w każdym skoku w pełni wyrównany.

### Wydajność i strojenie

Lot można przetwarzać w małych krokach, np. co kilka metrów, albo użyć równoważnej szybkiej metody. W playgroundzie porównać różne wielkości kroku i czas dużych serii; mniejszy krok nie może po cichu zmieniać wyniku w niekontrolowany sposób. Nie deklarować realizmu współczynników bez kalibracji.

Playground powinien umożliwiać porównywanie tych samych skoków i seedów przy różnych warunkach oraz współczynnikach: odległości bez wiatru i z wiatrem, wpływu fragmentów lotu, zmierzonego wiatru, rekompensaty, punktów i miejsc. Sprawdzić korelację między skokami i sąsiednimi miejscami, podmuchy, kierunki bliskie bocznemu, bardzo krótkie i dalekie skoki, skrajne wartości oraz koszt obliczeń.

## Losowy wiatr na konkurs, konfiguracja wiatru
