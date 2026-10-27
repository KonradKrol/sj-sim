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

- Wiatr ma **pamięć między skokami**: następny zawodnik zwykle startuje przy warunkach podobnych do poprzedniego, a siła i kierunek stopniowo się zmieniają. Czas między skokami i przerwy mogą wpływać na wielkość zmiany.
- Sąsiednie miejsca są powiązane. Istnieje ogólny wiatr na skoczni oraz lokalne odchylenia, np. bardziej osłonięty albo wystawiony fragment. Nie losujemy niezależnie zupełnie nowego wiatru dla każdego metra.
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

### Pomiar i rekompensata

Rzeczywisty wpływ wiatru na skok oraz pomiar używany do punktów to **oddzielne rzeczy**. System punktowy może korzystać z uproszczonego pomiaru w kilku miejscach i czasie, a następnie stosować współczynniki skoczni oraz zasady konkursu. Nie powinien znać idealnie rzeczywistego efektu wiatru ani automatycznie go zerować: dwóch zawodników z podobnym pomiarem może odczuć inne warunki podczas lotu.

Zasady uwzględniania punktów pomiaru i odległości trzeba określić oraz sprawdzić osobno od wzoru lotu. Zachować czytelność wyświetlanego wiatru i rekompensaty; nie zakładać bez pomiarów, że tylny lub przedni wiatr musi być zawsze w pełni zrekompensowany.

### Wydajność i strojenie

Lot można przetwarzać w małych krokach, np. co kilka metrów, albo użyć równoważnej szybkiej metody. W playgroundzie porównać różne wielkości kroku i czas dużych serii; mniejszy krok nie może po cichu zmieniać wyniku w niekontrolowany sposób. Nie deklarować realizmu współczynników bez kalibracji.

Playground powinien umożliwiać porównywanie tych samych skoków i seedów przy różnych warunkach oraz współczynnikach: odległości bez wiatru i z wiatrem, wpływu fragmentów lotu, zmierzonego wiatru, rekompensaty, punktów i miejsc. Sprawdzić korelację między skokami i sąsiednimi miejscami, podmuchy, kierunki bliskie bocznemu, bardzo krótkie i dalekie skoki, skrajne wartości oraz koszt obliczeń.

## Losowy wiatr na konkurs, konfiguracja wiatru
