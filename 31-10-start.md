> **AKTUALIZACJA / aktywna praca:** użytkownik ponownie otworzył przebudowę list
> i zgłosił crash oraz niewidoczne dropdowny. Czytaj najpierw
> [`HANDOFF-31-10-LISTY.md`](HANDOFF-31-10-LISTY.md). Kod nowego katalogu inline
> jest WIP; poniższy opis 34/34 jest stanem wcześniejszej kolejki, nie zatwierdzeniem
> bieżącej przebudowy. Tłumaczenia i pełne testy nowego UI są do dokończenia.

# Sj.Sim — aktualne przekazanie pracy, 31.10.2026

## Zasady pracy

- Branch `feature/simulation-v2`, nigdy main; bez subagentów.
- Osobne commity zakończonych zmian i push po każdym.
- Nie zamykać gry użytkownika, nie używać jego danych do testów.
- UI: hierarchia, kontrast, jasność, małe obciążenie poznawcze, kompaktowy desktop.
- Qt5/Qt6: zachować wersjonowane importy dla Linux Qt 5.15.3.
- Używać lokalnych umiejętności Qt Quick. Nie uruchamiać dwóch buildów naraz.

## Zakres zakończonej kolejki

Wdrożono punkty z pierwotnej listy **34 zadań**. Pełna lista, commity i zakres
sprawdzeń: `docs/WORK_QUEUE_31_10.md`. Wydajność: dwa znalezione i zmierzone
miejsca zamiast pięciu, zgodnie z wyjątkiem dopuszczonym przez użytkownika.
Nie utożsamiać tego z przejściem pełnego starego smoke całej aplikacji.

Pierwotny plik użytkownika: `/home/konrad/Desktop/pracasjsimwazna.md`.
Historyczne przekazanie z etapu 15/34: `docs/HANDOFF_31_10_EARLY.md`.

## Ważne decyzje i zachowania list

- Zapisane listy mają nowy ekran: dwa panele, domyślnie kraje; mały ekran
  przełącza „Wszyscy” / „Wybrani”. Własne wiersze, checkboxy, zbiorcze akcje,
  pojedyncze +/−, przeciąganie między panelami, cofanie.
- Pusta lista oraz brak aktywnej listy oznaczają wszystkich zawodników.
- Limity krajów są wyłącznie pomocnicze; nigdy nie blokują wyboru.
- Kalendarz może przypisać listę; aktywuje ją przygotowanie konkursu.
  Ręczne zmiany składu tego konkursu nie zmieniają przypisania.
- Usunięcie aktywnej listy wybiera wszystkich i wyświetla informację.
- Listy działają w indywidualnych i drużynowych konkursach; kwalifikacje
  nadal ograniczają pulę dostępnych zawodników.
- Przy ponownym automatycznym doborze ręcznych składów pytać o potwierdzenie.
- `activeRosterFeatureEnabled()` jest teraz **true**; dawny opis o wyłączonej
  funkcji jest historyczny. Zapis listy i powiązania kalendarza działa po ID.

## Dodatkowe poprawki zgłoszone w rozmowie

- Nazwa klasyfikacji na całą szerokość karty (`4f2bad1`).
- Kontrast napisu automatycznego zapisu i usunięcie pól ważności (`bc06ed7`,
  `69bfe45`). W edytorze zasad nie było odrębnego parametru; usunięto ostatnie
  kontrolki/presety z konfiguracji konkursu i projekcji QuickBridge.
- Nagłówek treningu / próbnej / kwalifikacji zamiast „Konkurs” (`792a044`).
- Całkowite wyniki klasyfikacji bez części dziesiętnej w całej kolumnie;
  kolumna z ułamkami zachowuje precyzję (`9e19ed3`).
- „Próbna przed konkursem” oraz sama data w pasku sezonu (`13a3bf6`).

## Pusta lista po Qualifications → Classical KO

Użytkownik potwierdził: wyniki kwalifikacji istnieją, ale przed KO widział 0.
Naprawiono przypadek porównywania różnych instancji zawodników o tych samych ID
(`dc100ea`). Regresja obejmuje kopie zawodników, standardowy i KO awans,
ex aequo, DSQ/DNS i ochronę upadku. Pełny test sezonu przechodzi rzeczywiste
„Qualifications” → „Classical KO”, również po zapisie i ponownym wczytaniu.
Nie twierdzić, że odtworzono dokładnie prywatny zapis użytkownika. Jeśli zgłosi
powtórkę, trzeba nadal zbadać jego konkretny przypadek; nie dodawać obejścia
przepuszczającego wszystkich, gdy kwalifikacja jest pusta.

## Weryfikacja i dalsza diagnostyka

Ukierunkowane testy wymieniono w `docs/WORK_QUEUE_31_10.md`. Stary pełny smoke
nie jest potwierdzony jako zielony. `30-10-work.md` nadal zawiera pełną regresję
oraz dawny PB 212,5 + Enter: ochrony i testy były dodane, pierwotnego losowego
segfaulta nie odtworzono. Nie badać ponownie prywatnych danych wskazanego zawodnika.

Linux: `SJ_BUILD_JOBS=8 scripts/test-quick-ui.sh --simulation-v2-season-only`.
Kolejne uruchomienia bez przebudowy: w `build-quick-ui` ustawić **absolutne**
`LD_LIBRARY_PATH` i `QML2_IMPORT_PATH` do `build-qml-preview/qt-runtime/lib`
i `qml`, następnie `QT_QPA_PLATFORM=offscreen QT_QUICK_BACKEND=software`
i `./sj-sim-ui-smoke --<flaga>-only`. Test zmienia cwd do prywatnego katalogu,
więc ścieżki względne do QML są błędne. Nie są to crashe normalnej gry.

Windows: instrukcja w `docs/RESUME_WINDOWS_2026-10-30.md`; runner
`scripts/test-quick-ui-windows.py` izoluje dane. Po tłumaczeniach uruchomić
`python -X utf8 scripts/check-translations.py` i zachować komplet PL/EN/CS.
