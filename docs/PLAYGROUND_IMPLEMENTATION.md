# Playground symulacji — plan i stan implementacji

Cel: realistyczne rozkłady i zależności parametrów; eksperymenty na syntetycznych zawodnikach lub kopiach danych gry. Najważniejszy jest widok jednego skoczka: miejsca i ich rozkład, odległości, punkty, wiatr, forma, charakterystyka skoczni, upadki i losowość. Parametry eksperymentalne pozostają odizolowane od normalnej gry. Sprzęt docelowy: Ryzen 7 5700G, 32 GB RAM.

## Zakres

- [x] Powtarzalne, izolowane strumienie losowania; nie zmieniają RNG normalnej gry.
- [x] Wspólny silnik eksperymentów dla interfejsu i CLI, zapis konfiguracji do JSON.
- [x] Batch konkursów na wielu wątkach; anulowanie i postęp, kontrola pamięci.
- [x] Syntetyczna stawka oraz kopie zawodników, skoczni i zasad; parametry pojedynczego zawodnika i całej stawki.
- [x] Porównania wariantów i skanowanie wartości parametru, wspólne ziarna losowania.
- [x] Makro: rozkłady odległości/punktów, upadki/DSQ, wiatr i rekompensata, stabilność rankingu.
- [x] Mikro: wybrany skoczek, histogram miejsc, zwycięstwa/podium/awans, rozkłady skoków, odtworzenie konkursu.
- [x] Wnioski liczbowe i porównania, przedziały niepewności; korelacja opisana jako zależność, nie dowód przyczynowości.
- [x] Eksport konfiguracji i raportów, CSV do samodzielnej analizy oraz CLI dostępne dla agenta.
- [x] Widoczny dostęp z Ustawień; spójny Qt Quick, jasny/ciemny i małe ekrany.
- [x] Testy powtarzalności, izolacji, wielu wątków, statystyk, anulowania, CLI i rzeczywistego UI.

## Decyzje

Nie przechowywać milionów pełnych obiektów skoków w pamięci. Statystyki zbierać agregacyjnie, próbki szczegółów ograniczać, a konkretny konkurs odtwarzać z jego numeru i ziarna. Nowe parametry silnika działają lokalnie dla eksperymentu; żadne uruchomienie nie zapisuje rekordów, nie zmienia baz ani sezonów.


## Architektura końcowa

- `PlaygroundConfig.cpp`: opis parametrów, przykład JSON i walidacja przed uruchomieniem natywnych parserów.
- `PlaygroundRunner.cpp`: pule pracowników, rzeczywiste managery konkursów, strumienie losowania, agregaty i odtwarzanie.
- `PlaygroundController.cpp`: asynchroniczne zadania, postęp, anulowanie, historia i źródła danych skopiowane z baz.
- `PlaygroundIo.cpp`: wspólne eksporty i CLI działające przez QCoreApplication, bez okna.
- `ExperimentalSimulation`: współczynniki lokalne dla wątku, z przywracaniem zakresu. W zwykłej grze pozostają nieaktywne.
- QML: osobne konfiguracja, porównania, perspektywa skoczka, odtworzenie konkursu, historia i opis CLI.

Każdy pracownik jest właścicielem swoich kopii zawodników, skoczni, zasad i managerów. Nie przekazuje obiektów GUI między wątkami. UI otrzymuje gotowe raporty; postęp jest atomowy. Raport zachowuje wyłącznie kompletne porównania wszystkich wariantów. Pamięć zależy od stawki, wariantów i ograniczonej próbki, a nie od całkowitej liczby konkursów.

Szczegóły użytkowe: [instrukcja](SIMULATION_PLAYGROUND.md). Przykłady: [konfiguracje JSON](../examples/playground). Testy i pomiary: [walidacja](PLAYGROUND_VALIDATION_2026-10-25.md).
