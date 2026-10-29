# Laboratorium balansu

Wejście: **menu główne → Laboratorium balansu** lub **Ustawienia → Symulacja →
Laboratorium balansu**. Dotychczasowy playground pozostaje obok.

To warsztat do strojenia silnika, a nie gotowy zestaw dobrze wyważonych ustawień.
Wykorzystuje ten sam silnik i te same reguły awansu co gra. Eksperymenty kopiują
dane; nie nadpisują paczki skoczków, skoczni ani zasad.

## Pierwsza próba

1. Wybierz pytanie, np. **OVR i forma**, **Lądowanie** albo **Pole wiatru**.
2. Po lewej w **Próba** wybierz własną paczkę, skocznię i badanego skoczka.
   Uwagi do importu są zwijane. Możesz też pracować na stawce syntetycznej.
3. W **Skoczkowie** zmieniaj dane badanego profilu. W **Silnik** zmieniaj reguły
   badanego mechanizmu. Podgląd przeliczy się w tle po krótkiej przerwie.
4. Kliknij **Zamroź odniesienie**, żeby zachować obecny wariant do porównania.
5. Po wstępnej ocenie wybierz **Pełna próba**. Liczbę konkursów ustawisz w **Próba**.

Na wąskim ekranie panel po lewej otwiera przycisk **Ustawienia / Opcje**.
Przycisk **Pytania** wraca do wyboru obszaru, zachowując dane eksperymentu.
**Cofnij** cofa zmianę konfiguracji; **Reset** przy parametrze przywraca wartość
wariantu ustawień gry.

Domyślnie panel **Silnik** pokazuje parametry danego obszaru. Przełącznik
**Pełny rejestr parametrów** otwiera wszystkie 607 ustawień wraz z wyszukiwarką.
Pozwala dotrzeć również do rzadziej zmienianych danych technicznych próby.

## Co porównują trzy serie

- **Ustawienia gry**: wariant odniesienia z parametrami aktywnego balansu.
- **Punkt odniesienia**: kopia badanego wariantu z chwili zamrożenia.
- **Bieżące**: wariant edytowany w warsztacie.

Zmiany paczki, skoczni, formatu i seeda są wspólne dla porównań. Edycja danych
skoczka lub grupy dotyczy bieżącego wariantu; ustawienia silnika dotyczą w nim
całej stawki. Zamrożenie zapisuje również indywidualne poprawki profili.
Warianty używają wspólnych seedów. Odtworzenie dokładnych liczb wymaga tej samej
kompilacji silnika.

Domyślne pasmo pokazuje środkowe 80% wyników, czyli rozrzut, a nie dokładność
średniej. Osobno można wybrać przedział 95% średniej. Dystanse i punkty pomijają
skoki DSQ; średnie miejsca pomijają nieklasyfikowane starty. Dodatnia różnica
miejsca oznacza pogorszenie. Kwantyle odległości pochodzą z koszyków histogramu.

Podgląd ma małą próbę, więc służy ocenie kierunku zmian. Pełne próby zachowują
raport i historię. Mapa ogranicza koszt do budżetu planowanych skoków; faktyczna
liczba konkursów na komórkę jest podana przy wyniku i może być mniejsza od
głównej próby. Stary wynik podczas przeliczania ma wyraźny opis nieaktualności.

## OVR, forma, perki i konkursy

W **Skoczkowie** możesz zaznaczyć jednego skoczka, wybraną grupę lub wszystkich.
Można ustawić im jedną wartość albo dodać zmianę do dotychczasowych wartości.
Przy dojściu do granic zakresu komunikat informuje o ograniczeniu wartości.
Przycisk **Tylko zaznaczeni w stawce** tworzy podzbiór do kontrolowanej próby.

W **Osie** wybierz gotowe **OVR × forma** lub **Belka × wiatr**, albo dowolne dwa
liczbowe parametry. **Policz mapę** tworzy siatkę; kliknięcie komórki ustawia
obie wartości w bieżącym wariancie. Wykres jednowymiarowy działa w obszarach
krzywych; inne obszary zachowują swoje wykresy diagnostyczne.

Format można przełączyć na indywidualny, KO lub drużynowy. W drużynach ustaw
2–4 skoczków. **Drużyny według krajów** używają reprezentacji z paczki; kraje bez
pełnego składu są pomijane. Można też przypisywać zaznaczonych skoczków do
własnych drużyn testowych. Skład zawężaj wyborem badanej stawki. Miejsce i suma
w raporcie drużynowym opisują wynik drużyny danego skoczka.

Przykład proporcji OVR–forma: wybierz własną paczkę, zamroź odniesienie,
w **Silnik** zmieniaj **Udział OVR** i współczynniki zależności formy od K.
Porównuj metry oraz miejsca i podium. Powtórz próbę na mniejszym i większym K.
Nie wyciągaj wniosków o miejscach wyłącznie z izolowanej średniej odległości.

## Lądowanie

W **Lądowanie → Silnik** pokazane są parametry lądowania oraz związanej z nim
geometrii i bocznego wiatru. Nie trzeba przeglądać wszystkich reguł silnika.

Pierwszy wykres to dokładny model przy zadanej odległości i bocznym wietrze:
telemark, dwie nogi, podpórka lub upadek. Drugi pokazuje upadki w pełnych lotach,
pogrupowanych co 2 m. Na szerokim ekranie są obok siebie; na małym pod sobą.
Rzadko osiągane odległości mają mało obserwacji. Poniżej jest średnia nota.
Klasa lądowania wpływa na noty, nie na prawdopodobieństwo upadku.

