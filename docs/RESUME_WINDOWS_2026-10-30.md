# Windows: kolejka 30 października

> Dokument historyczny. Aktualny stan i kolejność wznowienia: `../31-10-start.md`.

Kontynuować `30-10-work.md`, potem wszystkie punkty z
`C:/Users/user/Desktop/praca sj sim.md` w ich kolejności. Bez subagentów.
Branch `feature/simulation-v2`, HEAD `b08a29e`. Nie zamykać gry użytkownika.

## Bieżący stan

Build aplikacji i kontrola tłumaczeń przeszły po pullu 45 commitów. Pełna
wersja testowa Windows została przebudowana. Zmiany robocze nie są jeszcze
zatwierdzone ani wypchnięte; pełna regresja NIE jest zielona.

- `--athlete-commit-only` PASS: sześć nowych profili, PB 212,5 + Enter,
  odczyt zapisanego pliku i cykl życia kontrolerów. Pierwotny segfault nadal
  nieodtworzony; nie przedstawiać go jako definitywnie naprawionego.
- `--space-only` PASS po dostosowaniu sprawdzeń do studiów, aktywnej kategorii
  i jej obszaru roboczego. `--layout-only` również przeszedł.
- Poprawka `CatalogWorkspace.qml`: pamięta kontroler studia podczas zmiany
  kategorii, aby niszczony widok nie dostawał undefined. Zniknęły związane z tym
  komunikaty QML w `--space-only`.
- Pełny test musi mieć izolowany katalog pliku EXE. W `build-win-smoke/release`
  istnieją stare testowe simulationSaves. Dodano runner
  `scripts/test-quick-ui-windows.py`, który kopiuje EXE i fixture do temp,
  używa Qt DLL przez PATH i zapisuje wskazany log.
- Testy menu wymagały oczekiwania po animacji zamykania. W testowym main
  wymuszono Qt::AA_DontUseNativeDialogs dla testowania bez ekranu.
- Kolejna przeszkoda: `--workflow-only` przekracza 90/120 sekund przy operacjach
  kalendarza. Dokładna przyczyna NIE została ustalona. Nie pomijać tego testu
  ani nie oznaczać całej kolejki jako skończonej.

## Diagnostyka workflow

Log `build-tools/queue-workflow.log`. Ostatnio uruchomiony proces orchestration
ma exec session 98789; runner ma timeout 120 s i zabija tylko własny subprocess.
Wcześniejsze testy izolowane były ręcznie zatrzymywane wyłącznie po sprawdzeniu,
że ścieżka EXE jest w `sj-smoke-runtime-*` lub `sj-ui-runtime-*`.

Śledzenie (już usunięte z kodu) potwierdziło dotarcie za wybór wyniku
`calendar-field-hill` w `hillSearchRegression()`. Sekwencja w
`seasonWorkflowRegressions()` około linii 1310: render custom rules, sekcja 2,
render calendar-links, powrót do sekcji 0, resize, hillSearchRegression, undo.
Zatrzymanie jest w tym rejonie; nie wiadomo jeszcze dokładnie gdzie.
Guard QTimer w workflow zgłasza nieoczekiwane QMessageBox, także sjQuickPage,
ale nie ujawnił dotąd komunikatu. Zmiana ContentHeight timer 0→16 nie pomogła
i została CAŁKOWICIE wycofana. Nie powtarzać tej hipotezy jako potwierdzonej.
Ostatnia próba zmienia keyClick w hillSearchRegression na natywne windowHandle;
wynik jeszcze do oceny. Warto rozważyć dokładne śledzenie po końcowym ponownym
otwarciu i zamknięciu selektora, oraz undo. Obecny helper czeka 250 ms po close.

## Polecenia

Build przyrostowy (nie używać ponownie qmake bez potrzeby, wymusza szeroki build):
`powershell -ExecutionPolicy Bypass -File build-tools/wind-form-incremental.ps1 -BuildDirectory build-win-smoke`

`python scripts/test-quick-ui-windows.py --log build-tools/queue-workflow.log --timeout 120 -- --workflow-only`

Pełny test: ten sam runner bez argumentów po `--`, timeout domyślnie 2400 s.
Logi wcześniejszych przebiegów w `build-tools/queue-30-full.log`.
PB test ma log na końcu `build-tools/queue-30-smoke-build.log`.
`python -X utf8 scripts/check-translations.py` (Windows wymaga UTF-8).

Nie robić równoległych buildów w tym samym katalogu. Pierwsza pełna kompilacja
smoke zajęła kilkanaście minut; zmiana testowego main około 90 s. Nie wykonywano
jeszcze zmian z dużego pliku z pulpitu. Jego treść była przeczytana w rozmowie.
