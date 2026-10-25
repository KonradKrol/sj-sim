# Playground symulacji

Dostęp: **Ustawienia → Silnik symulacji → Playground symulacji**.

Tryb służy do eksperymentów z aktualnym silnikiem Sj.Sim: seryjnego rozgrywania konkursów, porównywania parametrów i oglądania rozkładów wyników. Nie zmienia zapisów sezonów, zawodników, rekordów ani ustawień normalnej gry. Parametry eksperymentalne działają tylko w lokalnym zakresie obliczeń i są oddzielone między wątkami.

## Szybki początek

1. Wybierz stawkę syntetyczną lub paczkę zawodników, skocznię i zasady.
2. Wybierz skoczka, którego parametry będą zmieniane przez grupę „Wybrany skoczek”.
3. Na początek ustaw 1000 konkursów na wariant. Jeden wariant odniesienia i sześć zmian oznacza 7000 konkursów.
4. Wybierz gotowe porównanie, np. **Przewaga wiatru**, albo dodaj własne warianty. Pierwszy wariant jest bazą.
5. Kliknij **Uruchom**. Interfejs pozostaje dostępny, pokazuje postęp i pozwala przerwać obliczenia.
6. Otwórz **Porównanie**, **Skoczek** albo **Konkurs**. Zakończony wynik trafia do **Historii** i można go wyeksportować.

Najpierw zmieniaj jeden czynnik. Potwierdź interesującą zależność większą liczbą konkursów i innym ziarnem, zamiast uznawać pojedynczą korzystną próbę za dowód.

## Dane i warianty

- **Stawka syntetyczna:** określasz liczbę i rozpiętość umiejętności. To pozwala odseparować wpływ konkretnego parametru od przypadkowej struktury prawdziwej bazy.
- **Paczka zawodników:** kopia danych, łącznie z cechami. Możesz zmienić parametry wybranego zawodnika i formę, lądowanie, równość lub styl pozostałych. Poziom i rozpiętość syntetycznej stawki nie służą do przekształcania takiej paczki.
- **Skocznia:** wariant syntetyczny lub kopia skoczni z bazy. Zachowywane są jej cechy i ręczna punktacja; pola automatyczne są przeliczane według K. K, HS, balans, mnożnik odległości i rekompensaty mogą być przedmiotem eksperymentu.
- **Zasady:** serie i limity w prostym scenariuszu albo preset gry, również wagi, reset punktów, KO i konkursy drużynowe. Ręczny dobór grup KO zastępuje się powtarzalnym losowaniem, ponieważ batch nie może czekać na interakcję.
- **Wybrany skoczek:** osobne parametry mają pierwszeństwo przed parametrami reszty stawki. Zmiana formy pozostałych pozwala mierzyć efekt pośredni na jego miejsce.

Grupa **Współczynniki eksperymentalne** udostępnia mnożniki wkładu wybicia i lotu w metry oraz fizycznego wpływu wiatru. Wartość 1 zachowuje zwykły model. Dostępne są także bonusy oceny wybicia, lotu i odległości przez istniejący mechanizm manipulacji skokiem. Nie są zapisywane do ustawień normalnej gry.

Wspólne parametry stanowią podstawę wariantów. Pozostałe warianty nadpisują wybrane wartości. Skanowanie tworzy serię wariantów między wskazanymi wartościami; zastępuje bieżącą listę. Liczby całkowite są zaokrąglane, a powtarzające się wartości skanu pomijane. Konfigurację można zapisać i później wczytać.

## Co widać w wynikach

**Porównanie:** średnie odległości i noty, upadki, DSQ, rozkłady, różnice wobec bazy, zależności wiatru z odległością i punktami oraz liczbowe obserwacje.

**Skoczek:** wybierz dowolnego zawodnika po obliczeniach. Bez ponownego uruchamiania dostajesz jego histogram miejsc, odległości i not, średnie, zmienność, zwycięstwa, podium oraz sparowane porównania wszystkich wariantów. Można zatem zobaczyć także wpływ zmiany kogoś innego na tego zawodnika.

**Konkurs:** odtwórz dowolny numer, również poza zachowaną próbką. Dostajesz pełne wyniki i wszystkie skoki. Wybór zawodnika pokazuje serie, odległość, punkty, wiatr, rekompensatę, noty, upadek/DSQ i wewnętrzne oceny wybicia, lotu oraz losową część skoku. Odtwarzanie nie rozgrywa konkursu w sezonie.

**Historia:** przechowuje raporty i kompletne konfiguracje. „Użyj konfiguracji raportu” pozwala przygotować kolejne doświadczenie na tej samej podstawie. Eksport zapisuje dane w wybranym folderze; wcześniejsze raporty nie są nadpisywane.

Przykłady konkursów obejmują pierwsze skonfigurowane numery, nie losową próbkę. Ich skróty miejsc dotyczą skoczka wskazanego przy uruchamianiu. Po wybraniu innego zawodnika lista prowadzi do odtworzenia całego konkursu. Pełne statystyki obejmują wszystkie ukończone konkursy, niezależnie od wielkości próbki.

