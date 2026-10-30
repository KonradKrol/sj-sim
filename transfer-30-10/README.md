# Tymczasowa kopia paczek — 30.10.2026

Migawka zapisanego stanu paczek z tego komputera. Nie synchronizuje się
samoczynnie z kolejnymi edycjami w grze.

- `skoczkowie/pack.json`: **Season 2026/2027**, 96 skoczków.
- `skocznie/pack.json`: aktualna paczka skoczni, 126 obiektów.

## Na drugim komputerze

1. Zrób pull brancha `feature/simulation-v2` i zbuduj aktualną wersję gry.
2. Baza danych → Zawodnicy → **••• → Importuj paczkę**.
   Wybierz `transfer-30-10/skoczkowie/pack.json`.
3. Baza danych → Skocznie → **••• → Importuj paczkę**.
   Wybierz `transfer-30-10/skocznie/pack.json`.

Import tworzy osobne kopie. Paczki znajdziesz na listach wyboru w odpowiednich
edytorach; można ich później używać do konfiguracji gry.

Zachowano wszystkie zapisane parametry, rekordy i ścieżki zdjęć. Wskazane
zdjęcia nie były obecne w źródłowym folderze paczki, więc ta migawka ich nie
zawiera. Można je później dołożyć w folderze zaimportowanej paczki.

`snapshot.json` zawiera liczebność i sumy kontrolne plików. Folder jest
przeznaczony do przeniesienia danych; po udanym imporcie można go usunąć z repo.