Zmieniaj np. **real HS** i krzywą ryzyka, obserwując przesunięcie dokładnego
wykresu. Pełna próba pokazuje, jak często skoczkowie rzeczywiście docierają
w niebezpieczną część zeskoku.

## Wiatr i losowanie pogody

To trzy odrębne pytania:

1. **Losowanie pogody** bada automatyczną propozycję i 11 presetów. Pokazuje
   rozkłady siły, zakresy i udział wiatru pod narty oraz w plecy. W **Silnik**
   wybierasz generator do edycji: jego zakresy, wagi i współczynniki.
2. **Pole wiatru** pokazuje przebieg w czasie, przekrój nad zeskokiem i mapę
   czas–odległość. Suwak wybiera moment przekroju. Rozwijany panel ustawia zakres
   czasu, odległość, gęstość próbkowania i skalę kolorów.
3. **Wykorzystanie wiatru** pokazuje wiatr odczuty, pomiar do rekompensaty,
   zysk/stratę metrów względem ciszy i końcową różnicę punktów. Efekty w metrach
   i punktach porównują trzy warianty. Pole i pomiar można przełączać osobno.

W **Próba** wybierz źródło pogody. **Ręczne pole** używa parametrów `wind.*`
i `precipitation.*`; tryby losowania używają współczynników `generator.*`.
Losowanie w konkursach uwzględnia K, perki skoczni, miesiąc, nawierzchnię i kraj
źródłowej skoczni. Zmiana samego pomiaru nie zmienia fizycznego wiatru.

Na początek użyj ręcznego pola, wspólnego zegara konkursu i niezerowej amplitudy
czasowej/podmuchów. Wtedy widać zmienność. Przy kontrolowanym zegarze każdy start
zaczyna się w tym samym momencie, choć podczas lotu pole nadal jest ciągłe.
Szybki podgląd ogranicza gęstość analizy; pełna próba respektuje wybrane wartości.

W **Odtwarzanie** wybierz konkurs z pełnej próby, wariant i konkretny skok.
Suwak lub **Odtwarzaj odcinki** pokazuje zapisane odcinki ekspozycji na wiatr.
To rzeczywiście napotkane warunki, bez dorabiania fizycznego toru lotu.
Raport z innej kompilacji zachowuje stare agregaty; nowe odtworzenie jest oznaczone.

## Cele, profile i zapis warsztatu

W **Cele** dodaj oczekiwany zakres, miarę i wariant. Możesz sprawdzać różnice
metrów/punktów/miejsc, ich średnie oraz dokładne ryzyko upadku na danej odległości.
**Zamroź dane tej próby jako scenariusz** zapisuje paczkę, skocznię, warunki i dane
profili. Późniejsze kontrole zachowują te dane, a używają aktualnie strojonych
współczynników silnika. Bez zamrożenia cel bada bieżący eksperyment.

Wynik celu jest spełniony, niespełniony lub nierozstrzygnięty. Dla średnich
potrzeba co najmniej 30 powtórzeń i oceny całego przedziału 95%. Dokładna szansa
upadku nie wymaga Monte Carlo. Raport pokazuje liczby i poprzedni stan zgodnego
celu. Automatyczne szukanie współczynników pozostaje na później.

**Profile** pozwalają zapisać warsztat pod nazwą, odczytać historię, przejrzeć
różnice i eksportować raport/CSV. Ostatni warsztat przywraca się automatycznie,
wraz z pytaniem, danymi, odniesieniem i celami.

**Zastosuj bieżący balans w grze** jest osobną operacją. Do gry przechodzą
współczynniki silnika, pomiaru i generatora, bez testowych zmian danych skoczków,
skoczni i formatu. Obowiązują od następnego konkursu lub sesji skoków, również
w istniejącym sezonie. Rozpoczęty konkurs zachowuje swoją kopię, także po zapisie.
**Standardowy balans** przywraca domyślne reguły dla następnych rozgrywek.

## CLI

Ten sam eksperyment developerski można zapisać w oknie **Profile** i uruchomić
z terminala. Nie jest to publiczny interfejs modowania gry.

```powershell
.\sj-sim.exe --balance-lab-example eksperyment.json
.\sj-sim.exe --balance-lab --config eksperyment.json --output raport --preview
.\sj-sim.exe --balance-lab --config eksperyment.json --output mapa --matrix
.\sj-sim.exe --balance-lab --config eksperyment.json --analysis 2 --output wiatr.json
.\sj-sim.exe --balance-lab --config eksperyment.json --replay 2:0 --output skoki.json
```

Bez `--preview` wykonywana jest pełna próba. Warianty i konkursy w CLI numerowane
są od zera. Folder wynikowy musi być nowy. `balance-analysis.json` zawiera wyniki
laboratorium, a `report.json`, `athletes.csv`, `focus-samples.csv` i
`comparisons.csv` pełny raport silnika. `--threads N` ustawia liczbę pracowników.
Przerwanie pełnej próby w UI nie zastępuje poprzedniego kompletnego raportu.

Test CLI: `python scripts/test-balance-lab-cli.py build-win/release/sj-sim.exe`.
Testy natywne mają przełączniki `--balance-lab-core-only` i `--balance-lab-only`.