## Warunki i belka

- Dodatni wiatr oznacza wiatr pod narty; ujemny — w plecy.
- „Średni wiatr” dotyczy całej stawki. „Różnica wiatru dla skoczka” pozwala badać przewagę warunków jednego zawodnika względem reszty.
- Zmienność między skokami i zmienność w pięciu sektorach są oddzielne. Pozwala to badać także sposoby uśredniania i rekompensaty.
- **Belka 0 w konfiguracji** oznacza jedną belkę dobraną z wariantu bazowego. Pozostałe warianty z wartością 0 korzystają z tej samej liczby. Faktyczna belka jest pokazana w wynikach i odtworzeniu.
- To celowo kontrolowane porównanie przy wspólnej belce. Zmiana wiatru na mocny wiatr pod narty może zwiększyć odległości i upadki. Jeśli chcesz porównać scenariusze z reakcją na warunki, ustaw osobne, konkretne belki w wariantach.
- Parametr DSQ nie jest procentem: 0 wyłącza DSQ; bez dodatkowych cech 100 oznacza około 1%, a 200 około 0,5%. Zasady muszą również dopuszczać dyskwalifikacje.

## Interpretacja statystyk

Różnice sparowane to **wariant minus baza**, dla tych samych numerów konkursów. Ujemna różnica miejsca oznacza poprawę. Program liczy takie porównania dla każdego zawodnika, także gdy zmiana parametrów dotyczy innej osoby.

Przedziały 95% są przybliżeniem opartym na niezależnych symulowanych konkursach. Dla średnich ze skoków uwzględnia się grupowanie skoków w konkursy; nie udaje się, że kolejne serie i zawodnicy to niezależne powtórzenia całego eksperymentu. P5, mediana i P95 odległości/not są przybliżane środkami koszyków histogramu; szerokość koszyka jest w JSON.

- Średnie odległości i not skoku wykluczają skoki DSQ. Liczby wszystkich skoków i DSQ są oddzielne.
- Udział upadków ma w mianowniku wszystkie oddane skoki, także DSQ. To definicja wskaźnika, a nie szacowanie szansy upadku warunkowej na brak DSQ.
- Średnie miejsc pomijają wyniki nieklasyfikowane. W histogramie pozostaje osobny koszyk N/K; należy go brać pod uwagę przy ocenie wariantu.
- Suma punktów może obejmować różną liczbę serii i wagi. Nie jest tym samym co przeciętna jakość jednego skoku.
- W drużynówkach miejsca, zwycięstwa, podium i sumy odnoszą się do drużyny zawodnika, a rozkłady skoków do niego samego. Udział w konkursie drużyny jest liczony także wtedy, gdy wcześniejsze DSQ uniemożliwiło oddanie jego skoku. Rezerwa nie otrzymuje startu. DSQ drużyny jest oddzielone od DSQ własnych skoków.
- Korelacja opisuje próbę, nie stanowi dowodu przyczynowości. Efekt wiatru oddzielaj od rekompensaty, umiejętności i doboru belki przez kontrolowane warianty.
- Oglądanie wielu parametrów i zawodników oznacza wiele porównań. Przedziały nie mają korekty wielokrotnych testów; traktuj analizę jako eksploracyjną i potwierdzaj wyniki nową próbą.
- Wyniki pokazują zachowanie obecnego modelu. Sam playground nie dowodzi realizmu względem rzeczywistych zawodów i nie wybiera arbitralnie „najlepszego” balansu.

## Powtarzalność, pamięć i przerywanie

Ziarna są rozdzielone według konkursu, serii i zawodnika. Losowanie wiatru jest oddzielone od losowania samego skoku. Liczba wątków nie zmienia histogramów, przykładów ani rezultatów zawodnika. Agregowanie liczb zmiennoprzecinkowych może dać nieistotną różnicę ostatnich cyfr średniej. Wyniki remisów otrzymują w laboratorium identyfikatory wyprowadzone z ziarna; usuwa to przypadkową kolejność z losowych UUID, bez zmiany zasad zwykłej gry.

Dokładne odtworzenie wymaga tej samej wersji/kompilacji silnika i biblioteki standardowej. Raport zawiera identyfikator kompilacji. Jeśli odtworzenie historycznego raportu używa innej kompilacji, UI wyraźnie o tym informuje. Oryginalne statystyki i próbki w raporcie pozostają zachowane.

Limity wejścia: do **1 000 000 konkursów na wariant**, **32 wariantów**, **512 zawodników**, **8 serii**, **64 żądanych wątków** i **4096 przykładów na wariant**. Liczba realnych wątków może być niższa ze względu na wielkość zadania i szacunek pamięci. Domyślnie używa się do 16 wątków, pozostawiając jeden logiczny wątek procesora poza pulą.

