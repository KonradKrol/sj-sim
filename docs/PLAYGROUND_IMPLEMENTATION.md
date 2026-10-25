# Playground symulacji — plan i stan implementacji

Cel: realistyczne rozkłady i zależności parametrów; eksperymenty na syntetycznych zawodnikach lub kopiach danych gry. Najważniejszy jest widok jednego skoczka: miejsca i ich rozkład, odległości, punkty, wiatr, forma, charakterystyka skoczni, upadki i losowość. Parametry eksperymentalne pozostają odizolowane od normalnej gry. Sprzęt docelowy: Ryzen 7 5700G, 32 GB RAM.

## Zakres

- [ ] Powtarzalne, izolowane strumienie losowania; nie zmieniają RNG normalnej gry.
- [ ] Wspólny silnik eksperymentów dla interfejsu i CLI, zapis konfiguracji do JSON.
- [ ] Batch konkursów na wielu wątkach; anulowanie i postęp, kontrola pamięci.
- [ ] Syntetyczna stawka oraz kopie zawodników, skoczni i zasad; parametry pojedynczego zawodnika i całej stawki.
- [ ] Porównania wariantów i skanowanie wartości parametru, wspólne ziarna losowania.
- [ ] Makro: rozkłady odległości/punktów, upadki/DSQ, wiatr i rekompensata, stabilność rankingu.
- [ ] Mikro: wybrany skoczek, histogram miejsc, zwycięstwa/podium/awans, rozkłady skoków, odtworzenie konkursu.
- [ ] Wnioski liczbowe i porównania, przedziały niepewności; korelacja opisana jako zależność, nie dowód przyczynowości.
- [ ] Eksport konfiguracji i raportów, CSV do samodzielnej analizy oraz CLI dostępne dla agenta.
- [ ] Widoczny dostęp z Ustawień; spójny Qt Quick, jasny/ciemny i małe ekrany.
- [ ] Testy powtarzalności, izolacji, wielu wątków, statystyk, anulowania, CLI i rzeczywistego UI.

## Decyzje

Nie przechowywać milionów pełnych obiektów skoków w pamięci. Statystyki zbierać agregacyjnie, próbki szczegółów ograniczać, a konkretny konkurs odtwarzać z jego numeru i ziarna. Nowe parametry silnika działają lokalnie dla eksperymentu; żadne uruchomienie nie zapisuje rekordów, nie zmienia baz ani sezonów.
