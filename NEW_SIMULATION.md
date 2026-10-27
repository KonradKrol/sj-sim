Czas zmigrować na nowy system symulacji i formy. Utwórz nowy branch od aktualnego. Nazwij go `feature/simulation-v2`.

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

# Nowy silnik formy

# Nowy system wiatru

## Ciągły model wiatru i lotu

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

**Rekompensaty mają działać jak w prawdziwych skokach**, według właściwych przepisów dla danego konkursu, a nie idealnie odwracać obliczony przez symulator wpływ wiatru na odległość. Rzeczywisty wpływ wiatru na skok oraz pomiar używany do punktów to oddzielne rzeczy: dwóch zawodników z podobnym pomiarem może odczuć inne warunki podczas lotu.

- Dla pomiaru wiatru wzdłuż zeskoku stosować odpowiedni współczynnik skoczni: przy korzystnym wietrze pod narty punkty są odejmowane, przy tylnym dodawane; przy zerze nie ma punktów za wiatr. Uwzględnić właściwe współczynniki dla obu kierunków i zasady zaokrąglania. Przykład oficjalny: pomiar +0,39 m/s i współczynnik 9,26 pkt/(m/s) daje −3,6 pkt. [Zasady zawodów olimpijskich 2026](https://wmr-static-assets.scd.dgplatform.net/wmr/static/_PDF/OWG2026/SJP/2026-02-02/SJP-------------------------------__N02A_1.0.pdf).
- Uwzględnić także rzeczywiste zasady rekompensaty za belkę, w tym rozróżnienie decyzji jury i obniżenia przez trenera oraz próg 95% HS tam, gdzie wymagają tego przepisy. [Przepisy FIS, art. 422.1](https://assets.fis-ski.com/f/252177/x/c67426c343/icr-ski-jumping-2024_e_clean.pdf).
- Przed implementacją zweryfikować w aktualnych oficjalnych przepisach i dokumentacji technicznej dokładny układ punktów pomiarowych dla systemu rekompensaty, okna czasowe, ważenie, sposób uwzględniania odległości i zaokrąglanie. Nie wymyślać ich ani nie zakładać, że trzy ogólne stanowiska pomiaru warunków opisane w ICR są automatycznie całym układem używanym przez system wind/gate; ICR odsyła tu do osobnych zasad. [FIS ICR, art. 415.3](https://assets.fis-ski.com/f/252177/x/c67426c343/icr-ski-jumping-2024_e_clean.pdf).
- **Stan obecny Sj.Sim:** istnieje już wyliczanie średniego wiatru i rekompensaty z osobnymi współczynnikami skoczni dla wiatru przedniego i tylnego, a także rekompensata za belkę oraz logika progu 95% HS. Dostępne są różne sposoby uśredniania i uwzględniania odległości; sam fakt, że kod istnieje, nie potwierdza jeszcze pełnej zgodności z aktualną metodą FIS. Przed migracją porównać te mechanizmy z przepisami, zachować lub świadomie zmigrować ustawienia i stare zapisy oraz dodać przykłady i testy zgodności.

Wyświetlać czytelnie zmierzony wiatr, rekompensatę za wiatr, rekompensatę za belkę i sumę. Nie zakładać, że rzeczywisty wpływ wiatru na wynik musi być w każdym skoku w pełni wyrównany.

### Wydajność i strojenie

Lot można przetwarzać w małych krokach, np. co kilka metrów, albo użyć równoważnej szybkiej metody. W playgroundzie porównać różne wielkości kroku i czas dużych serii; mniejszy krok nie może po cichu zmieniać wyniku w niekontrolowany sposób. Nie deklarować realizmu współczynników bez kalibracji.

Playground powinien umożliwiać porównywanie tych samych skoków i seedów przy różnych warunkach oraz współczynnikach: odległości bez wiatru i z wiatrem, wpływu fragmentów lotu, zmierzonego wiatru, rekompensaty, punktów i miejsc. Sprawdzić korelację między skokami i sąsiednimi miejscami, podmuchy, kierunki bliskie bocznemu, bardzo krótkie i dalekie skoki, skrajne wartości oraz koszt obliczeń.

## Losowy wiatr na konkurs, konfiguracja wiatru
