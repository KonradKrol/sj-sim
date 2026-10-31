# Kolejka właściciela — 34 punkty, 31.10.2026

Kopia listy z pulpitu, aby była dostępna po pullu na innym komputerze.
Bieżące decyzje, wyniki testów i nowe zgłoszenia: `../31-10-start.md`.
Nie oznaczać całej listy jako ukończonej na podstawie częściowych testów.

# Poprawki w edytorze bazy danych
1. Do poprawy jest UI edytora bazy danych. Ustawienia paczki mają być widoczne na każdym ekranie, a UI ustawień paczki ma być jeszcze bardziej z hierarchią i bardziej jasne dla użytkownika. To samo tyczy się listy rozwijanej na ekranie edycji skoczkow i skoczni, niektóre opcje tam się duplikują z opcjami paczki, to jest niespójne wszystko. Ten znak zapytania też nie ma sensu w tamtym miejscu, koło listy rozwijanej w edytorze skoczków i skoczni.

2. Dodaj drag and drop dla elementów na liście skoczków

3. Przywróć przycisk sortowania skoczków krajem, skoczni także, oraz daj are you sure dialog.

4. Usuń parametr tailwind points z edytora skoczni, bo on jest liczony automatycznie. Tzn zablokuj edycję ale wyświetlaj edytowaną wartość.

# UX edytowania dat konkursów i kalendarzy itd.
1. Opcje za bardzo się "zlewają". Ma być dużo więcej hierarchii, trochę więcej kontrastu; usuń wszelki visual noise, który obciąża poznawczo użytkownika i wspomóż się także dopasowaniem odstępu elementów. Zapisz to gdzieś w ultra ważnych zasadach UI Sj.Sima: Hierarchia w UI, brak "szumu" i bałaganu wizualnego który obciąża kognitywnie użytkownika, kontrast, odpowiedni spacing jeśli wymaga tego sytuacja. Trzymaj się tych zasad BARDZO.

2. Np. "Dates of existing competitions" - ten label nie ma tutaj sensu, albo jest w takim miejscu, że tego sensu nie komunikuje. Potem są dwa teksty o tej samej  czcionce, nierozdzielone odstępem, które mówią coś innego ale user jest pogubiony. Potem dropdown jest za wysoki i niepotrzebnie zużywa dwie linijki. Itd, itp. - tyle jest tu nieprawidłowości.

3. Warningi są NIEJASNE (złota zasada UX writingu w Sj.Simie jest taka że ma być on JASNY i ZROZUMIAŁY bez wysiłku i jakiegoś dziwnego modelu mentalnego programisty który myśli że to jest zrozumiałe). zapisuj te zasady gdzies w wazne miejsce i w kazdej poprawce się ich trzymaj. I warningi są niewidoczne.
"Więcej opcji" nie wygląda jak przycisk za barzdo
Zmien "Zaplanuj terminy" i tym podobne na "Szybkie planowanie". "Ustal terminy" na "Ustal wiele terminów na raz" (lub skróć by było mniej słów ale sens ten sam)

4. Usuń "ważność treningów" z edytora konkursów. Obecnie symulacja z tego totalnie już nie korzysta.

# UX trybu sezonów
1. Panel na górze jest za wysoki nawet jeśli nie wykorzystuje tego miejsca. Ten panel z datą, nazwą save'a i przyciskami. Ten panel sam w sobie ma być ładniejszy i bardziej przydatny, prawdopodobnie dużo mniejszy i mniej rzucający się w oczy.
2. Przyciski tych akcji niech mają inne miejsca, niech będą dużo ładniejsze, mogą mieć ikonki.
3. Przycisk zapisania do pliku też ma być dużo inny, bo on jest ważny. Wykorzystaj zasadę kontrastu.
4. (poprawka również poza UX) Wprowadź auto-save i toggle w przeglądzie sezonu, czy zapisywać sezony. Np. po każdym konkursie albo po zmianach w kalendarzu i innych miejscach, po jakimś czasie np. 15 sekund — jeśli masz inny timing lub uważasz że od razu po każdej zmianie powinno być — śmiało.

# Listy zawodników
1. Listy zawodników są OCHYDNE, nie mają sensu. Przebuduj cały system list zawodników w prosty sposób.
2. Cel jest taki, że tworzę kilka list i BARDZO SWOBODNIE, prosto i bez żadnego tarcia, zmieniam sobie zawodników tam.
3. Daj prostą możliwość wprowadzania limitów dla krajów w danej liście skoczków, tak pomocniczo. Będzie kompaktowa tabelka lub lista krajów, tak zrobiona by łatwo było wpisywać i zmieniać, a tam limit skoczków. 
4. A propos list trochę, ten limit nie oznacza że w konkursie nie można wybrać więcej skoczków z danego kraju.
5. W ekranie Jumper lists oraz w Przeglądzie sezonu możesz łatwo i ładnie sobie wybierać aktywną listę skoczków
6. Przed konkursem wczytuje się aktywna lista skoczków, i tylko ci zawodnicy są na liście startowej. Jest opcja żeby łatwo dodać innych skoczków, tak jak jest obecnie po lewej stronie (po lewej wybieranie CZY będzie skakał, a po prawej lista startowa tak jak już jest)
7. W edytorze listy daj dwa proste widoki, po prostu widok zwykły, oraz widok posortowany drużynami i pogrupowany drużynami, który jeszcze bardziej ułatwi trzymanie się zdefiniowanych limitów
8. Listy zawodników mają też działać w drużynówkach.

