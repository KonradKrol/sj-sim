# Biblioteka Sj.Sima

## Dane gracza

Domyślna lokalizacja to **Dokumenty/SjSimData** (systemowy folder Dokumenty,
nie zależy od katalogu roboczego ani AppData/Roaming).
Ustawienia → Biblioteka udostępnia „Otwórz folder” i „Zmień folder”.
Zmiana wymaga pustego katalogu: przy zamknięciu gry kopiowane są wszystkie
bieżące dane, a następne uruchomienie korzysta z nowego miejsca. Oryginał zostaje.
Nieudane kopiowanie nie zmienia wybranego miejsca.

Tryb przenośny: przed uruchomieniem utwórz pusty `portable.txt` obok pliku
wykonywalnego. Biblioteka powstanie w `SjSimData` obok niego. Ten tryb ma
pierwszeństwo przed ustawieniem folderu; usuń `portable.txt`, aby używać
folderu wybranego w ustawieniach. W dystrybucji Linux jest to `usr/bin`.
Nie zapisuj przenośnej instalacji w folderze bez uprawnień do zapisu.

```text
SjSimData/
  Packs/
    Athletes/<UUID>/pack.json + zdjęcia ze względnych ścieżek
    Hills/<UUID>.json
    Rules/<UUID>.json
    Points/<nazwa>.json
    catalog.json                   # wyłącznie kolejność i aktywne paczki
  Calendars/<nazwa>.json
  Saves/<nazwa>.json
  Saves/.order                     # kolejność zapisów
  Settings/{app,simulation,countries}.json
  Tools/{FormPresets,ImportancePresets,BalanceLab,FormEngine,Playground}/
  library.json                     # znacznik przygotowanej biblioteki
```

Paczek, punktacji, presetów i zapisów nie trzymamy już w
`userData/GlobalDatabase`, `userData/Packs`, AppData ani dawnym `simulationSaves`.
Nie ma automatycznej migracji starej biblioteki. Stare pliki pozostają nietknięte.
Klasa C++ `GlobalDatabase` nadal służy jako adapter istniejących ekranów w pamięci;
nie jest drugim plikiem bazy danych ani autorytatywnym magazynem paczek.

## Pusta instalacja i wydawanie bety

Źródło zasobów startowych to **`data/defaults/` w repozytorium**.
Build i oba skrypty pakujące umieszczają je w `Defaults` obok executable.
Przy pierwszym uruchomieniu kopiowane są do biblioteki:

- jedna domyślna paczka 83 skoczków, razem ze zdjęciami;
- jedna paczka Hills 26/27 — 128 skoczni;
- jedna aktualna paczka zasad;
- pięć aktualnych presetów punktacji;
- sześć presetów kalendarza: LGP23, LGP24, LGP26, WC23-24, WC24-25, WC26-27;
- kraje i narzędziowe presety formy/ważności.

Kolejne uruchomienia nie nadpisują edycji ani nie przywracają usuniętych zasobów.
`Defaults` jest wzorcem instalacyjnym, a nie miejscem edycji. Aby zmienić zasoby
następnej bety, zmień `data/defaults`, zbuduj i spakuj. Dawne `data/beta/Packs`
i repozytoryjne `userData` nie są już źródłem pakowania. Kalibracja silnika
`data/beta/default-balance.json` nadal jest zasobem silnika, niezależnym od paczek.

## Udostępnianie

Biblioteka jest dostępna z ustawień i paska paczek edytora. Zaznacz dowolną
kombinację zasobów i wybierz „Eksportuj”. Otrzymasz **jeden `.sjsim-pack`**.
Eksport pojedynczej paczki z edytora korzysta z tego samego formatu.
Import: „Wczytaj paczkę”, upuszczenie pliku na ekranie głównym/bibliotece lub
uruchomienie `sj-sim ścieżka/do/pliku.sjsim-pack`.

- Zdjęcia wskazane przez zawodników są dołączane; brak opcjonalnego zdjęcia
  nie blokuje eksportu. Po imporcie ścieżki pozostają względne do paczki.
- Kalendarze zawierają swoją strukturę i dołączają pasujące paczki skoczni,
  zasad oraz punktacje. Nie wymagają ręcznego zbierania kilku plików.
- Eksport symulacji dołącza dostępne paczki i punktacje biblioteki.
- Istniejące pliki nie są nadpisywane. Kolizja identyfikatora paczki tworzy
  nowy identyfikator; kolizja nazwy presetu/zapisu dopisuje `(2)`, `(3)` itd.
- Cały import jest walidowany w folderze tymczasowym przed publikacją.
  Niepoprawna ścieżka, uszkodzone zdjęcie lub niepoprawny zasób odrzuca paczkę.
- Importowane zasoby pozostają w bibliotece po anulowaniu innych zmian edytora.

## Format dla narzędzi

To własny kontener, nie ZIP: nagłówek bajtowy `SJSIM-PACK\n1\n`, dalej UTF-8 JSON:

```json
{"format":"sj-sim-pack","version":1,"resources":[
  {"kind":"Athletes","document":{},"files":[
    {"path":"photos/POL_NameSurname.png","data":"<base64>","sha256":"<hex>"}
  ]}
]}
```

`document` zawiera właściwy pełny dokument danej kategorii; puste `{}` powyżej
pokazuje jedynie strukturę, nie stanowi poprawnej paczki zawodników.
Kategorie: `Athletes`, `Hills`, `Rules`, `Points`, `Calendars`, `Saves`.
`files` obsługuje zdjęcia zawodników. Kontener ma limit 256 MiB, pojedynczy
załącznik 32 MiB. Nie wykonujemy żadnego kodu z paczek.

## Pliki implementacji i weryfikacja

- `source/global/UserDataPaths.*`: lokalizacja, zasoby startowe i zmiana folderu.
- `source/global/ResourceBundle.*`: eksport/import i zależności.
- `source/simulation-v2/SimulationCatalog.*`: jeden katalog paczek.
- `source/UI/Library/ResourceLibraryWindow.*`, `source/UI/Quick/ResourceLibrary.qml`.
- `tests/resource-library.cpp`, `scripts/test-resource-library.py`: izolowane
  testy z bieżącymi obiektami po buildzie, bez edycji danych użytkownika.

```sh
scripts/build-quick-ui.sh
python3 scripts/test-resource-library.py
QT_QPA_PLATFORM=offscreen QT_QUICK_BACKEND=software build-quick-ui/sj-sim --deployment-smoke
```

Weryfikacja 12.11.2026 (Linux, Qt 5): build zakończony poprawnie; izolowany
roundtrip paczki i przykładowej symulacji z dwoma kalendarzami przeszedł.
Sprawdzono także odrzucenie uszkodzonych zdjęć, ścieżek wychodzących poza paczkę
oraz duplikatów, zachowanie istniejących plików, kopiowanie ostatnich zmian
przy przenoszeniu folderu i faktyczne otwarcie ekranu Biblioteki w Quick UI.
Test startu pustej instalacji `--deployment-smoke`: PASS.
Windows: skrypt pakujący został dostosowany; build Windows wymaga sprawdzenia
na Windows.