Nie przechowuje się milionów pełnych obiektów skoków. Agregaty i ograniczona próbka mają kontrolowany rozmiar; dowolny konkurs można ponownie odtworzyć. Większa liczba wariantów i wielkość stawki zwiększają koszt agregatów; aplikacja ogranicza pulę według oszacowania pamięci roboczej.

„Przerwij” zachowuje tylko konkursy ukończone we wszystkich wariantach. Normalne wyjście z pracującego playgroundu najpierw przerywa obliczenia i zapisuje ukończoną część. CLI obsługuje Ctrl+C i SIGTERM z zapisaniem częściowego raportu. Wymuszone zabicie procesu, awaria systemu lub odcięcie zasilania nie są mechanizmem zapisu postępu. Nie ma automatycznego wznawiania częściowego batcha; do kolejnego niezależnego batcha użyj nowego ziarna.

## CLI — także do użycia przez agenta

CLI działa bez uruchamiania okna i korzysta z tej samej implementacji co UI. Przykłady zakładają, że `sj-sim` jest dostępne w PATH; można podać pełną ścieżkę do zbudowanego programu. Windows używa `sj-sim.exe`.

```bash
sj-sim --playground-example experiment.json
sj-sim --playground --config experiment.json --output results/run-01 --threads 8
sj-sim --playground --config experiment.json --replay 1:777 --output replay.json
```

W lokalnej kompilacji linuksowej tego repo można użyć `scripts/run-quick-ui.sh` zamiast `sj-sim`; skrypt ustawia ścieżki do bibliotek i przekazuje argumenty.

- Indeksy wariantu, konkursu i zawodnika w JSON/CLI są liczone od zera. Serie zaczynają się od 1. UI pokazuje numery konkursów od 1.
- `--playground-example` tworzy jawny, syntetyczny scenariusz; nie odczytuje ustawień Twojej gry. Do analizy bieżącej konfiguracji zapisz ją z UI.
- `--output` batcha wskazuje nowy folder raportu. Przy odtwarzaniu wskazuje nowy plik JSON; bez niego wynik idzie na stdout.
- Wynik na stdout zawiera JSON z liczbą ukończonych porównań, czasem i użytymi wątkami. Postęp uruchomienia jest na stderr.
- Kod zakończenia: 0 — sukces, 2 — błąd wejścia/wykonania, 130 — przerwanie z zachowanym raportem.
- W konfiguracji `parameters` zawiera wartości wspólne, a `variants[].changes` nadpisania. `roster`, `hill.source` i `rulesData` pozwalają przenosić kopie danych gry; eksport UI przygotowuje poprawną strukturę i identyfikatory.

Raport zawiera:

| Plik | Zawartość |
| --- | --- |
| `experiment.json` | Pełne wejście eksperymentu |
| `report.json` | Rozkłady, statystyki każdego zawodnika, sparowane porównania, próbki, metadane |
| `athletes.csv` | Wybrane agregaty wszystkich zawodników we wszystkich wariantach |
| `focus-samples.csv` | Przykładowe skoki zawodnika wskazanego przy uruchamianiu |
| `comparisons.csv` | Sparowane efekty dla skoczka wskazanego przy uruchamianiu |
| `README.txt` | Opis formatów i interpretacji |

CSV jest zapisane w UTF-8, z przecinkiem jako separatorem pól, kropką dziesiętną i cytowaniem tekstów. Pełne dane, w tym porównania dowolnego zawodnika, są w `report.json`.

Analiza bez dodatkowych bibliotek Pythona:

```bash
python3 scripts/analyze-playground.py results/run-01
python3 scripts/analyze-playground.py results/run-01 --focus 12 --json
python3 scripts/analyze-playground.py results/run-01 results/run-02 --output comparison.md
```

Skrypt sprawdza sumy histogramów i liczników, przygotowuje tabelę oraz opisuje kierunek zmian i przedziały. Porównanie osobnych raportów jest opisowe; nie dorabia sparowanych przedziałów z samych agregatów.

Gotowe konfiguracje są w [examples/playground](../examples/playground): wiatr całej stawki, przewaga warunków jednego skoczka, forma, losowość oraz zamiana wybicia na lot przy tej samej sumie atrybutów. Zapisz wyniki osobno; wcześniejszych raportów nie trzeba usuwać.

## Weryfikacja

```bash
scripts/test-quick-ui.sh --playground-only
python3 scripts/test-playground-cli.py --exe build-quick-ui/sj-sim --competitions 1000 --keep
```

Testy obejmują zwykłe konkursy, KO i drużyny; pełne remisy i kolejność startu; neutralne/zmienione współczynniki; niezależność wątków; identyczne warianty; efekty parametrów; wczytanie źródłowych danych; błędne konfiguracje; eksport; anulowanie oraz wejście z ustawień, rzeczywiste klikanie UI i responsywne renderowanie.

Test CLI porównuje jeden i osiem wątków, sprawdza dokładne odtworzenie w osobnych procesach, CSV/JSON, skrypt analizy i — na POSIX — przerwanie sygnałem. Automatyczny test sygnału na Windows jest pomijany; anulowanie przez UI ma osobny test.
