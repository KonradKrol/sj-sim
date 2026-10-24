# Audyt silnika: 10 błędów i równoległe partie skoków

Prace na `feature/qml-competition-config`, bez zmian main i bez subagentów. Baza tej partii: `6c7ebdc`.

## Dowody przed poprawkami

Każdy przypadek ma niezależne uruchomienie `--engine-audit-only --engine-case=N`. Przypadki 1–9 zakończyły się przed poprawką kodem 1 i oczekiwanym komunikatem FAIL. Przypadek 10 przekroczył limit czterech sekund, ponieważ losowanie nie kończyło pętli. Logi: `/tmp/sj-engine-before-1.log` do `/tmp/sj-engine-before-10.log`. Po zmianach wszystkie dziesięć przeszło, log `/tmp/sj-engine-fixed.log`.

Testy dodano przed poprawkami w `f1b9c33`; są w `tests/engine-audit-regressions.h`. Nie wliczono zmian wyglądu ani nawigacji do dziesięciu błędów silnika.

## Dziesięć osobnych poprawek

| Nr | Problem i przykład | Poprawka | Commit |
| --- | --- | --- | --- |
| 1 | `JumpData` pozostawiał niezainicjalizowane wyniki i wskaźniki. Reset nie usuwał DNS, belki trenera, ochrony 95% HS ani formy poprzedniego zawodnika. Ponowne użycie obiektu mogło przenieść stan poprzedniej próby. | Konstruktor inicjalizuje komplet danych; reset czyści flagi, parametry oraz powiązania. Konstruktor zachowuje przekazane wskaźniki skoczka i skoczni. | `7cfef95` |
| 2 | `JumpSimulator` miał niezainicjalizowane parametry startowe. Odpięcie skoczka pozostawiało wskaźnik jego umiejętności. Symulacja niekompletnego kontekstu dereferencjonowała puste wskaźniki, a brak opcjonalnego manipulatora także powodował awarię. | Zdefiniowane wartości domyślne, czyszczenie skills, kontrola skoczka/reguł/geometrii i czytelne `invalid_argument` dla błędnego kontekstu. Brak manipulatora korzysta z tymczasowej instancji, bez pozostawiania wiszącego wskaźnika. | `87b5792` |
| 3 | Cecha `dsq-probability = -7` zerowała mianownik. Obliczenie powodowało dzielenie przez zero i nieprawidłową konwersję na `int`; jeszcze mniejsze wartości mogły tworzyć ujemny zakres losowania. | Walidacja bazy i mnożnika przed dzieleniem; zerowe/ujemne prawdopodobieństwo nie losuje DSQ, a nieskończone wartości nie są konwertowane na liczbę całkowitą. Test sprawdza również flagi wyjątków zmiennoprzecinkowych. | `9d6c768` |
| 4 | Kara wysokości lotu liczyła `abs(ideał - wysokość × 1,6 × skala)`. Nawet idealna wysokość mogła obniżać ocenę lotu. | Kara jest proporcjonalna do `abs(ideał - wysokość) × 1,6 × skala`. Test zerowej różnicy oczekuje zerowej kary. | `6e50e1c` |
| 5 | Wektor jednej wymuszonej noty był akceptowany, ale silnik czytał pięć pozycji przez `at()`. Puste/krótkie dane powodowały wyjście poza tablicę; noty ponad 20 mogły podbijać punktację. | Normalizacja do pięciu pozycji, brakujące/ujemne/niefinitywne wartości oznaczają brak wymuszenia, górny limit wynosi 20. | `9b8644b` |
| 6 | Podwyższenie belki trenera poniżej 95% HS nie odejmowało rekompensaty, mimo że wyższa belka zwiększała dystans. Reguła ochronna została błędnie zastosowana również do ujemnej rekompensaty. | Ujemna rekompensata za wyższą belkę obowiązuje niezależnie od progu 95%; próg nadal warunkuje dodatnią rekompensatę za obniżenie. | `6266cb8` |
| 7 | Zapis `points-for-meter` sprawdzał `autoPointsForKPoint` zamiast `autoPointsForMeter`. Ustawienie automatycznych punktów za K usuwało indywidualną stawkę za metr podczas zapisu; odwrotny układ gubił automatyczność. | Obie flagi są niezależne. Regresja sprawdza oba układy i odczyt zapisanego JSON. Poprawka nie odtwarza niestandardowych wartości utraconych we wcześniejszych zapisach. | `111d6d0` |
| 8 | Przedziały punktacji pomijały `169 < K < 170`. Skocznia K169,5 dostawała 0 pkt/m, co mogło dalej prowadzić do dzielenia przez zero przy wpływie belki. | Ciągły przedział do K170: K169,5 ma 1,8 pkt/m, K170 ma 1,2. | `b48fa0d` |
| 9 | Kąty wiatru z większym obrotem pozostawały poza zakresem: np. -450° było interpretowane jako wiatr przedni zamiast bocznego. Generatory mogły wyjść poza jeden obrót przy dużej niestabilności. | Normalizacja modulo 360 w setterze i konstruktorze kierunkowym; ujemne kąty otrzymują równoważny dodatni kierunek. Wartość niefinitywna jest neutralizowana. | `090bd61` |
| 10 | Ucięty rozkład normalny z odchyleniem 0 i średnią po złej stronie nigdy nie kończył odrzucania próbek. Odległy przeciwny ogon przy niezerowym odchyleniu również mógł praktycznie zatrzymać symulację. | Przypadek zdegenerowany kończy się deterministycznie na granicy 0; odległy ogon używa propozycji wykładniczej z poprawką akceptacji. Testuje się terminację, skończoność i średnią warunkową dla ogona czterech odchyleń. | `53a9960` |