# Ścieżka UX od pierwszego uruchomienia do symulacji
1. Ma być dialog modalny po pierwszym wejściu, zachęcający od razu do włączenia tej symulacji. Wytłumacz, że użtykownik przesymuluje sezon ze znanymi sobie skoczkami, skoczniami i kalendarzem zawodów najwyższej rangi.

# Wybieranie skoczków na konkurs - ciąg dalszy
1. Wprowadź jeszcze łatwiejsze wybieranie skoczków na konkurs: wymyśl jakieś 2-3 świetne rozwiązania UX.
	- Ma być w miarę kompaktowo, mają być flagi zawodników jeśli ich jeszcze nie ma.
	- Mają być różne fajne opcje ustawiania kolejności wg czegoś-tam.
	- Np. wg wyników innego konkursu, wg klasyfikacji, wg OVR nawet rosnąco

2. Póki co UI listy startowej przed konkursem jest... brzydkie. Te opcje schowane w menu rozwijanym po kliknięciu na przycisk z trzema kropkami — to jest nieintuicyjne i brzydkie. Tzn. może zostać to w takiej formie mniej więcej, ale wymyśl coś prostszego dla usera i ładniejszego — obecnie ucina nazwy opcji, opcje są nieuporządkowane co też trochę obciąża kognitywnie usera. W dodatku osobno jest zakładka kolejności według wyników co jest błędem.

3. UWAGA WAŻNE: Opcjonalnie powiąż listy zawodników z kalendarzami. Tzn. kalendarzowi przypisujesz listę zawodników. Wtedy automatycznie lista staje się aktywna gdy jest konkurs z danego kalendarza. Ma być też proste UX, ale to ma być widoczne dla usera żeby znał taką funkcję, ma być jasna i krótko wytłumaczona bez zbędnego gadania.

4. Wprowadź automatyczne kompletowanie drużyn przed konkursem. Domyślnie też to rób po wejściu w konfigurację konkursu. Zasada jest taka, że od najsłabszego skoczka do najlepszego. Licz to jakąś ważoną sumą formy i skilla, dobierz takie wagi jak są w silniku symulacji.
Jednakże, dodaj trochę losowości do wyborów, a konkretniej do ratingu obliczanego dla skoczków na bazie formy i skilla, żeby nie było to takie liniowe i żeby w 100% nie dało się wywnioskować że wg gry zawodnik A jest w lepszej formie niż zawodnik B.
Dla konkursów 4-osobowych na początku niech skacze trzeci najlepszy, potem czwarty, potem drugi, potem najlepszy. Dla 3-osobowych na początku niech skacze drugi najlepszy, potem trzeci najlepszy i najlepszy. Dla 5-osobowych najpierw niech skacze trzeci najlepszy, potem piąty, potem czwarty, potem drugi, potem pierwszy. Wywnioskuj ogólną zasadę doboru.
A dla duetów dalej najlepszy na końcu

# Inne (zrób na końcu)
1. Upewnij się że archiwum działa. Wydaje mi się że tak.
2. Zaproponuj 2 ustawienia symulacji które naprawdę mają sens - jeśli to nie wymaga moich decyzji i jest to proste — zaimplementuj; usuń ustawienie reduce save size
3. Uwaga: listy do edytowania skoczków i skoczni w sezonie mają być takie same jak w edytorze — to ma bazować na tych samych komponentach, ale wywalasz wszystko co jest niezwiązane z bazą danych i nie ma sensu w kontekście trybu symulacji sezonów.
4. Po prostu "Competition rules" bez presets w nawiasie.
5. Znajdź 5 miejsc gdzie użytkownika może trochę denerwoawć długie czekanie na coś i zmniejsz ten czas przynajmniej o 50%. Jeśli aż tyle nie znajdziesz, to jest OK.
6. "Przejdź do menu" pokazywane w samym menu nie ma sensu, ale i tak to chyba juz przeniosłeś do dialogu modalnego.

# Pomoc
1. Usuń "Pomoc" otwieraną w menu głównym.

# Konkursy
1. Usuń zakładkę "Opcje" przed konkursem. Daj opcje stamtąd do "Przygotowanie"
2. Wszędzie gdzie ustawia się DSQ w aplikacji, ma być do ogarnięcia w sekundę że chodzi o prawdopodobieństwo typu 1 na N, bo obecnie to jakiś magic number.
