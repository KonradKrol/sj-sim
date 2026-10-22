# Przyspieszenie automatycznej symulacji

## Zmiany

### Obsługa całego konkursu, serii, grupy i kilku skoków

Przy przejściu do każdego kolejnego skoczka sygnał menedżera uruchamiał prezentację zawodnika: etykiety, flagę, zdjęcie, obliczanie poprzedniego miejsca i odświeżanie/fokus listy startowej. W automatycznej symulacji te pośrednie stany nie były potrzebne użytkownikowi, ale ich koszt ponoszono dla każdego skoku.

`CompetitionManagerWindow` grupuje teraz te aktualizacje. W trakcie partii nadal wykonuje wszystkie skoki, losuje warunki, zapisuje wyniki, wyznacza kwalifikacje, KO i przechodzi między seriami/grupami. Po partii odświeża prezentację zawodnika i listę. Flaga jest chroniona `QScopedValueRollback`.

Commit: `8090805`.

### Odczyt cech w silniku

`ClassWithCharacteristics::getLevelOfCharacteristic()` wcześniej tworzył kopię `QSet`, wymuszał jej odłączenie i przeszukiwał wszystkie elementy przy każdym odczycie. Ponadto funkcja hash cechy zwracała sam seed, przez co wszystkie typy trafiały do tego samego koszyka.

Odczyt korzysta teraz bezpośrednio z `constFind`, a hash zależy od typu cechy. Nie ma dodatkowej pamięci podręcznej, więc edycja lub zastąpienie cech nie wymaga unieważniania. Regresja sprawdza wartości dodatnie, ujemne, zero, brak cechy, jej zmianę oraz niezależność kopii umiejętności. Wzory fizyki, punktacji i prawdopodobieństw pozostają te same.

Commit: `af8f47c`.

### Logowanie

Kompilacje Release definiują `QT_NO_DEBUG_OUTPUT`, więc `qDebug` jest wyłączony także bez zmiennej środowiskowej użytkownika. Ostrzeżenia, błędy i informacje `qInfo` pozostają dostępne. Kompilacje Debug zachowują logi diagnostyczne.

Commit: `380b671`.

## Pomiary przed i po

Pomiar na tym samym środowisku Qt 5.15.3, Release `-O2`, z interfejsem offscreen/software. Przed poprawkami `QT_LOGGING_RULES='*.debug=false'` wyłączało logi, aby porównać rzeczywistą pracę programu niezależnie od kosztu wypisywania konsoli. Końcowy build nie potrzebuje tej zmiennej. Czasy konkursów obejmują wywołanie rzeczywistej akcji automatycznej i jej potwierdzenie; nie obejmują budowy aplikacji ani początkowego konfigurowania zawodów.

| Scenariusz | Przed | Po | Wynik |
| --- | ---: | ---: | --- |
| Silnik: 10 000 skoków, z cechami zawodnika i skoczni | 254 ms | 87 ms | Około 2,9× szybciej |
| Konkurs indywidualny, około 130 skoków; mediana 3 prób | 1538 ms | 67 ms | Około 23× szybciej |
| Konkurs KO, około 160 skoków | 2037 ms | 50 ms | Około 41× szybciej |
| Konkurs drużynowy, 32 skoki | 66 ms | 36 ms | Około 1,8× szybciej |
| Powiadomienia zmiany danych listy, indywidualny konkurs | około 130 | 1 | Jedna końcowa aktualizacja prezentacji |

Poszczególne losowania mogą dawać remisy, dodatkowych finalistów i nieznacznie różną liczbę skoków (129–131 w trzech indywidualnych próbach); liczby nie są gwarancją czasu na każdym komputerze. Koszt wyłączenia samych logów nie został osobno skwantyfikowany.

Logi pomiarów: `/tmp/sj-simulation-baseline.log`, `/tmp/sj-simulation-batched.log`, `/tmp/sj-simulation-final.log`.

## Weryfikacja

- `scripts/test-quick-ui.sh --simulation-benchmark`: silnik oraz trzykrotny konkurs indywidualny, KO i drużynowy; sprawdza ukończenie, rzeczywiste skoki, poprawnych właścicieli wyników i skończoną punktację.
- `scripts/test-quick-ui.sh --bulk-only`: automatyczny cały konkurs w trzech odmianach, pojedyncza seria, dokładnie 37 skoków i grupa drużynowa. Sprawdza granice operacji, następnego zawodnika oraz ograniczoną liczbę aktualizacji listy. Te scenariusze należą również do pełnego smoke testu.
- Regresja odczytu cech należy do testów backendu. Zmiana nie zawiera testu z twardym limitem czasu, zależnego od obciążenia komputera.

Test użytkownika: rozegrać automatycznie cały konkurs, następnie na nowych zawodach serię, kilkanaście skoków i grupę drużynową; po każdej części sprawdzić wyniki, listę i zawodnika na belce.
