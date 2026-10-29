# Cztery poziomy siły perków

Obowiązuje jedna skala: **Lekko / Wyraźnie / Mocno / Skrajnie**
(**Light / Noticeable / Strong / Extreme**), wartości 1–4, z minusem dla
przeciwstawnego perka. Zero nadal oznacza brak perka. Liczba kategorii się nie
zmienia: skoczek ma trzy przeciwstawne pary, skocznia cztery.

## Wpływ na symulację

Cały zakres wpływu został rozłożony na cztery poziomy. Poziomy 1 i 2 też mają
nowe współczynniki. Dawne krzywe między brakiem perka i maksimum są próbkowane
w 1/4, 2/4, 3/4 i 4/4 zakresu. Zachowujemy ich kształt i kierunek działania;
nie zakładamy, że odległość lub prawdopodobieństwo wynikowe jest liniowe.

Przykład: mnożnik losowych strat perka Regularny:

| Poziom | Mnożnik straty |
| --- | --- |
| Brak | 1,00 |
| Lekko | 0,85 |
| Wyraźnie | 0,70 |
| Mocno | 0,55 |
| Skrajnie | 0,40 |

Skrajnie zachowuje poprzedni maksymalny efekt. Podobnie przeskalowano bonusy,
kary, rozrzut, specjalizację i parametry skoczni. W automatycznej pogodzie
wpływ loteryjności skoczni używa poziomu / 4. Natężenie wiatru i opady nie
zyskują dodatkowych poziomów — to osobne ustawienia.

## Jednorazowa konwersja danych

Gra nie utrzymuje osobnego silnika ani skali trzech poziomów. Profile i
konfiguracje eksperymentów zapisują `perkLevels: 4`; brak znacznika wymaga
jednorazowej konwersji, zamiast cichego odczytania starego Skrajnie jako Mocno.

**Zamknij grę przed konwersją**, aby otwarta instancja nie zapisała ponownie
danych starego formatu. W głównym folderze repozytorium uruchom:

```sh
python scripts/convert-four-perk-levels.py --app-data
python scripts/convert-four-perk-levels.py --app-data --apply
```

Pierwsze polecenie tylko pokazuje plan. Drugie przelicza znalezione dane w
standardowych folderach Sj.Sima i zapisach roboczych. Na Linuxie można użyć
`python3`. Nietypową lokalizację lub eksport paczki wskaż wprost:

```sh
python scripts/convert-four-perk-levels.py "ścieżka/do/paczki/pack.json" --apply
```

Konwersja zachowuje imiona, zdjęcia, OVR, formę i pozostałe dane. Dawne Lekko
pozostaje Lekko, Wyraźnie pozostaje Wyraźnie, a Skrajnie przechodzi z 3 na 4
(również dla ujemnych perków). Współczynniki zapisanych profili balansu są
przeliczane na nową skalę; nie kopiujemy starego współczynnika poziomu 2 do
nowego poziomu 2. Każdy zmieniany plik ma dokładną kopię `.three-levels.bak`.
Ponowne uruchomienie pomija już przekonwertowane pliki. Nieznane dane nie są
przepisywane. Pakiet `data/packs/men-2026/pack.json` jest już w nowym formacie.

Narzędzie nie pobiera danych z internetu. Test konwersji:
`python scripts/test-four-perk-levels.py`.
