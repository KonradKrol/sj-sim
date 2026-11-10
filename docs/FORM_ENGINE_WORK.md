# Automatyczna forma — stan prac

Specyfikacja właściciela: [FORM_SYSTEM.md](FORM_SYSTEM.md), skopiowana z pulpitu bez zmian. Zakres: wyłącznie MVP. Wartości startowe wymagają kalibracji, nie są obietnicą gotowego balansu.

## Ustalenia właściciela

- Przy aktywnej auto-formie zablokować ręczne zmienianie F, również w backendzie. W edytorze sezonowym pokazać krótką diagnostykę: F, μ, reżim, ostatnie przeliczenie. Nie implementować teraz nowych sposobów ręcznego zarządzania formą.
- Kalibracja z laba ma wpływać na nowe rozgrywki. Nowy zapis przejmuje domyślny profil; rozpoczęte zapisy zachowują własny snapshot.
- Bez limitu liczby perków; nowa, czwarta para jest niezależna.
- Aktualizować przed konkursem (także przed przygotowaniami); cofać zmiany, gdy gracz wyjdzie bez rozegrania.
- Na razie nie wdrażać celu „3–6 ze 100 osiąga 90+”. Raportować rzeczywiste statystyki bez oceny zgodności z tym celem.

## Rozpoznana architektura

- `source/athletes/AthleteProfile.{h,cpp}`: F i OVR są całkowite 1–100; trzy pary perków, każda od −4 do +4. Nowy perk musi być odrębny od regularności pojedynczego skoku.
- `source/simulation-v2/QualityModel.cpp` / `ProfileBridge.cpp`: forma jest już wejściem jakości skoku. Nie przebudowywać jej wpływu ani OVR.
- `source/form-generator/JumperFormGenerator.cpp`: dawny ręczny generator, nie model czasowy. Nie używać jego niestabilności jako stanu nowego procesu.
- `source/seasons/SimulationSave.{h,cpp}`: JSON, atomowy zapis QSaveFile, walidacja grafu i profili przed wczytaniem. Nowy opcjonalny obiekt stanu formy musi mieć pełną walidację. Brak obiektu w dotychczasowym zapisie oznacza auto-formę wyłączoną.
- `source/seasons/Season.{h,cpp}`: flagi dat i rozpoczęcia; daty znajdują się w CompetitionInfo. Wiele kalendarzy wymaga ochrony przed ponownym przeliczeniem oraz cofaniem dat.
- `source/UI/Seasons/SimulationSaveManagerWindow.cpp`: tworzenie przygotowań i uruchamianie konkursu; wyjście odtwarza stan wydarzenia. Integracja formy nie może zepsuć tej transakcji. `seasonAthleteDraft` obsługuje nowy edytor zawodnika.
- `source/simulation-v2/ProfileDraft.{h,cpp}`: wspólny kontrakt edytora; blokada F ma dotyczyć kontekstu sezonowego, nie edytora paczek.
- `source/UI/Quick/QuickBridge.cpp`, `SeasonBar.qml`: widoczny przełącznik sezonowy i jego dostępność zależna od dat.
- `source/UI/AppSettings/AppSettingsWindow.cpp`, `QuickBridge.cpp`, `Node.qml`: wejście do laboratorium z ustawień, zgodnie z istniejącym wzorcem BalanceLabWindow.
- `source/UI/Quick/BalanceLabPlot.qml`: gotowe wykresy do wykorzystania bez projektowania nowego rozbudowanego UI.
- Projekt budowany przez qmake (`sj-sim.pro`), Qt 5.15 na tym Linuksie. Zachować wersjonowane importy QML.

## Podział prac

1. Niezależny silnik i konfiguracja: CTMC, μ, F, perk, deterministyczny i zapisywalny RNG. Bez dostępu do UI i wyników skoków.
2. Testy matematyczne: Laplace o wariancji 1; dodatnie, znormalizowane przejścia; zachowanie F₀; niezmienność wyniku po podziale aktualizacji na dni; dokładne wznowienie po serializacji; stały czas pobytu niezależny od perka.
3. Runner/lab: asynchroniczna próba i batch seedów; wykresy F/μ/reżimu, kilka historii, statystyki populacji, niepewność wyników; jedna konfiguracja parametrów, zapis/eksport i porównanie A/B.
4. Integracja: stan według trwałych identyfikatorów zawodników, wyłącznie aktywne daty, brak nadrabiania wyłączonego okresu, brak resetu po długiej przerwie. Walidacja zapisu i przełącznika.
5. Blokada ręcznej formy, diagnostyka w edytorze, czwarta para perków po decyzji właściciela. Zachować możliwość wczytywania profili zawierających poprzednie trzy pary (nowa para neutralna).
6. Testy integracji i kalibracji; osobne commity i push po każdym. Bez subagentów, bez zmian na main.

### Zrobione: niezależny rdzeń

`source/form-engine/FormEngine.{h,cpp}`: konfiguracja i opisy parametrów, inicjalizacja bez zmiany F₀, CTMC z zachowanym czasem oczekiwania, dryf μ, dobowe innowacje Laplace’a, konfigurowalne dziewięć wariantów perka, serializacja stanu i domyślnego profilu. Korzysta z istniejącego SeededRandom, który otrzymał odczyt stanu bez zmiany algorytmu. F i μ są ograniczane po aktualizacji do 1–100; to ograniczony, dyskretny model, nie dokładny nieograniczony proces OU.

`scripts/test-form-engine.sh` przeszedł: F₀, konfiguracja, inicjalizacja, normalizacja przejść, kierunek działania perka, 150 pojedynczych dni kontra jeden przeskok, wznowienie po JSON do 1000. dnia, identyczny dzień bez zużycia RNG, odrzucenie cofnięcia czasu. Próba 100 000 niezależnych jednodniowych aktualizacji potwierdza wariancję Laplace’a wynikającą z równania. Do zrobienia: runner/lab, integracja sezonu, czwarta para i blokada edycji, testy integracji.

## Ważne uwagi matematyczne

- Laplace(scale=1) ma wariancję 2. Do równania ze specyfikacji potrzebny jest scale=1/√2.
- CTMC: zapisywać także pozostały czas do przejścia. Długie przerwy przechodzą przez wszystkie dni i ewentualne reżimy, zamiast jednego losowania na konkurs.
- Zmienność F liczyć w stałym kroku dobowym; `sqrt(1−a²)` zachowuje skalę wariancji przy stałej bazie. Ruchoma μ i ograniczenie wartości zmieniają rozkład stacjonarny — opisać to uczciwie.
- Własny strumień RNG na zawodnika; nie korzystać z globalnego losowania skoków. Kolejność zawodników i częstotliwość zapisu nie mogą zmieniać historii.
- Surowy stan F zachowuje ułamki; profil gry dostaje wartość całkowitą 1–100. Nie kumulować błędu zaokrąglania w procesie.
- Cele 90+ zależą od rozkładu F₀ i perków. Raportować próbę i wiele seedów, nie wymuszać limitu 3–6 osób.
