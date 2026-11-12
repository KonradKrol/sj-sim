# Sj.Sim

Symulator skoków narciarskich. Układasz sezon, wybierasz skoczków i skocznie, rozgrywasz konkursy. Wyniki zależą od umiejętności, formy, perków, wiatru, belki i lądowania.

![Menu główne Sj.Sim](docs/images/menu.png)

- Sezony z własnym kalendarzem, kwalifikacjami i klasyfikacjami.
- Konkursy indywidualne, KO i drużynowe oraz pojedyncze skoki.
- Edytory skoczków, skoczni, zasad i punktacji. Paczki można importować i udostępniać.
- Laboratorium do porównywania wyników i dostrajania symulacji.
- Polski, angielski i czeski. Jasny i ciemny motyw.

![Edytor bazy danych](docs/images/baza-danych.png)

## Uruchomienie

Rozpakuj całą paczkę. Na Windows uruchom `sj-sim.exe`, na Linuksie `Sj.Sim`. Zachowaj pozostałe pliki obok programu, w tym folder `Defaults`. Paczki, kalendarze i zapisy są w `Dokumenty/SjSimData`; ustawienia → Biblioteka pozwalają otworzyć lub zmienić ten folder. To wersja beta 2.0.0 — przed aktualizacją zrób kopię swoich danych i zapisów.

## Development

C++17, Qt 6 i QML; build przez qmake. [Przygotowanie Windows](docs/WINDOWS_SETUP.MD), [laboratorium](docs/BALANCE_LAB.md), [playground i CLI](docs/PLAYGROUND_V2.md).

GitHub Actions buduje przenośne paczki Windows i Linux po pushu. Są dostępne w artefaktach workflow; tag `v…` dodatkowo tworzy wydanie. [Lokalna weryfikacja deployu](docs/development/DEPLOY.md).

Paczkę ze zdjęciami i zależnościami udostępnisz jako jeden plik `.sjsim-pack`. [Foldery danych, import i tryb przenośny](docs/RESOURCE_LIBRARY.md).