## Optymalizacje async/multithread — decyzje

Z trzech rozważonych obszarów wdrożono jeden:

1. **Duże partie niezależnych pojedynczych skoków — wdrożone.** Od 4096 prób obsługa korzysta z zadania asynchronicznego i maksymalnie czterech pracowników. Mniejsze partie zachowują prostą ścieżkę. Dostępne jest też API `SingleJumpsManager::simulateAsync` do obliczeń bez widgetów w pracownikach.
2. **Skoki w tym samym konkursie — pozostają sekwencyjne.** Wynik wpływa na listę startową, linie, KO, ustawienia belki i dalszy przebieg. Równoległe wykonanie kolejnych zawodników zmieniłoby znaczenie tych operacji; nie uznano tego za bezpieczną optymalizację.
3. **Wiatry sektorów i noty jednego skoku — pozostają lokalne.** To małe tablice i krótka arytmetyka. Rozbijanie ich na zadania wymagałoby synchronizacji w każdym skoku; wybrano równoległość całych niezależnych prób, gdzie narzut rozkłada się na dużą partię.

### Izolacja danych i anulowanie

- Każdy pracownik ma własne kopie zawodnika, skoczni, reguł, generatora wiatrów i symulatora.
- Ustawienia globalne są pobierane raz jako snapshot przed rozpoczęciem. Zakresowy kontekst wątku kieruje odczyty silnika i skoczni do tej kopii. Test zmienia główne `maxSkills` na 0 po rozpoczęciu i sprawdza, że nie wpływa to na partię.
- Generatory liczb losowych i rozkład normalny są lokalne dla wątku. Nie ma wspólnego mutowanego `mt19937` ani blokady globalnego generatora przy każdym losowaniu.
- Zadania pobierają bloki 32 prób i sprawdzają anulowanie również pomiędzy skokami. Wyniki są składane według indeksów, niezależnie od kolejności zakończenia pracowników.
- Wyniki pracowników są wartościami bez wskaźników do ich lokalnych obiektów. Właściciel przypina je do własnego skoczka/skoczni dopiero po powrocie; serializacja i diagnostyka radzą sobie także z odłączonym snapshotem.
- Tylko główny wątek dotyka dialogu postępu i listy wyników. Oczekiwanie na przyszły wynik obsługuje zdarzenia; anulowanie czeka na bezpieczne zakończenie zadań i zachowuje ukończone skoki.
- Zachowano dopisywanie wyników, stałe i zmienne wiatry oraz niezmienność wcześniejszych historii wiatru.

