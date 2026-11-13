# Instalator Windows i AppImage Linux

Aktualny opis: [DISTRIBUTION.md](../DISTRIBUTION.md).

Workflow `.github/workflows/release.yml` buduje Setup.exe i AppImage po pushu.
Gotowe formaty są testowane; archiwa ZIP/TAR.GZ z folderami programu nie są już
formatem dystrybucji. Tag `v…` publikuje wydanie, zwykły push tworzy artefakty CI.

`--deployment-smoke` sprawdza rzeczywiste załadowanie QML, a nie tylko obecność
procesu. Używa tymczasowej biblioteki zamiast danych gracza.

Poniższe wyniki dotyczą poprzedniego sposobu dystrybucji i są zapisem historycznym.

## Sprawdzone lokalnie — 4 listopada 2026

- `actionlint` 1.7.12: workflow bez błędów.
- Windows: MSVC, Qt 6.8.3, Release, `windeployqt`, uruchomienie widoków QML z paczki i ZIP. Test zakończył się kodem 0.
- Linux: WSL Ubuntu 26.04, GCC, Qt 6.10.2, pełny build Release, `linuxdeploy`, TAR.GZ i uruchomienie z gotowej paczki w trybie `offscreen`. Test zakończył się kodem 0. To dodatkowa kontrola zgodności; dokładny obraz CI z Qt 6.8.3 na Ubuntu 22.04 nie był uruchamiany.
- Skrypt Linux: składnia Bash i odrzucenie ścieżek wychodzących poza `dist`, z zachowaniem pliku kontrolnego.
- Windows: dwie kopie programu z różnymi folderami `userData` nie mogą równocześnie zapisywać wspólnych profili AppData.

Nie wykonano pusha, uruchomienia zdalnego workflow ani publikacji wydania.
