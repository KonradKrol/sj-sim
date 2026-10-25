# Playground — walidacja 25.10.2026

## Zakres i środowisko

Linux, Qt 5.15.3, kompilacja release, AMD Ryzen 7 5700G. Testy używają izolowanych katalogów; nie modyfikują rzeczywistych sezonów ani baz. Windows i Qt 6 nie były tutaj uruchamiane — kod zachowuje obie ścieżki budowania, lecz nie zastępuje to testu na docelowym systemie.

Identyfikator wykonywalnego pliku użytego do pomiarów:

`playground-1 / 5.15.3 / x86_64-little_endian-lp64 / sha256:635d7ac7ff86cbb25ae21aec2db5be3b38a60cc0940265c28c161e813f055637`

## Wyniki testów

- Pełny dotychczasowy zestaw Qt Quick: PASS, **407 responsywnych renderów**, scenariusze indywidualne, KO, drużynowe i sezony.
- Końcowy `scripts/test-quick-ui.sh --playground-only`: PASS. Silnik, izolacja ustawień i losowania, identyczne warianty, wiele wątków, neutralne/zmienione współczynniki, remisy, źródłowe dane, anulowanie, eksport i walidacja. Rzeczywiste wejście z Ustawień, obliczenia, historia, analiza każdego skoczka, odtworzenie i **38 renderów** playgroundu, w tym wąskie ekrany, oba motywy i większy tekst.
- Budowa alternatywnego Qt Widgets: PASS.
- Test CLI w osobnych procesach: PASS, histogramy i zachowane przykłady zgodne między 1 i 8 wątkami; dokładne odtworzenie, eksporty CSV/JSON i analiza Pythona. SIGINT: kod 130, zapisano 120 ukończonych porównań wszystkich wariantów.
- Neutralne eksperymentalne mnożniki 1/1/1 dają ten sam wynik co dotychczasowa ścieżka. Zakres eksperymentu jest przywracany także przy zagnieżdżeniu i oddzielony między wątkami.

## Pomiar wielu wątków

Komenda:

```bash
python3 scripts/test-playground-cli.py --competitions 1000 --keep
```

1000 konkursów × 3 warianty, 50 zawodników, dwie serie z limitem finałowym 30:

| Pracownicy | Czas |
| --- | ---: |
| 1 | 14.062 s |
| 8 | 3.212 s |

Przyspieszenie **4.38×**, czas mniejszy o 77.2%. To pomiar jednego scenariusza na tej maszynie, a nie deklaracja wydajności wszystkich presetów. Wartości agregatów zmiennoprzecinkowych mogą różnić się na ostatnich cyfrach w zależności od kolejności łączenia wątków; histogramy i odtwarzane skoki są zgodne.

## Przykład: przewaga warunków

Konfiguracja [wind-advantage.json](../examples/playground/wind-advantage.json), w tym pomiarze zmieniono tylko `competitions` z 5000 na **1000**. Ziarno `20261025`, wspólna belka 33, zawodnik indeks 24, reszta parametrów z pliku. Każdy wariant ma 1000 powtórzeń.

| Wariant | Średnie miejsce | Średnia odległość | Średnia nota skoku |
| --- | ---: | ---: | ---: |
| Bazowy | 26.08 | 122.43 m | 115.90 pkt |
| athlete.windBias = -3 | 48.28 | 90.65 m | 87.13 pkt |
| athlete.windBias = -2 | 45.80 | 99.61 m | 93.14 pkt |
| athlete.windBias = -1 | 38.20 | 110.91 m | 103.85 pkt |
| athlete.windBias = 1 | 19.73 | 129.58 m | 121.42 pkt |
| athlete.windBias = 2 | 18.45 | 134.87 m | 122.46 pkt |
| athlete.windBias = 3 | 21.96 | 140.20 m | 118.70 pkt |

Przy tych ustawieniach rekompensata nie wyrównuje skutków różnicy warunków: wiatr w plecy pogarsza średnie miejsce, a +2 m/s pod narty poprawia je mocniej niż +3 m/s. Samo zwiększanie metrów nie gwarantuje poprawy noty. To obserwacja aktualnego modelu w kontrolowanej konfiguracji, nie wniosek o całym sporcie. Kolejny eksperyment może rozdzielić wpływ not, rekompensaty, upadków i reakcji belką.

## Przykład: wybicie / lot

Konfiguracja [takeoff-flight.json](../examples/playground/takeoff-flight.json), również **1000** powtórzeń na wariant, to samo ziarno i belka. Stała suma atrybutów, zmieniony podział wybicia/lotu:

| Wariant | Średnie miejsce | Średnia odległość | Średnia nota skoku |
| --- | ---: | ---: | ---: |
| 70 / 70 | 26.08 | 122.43 m | 115.90 pkt |
| 50 / 90 | 16.31 | 126.81 m | 124.65 pkt |
| 60 / 80 | 21.24 | 124.61 m | 120.31 pkt |
| 80 / 60 | 30.73 | 120.23 m | 111.47 pkt |
| 90 / 50 | 34.97 | 118.03 m | 107.04 pkt |

Na tej skoczni model wyraźnie premiuje przesunięcie atrybutów w stronę lotu. Dla 50/90 względem 70/70 średnia różnica miejsca wyniosła -9.77; przybliżony 95% przedział [-10.04, -9.50]. Nie dowodzi to optymalnego balansu: warto powtórzyć skan dla innych K/HS, balansu skoczni, formy, wiatru i eksperymentalnych mnożników.

## Odtworzenie i analiza

Pliki przykładów w repo mają po 5000 konkursów na wariant. Do dokładnego powtórzenia powyższych tabel ustaw 1000; pozostałych pól nie zmieniaj. Zapisz plik jako nową konfigurację i uruchom:

```bash
scripts/run-quick-ui.sh --playground --config moja-konfiguracja.json --output results/nowy-batch --threads 8
python3 scripts/analyze-playground.py results/nowy-batch
python3 scripts/analyze-playground.py results/nowy-batch --focus 12
```

Surowe raporty końcowej walidacji powstały lokalnie w `/tmp/sj-playground-cli-_f2g2dai` i `/tmp/sj-playground-delivery-examples`. Są danymi testowymi poza repo; trwałym opisem odtworzenia są konfiguracje, ziarno, komendy i powyższe tabele. Nie polegaj na zachowaniu katalogów `/tmp`.