### Pomiar

Benchmark tego samego jądra po poprawkach, trzy przebiegi po 20 000 skoków, sześć sektorów zmiennego wiatru:

| Pracownicy | Mediana |
| --- | ---: |
| 1 | 359 ms |
| 4 | 236 ms |

To przyspieszenie przepustowości około **1,52×**, czyli czas krótszy o około **34%** w tym pomiarze. Nie jest to obietnica takiego wyniku dla każdego komputera ani pomiar całego UI. Partie 1000 skoków pozostają poniżej progu równoległości.

Optymalizacja jest w osobnym commicie `a99df69`. Pierwszy pomiar podczas integracji wynosił 403/235 ms (około 42% krócej); w tabeli podano późniejszy pomiar izolowany, bez równoległego przebiegu testów interfejsu.

## Weryfikacja całej partii silnika

- `--engine-audit-only`: wszystkie 10 przypadków PASS; również normalny pełny zestaw wykonuje tę regresję.
- `--engine-async-benchmark-only`: kod 0, `/tmp/sj-engine-async-isolated.log`; snapshot ustawień, unikalne ID, serializacja odłączonych wyników, anulowanie przed startem, zakończenie po zniszczeniu właściciela i porównanie 1/4 pracowników.
- `--single-jumps-only`: kod 0, `/tmp/sj-engine-single-final.log`; anulowanie rzeczywistej dużej partii z dialogu, dopisywanie kolejnych wyników i historyczny wiatr.
- Pełny `scripts/test-quick-ui.sh`: kod 0, 391 testowych zrzutów interfejsu, `/tmp/sj-engine-full.log`.
- Wariant Widgets-only: kod 0, `/tmp/sj-engine-widgets.log`.
- Lokalne środowisko: Linux/Qt5.15.3, offscreen/software. Brak natywnego testu Windows/Qt6 i brak uruchomienia ThreadSanitizera. Wątki sprawdzono regresjami izolacji i cyklu życia; nie jest to obietnica deterministycznej identyczności losowych wyników między liczbami wątków.

Nie zmieniono w tej partii ogólnych zasad zaokrąglania ani parametrów balansu poza konkretną naprawą kary wysokości lotu i udokumentowanymi błędami. Nie przeprowadzono przebudowy fizycznego modelu skoku.

## Zadania dopisane przez użytkownika

Po zakończeniu audytu i optymalizacji kolejność jest następująca: wersja `2.0.0-beta` w osobnym commicie; następnie przycisk „Dodaj konkurs w kalendarzu” z przeglądu ma rzeczywiście dodawać konkurs i otwierać jego edycję; na samym końcu straty przed skokiem mają porównywać punkty po wagach na tym samym etapie, bez dodatkowego skoku lidera bieżącej serii. To osobne zmiany, niewliczane do dziesięciu błędów silnika.

## Dodatkowa kontrola pamięci i punkt kontrolny

Po ukończeniu dziesięciu poprawek silnika AddressSanitizer wykrył osobny błąd adaptera UI: zmiana języka odświeżała ukryte listy przez wskaźniki do zastąpionej bazy skoczków. Stos prowadził z QuickUi do QuickRoster::invalidateText i Jumper::getCountryCode. Poprawka `a72a6b5` odświeża etykiety na podstawie przechowywanych map wartości; osobna regresja zwalnia źródłowego skoczka przed takim powiadomieniem. Nie wliczono tego błędu do dziesięciu poprawek silnika.

W chwili bieżącego punktu kontrolnego regresja poprawki przeszła, a pełny ponowny przebieg ASAN jest jeszcze do potwierdzenia. Użytkownik polecił zapisać i wypchnąć wszystkie zmiany oraz kontynuować tę diagnostykę. Aktualny stan i polecenia są w [RESUME_ENGINE_AUDIT_2026-10-24.md](RESUME_ENGINE_AUDIT_2026-10-24.md).
