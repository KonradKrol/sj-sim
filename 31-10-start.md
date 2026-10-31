# Sj.Sim — wznowienie 31.10.2026

Pracuj na `feature/simulation-v2`, bez subagentów. Użytkownik chce wykonywać
kolejno wszystkie punkty z `C:/Users/user/Desktop/praca sj sim.md`, bez
kończenia pracy po pojedynczym zadaniu. Po każdej ukończonej zmianie zrób
osobny commit i push. Nie zamykaj działającej gry użytkownika i nie używaj
jego rzeczywistych zapisów do testów.

## Stan listy z pulpitu: 15 z 34 głównych punktów

Ukończone, przetestowane i wypchnięte:

- **Edytor bazy, 4/4:** ustawienia paczki we wszystkich kategoriach i uporządkowane menu; przeciąganie skoczków z cofnięciem; sortowanie skoczków i skoczni według kraju z potwierdzeniem; automatyczne punkty za wiatr w plecy widoczne bez możliwości edycji. Punkty za wiatr w plecy wynikają z wartości dla wiatru pod narty, z limitem 100.
- **Kalendarze i daty, 4/4:** hierarchia i zasady UI w `AGENTS.md`; czytelniejsze szybkie planowanie i krótszy wybór niedzielnych kwalifikacji; widoczne, jasne ostrzeżenia; usunięta z edytora nieużywana ważność treningów.
- **Tryb sezonów, 4/4:** mniejszy pasek sezonu, uporządkowane akcje, wyróżniony przycisk zapisu; automatyczny zapis włączany osobno dla sejwu, po konkursie i po 15 sekundach od zmian, ze świadomym ręcznym zapisem.
- **Listy zawodników, 3/8:** pomocnicze limity krajów, które nie blokują wyboru; grupowanie krajami obok widoku kolejności, bez zmiany zapisanej kolejności. Limity mają cofanie i zapisują się w sejwie.

Pozostało 19 punktów: listy zawodników 1, 2, 5, 6 i 8; pierwsze uruchomienie 1; wybór zawodników przed konkursem 1–4; „Inne” 1–6; Pomoc 1; Konkursy 1–2. Niektóre punkty obejmują kilka zmian. Szeroka przebudowa list (1–2) jest rozpoczęta, ale nieukończona.

## Od czego zacząć: aktywna lista zawodników

Decyzje użytkownika: interfejs list przebudować od podstaw. Pokazywać obok
siebie wszystkich i wybranych zawodników, z szybkim dodawaniem i przeciąganiem;
domyślny widok ma grupować zawodników krajami. Ręczny wybór listy przed
konkursem obowiązuje dla tego konkursu i nie zmienia przypisania kalendarza.

W kodzie jest **niedokończony, celowo wyłączony** szkielet aktywnej listy. `SimulationSave::activeRosterFeatureEnabled()` zwraca `false`: wyboru nie pokazuje się użytkownikowi, starych sejwów nie migruje się i dotychczasowa logika wyboru zawodników nie zmienia się. Nie oznaczać tych punktów jako ukończonych i nie włączać flagi przed połączeniem całej ścieżki.

1. Dokończ i przetestuj `SimulationSave::activeJumpersListId`, `activeJumpers`, zapis identyfikatora oraz migrację starszych sejwów. Istniejący szkic migracji zachowuje poprzedni wynik operacji dołączania/wykluczania i, jeśli potrzeba, tworzy listę „Dotychczasowy skład”. Sprawdź puste listy, usunięcie aktywnej listy, duplikaty identyfikatorów, zapis i ponowne wczytanie. Brak wybranej zapisanej listy ma oznaczać **wszystkich zawodników**, a nie pusty skład.
2. Dokończ przełącznik w `SeasonRosterChoice.qml`, przeglądzie sezonu oraz ekranie list. Przywróć zaplanowaną nazwę pola edytora „Ustaw jako aktywną listę zawodników”; upewnij się, że zmiany, usuwanie i cofanie nie pozostawiają nieaktualnego wyboru.
3. W `CompetitionConfigWindow` użyj aktywnej listy w indywidualnych i drużynowych konkursach, zachowując zasady kwalifikacji i możliwość ręcznego dodania innych zawodników. Obecny kod nadal używa starego `is-default`; nie włączaj flagi, dopóki obie ścieżki nie są gotowe.
4. Dodaj opcjonalne przypisanie listy do kalendarza; przed jego konkursem przypisana lista ma stać się aktywna, a użytkownik ma widzieć skąd pochodzi i móc ją zmienić. Przetestuj stare sejwy, usunięcie listy oraz sezony z kilkoma kalendarzami.
5. Kontynuuj resztę pliku z pulpitu po kolei. Testuj reprezentacje i dowolne drużyny, kolejność oraz automatyczne kompletowanie zgodnie z opisem użytkownika. Limity krajowe są wyłącznie informacyjne i nigdy nie blokują wyboru.

## Weryfikacja i ważne ograniczenia

- Edytor limitów i grupowanie przeszły trzy równoległe powtórzenia testu, rzeczywiste wpisywanie i kasowanie limitu, cofanie, zachowanie wiersza/modelu, zapis, odczyt oraz test poprzedniego edytora składu. Sporadyczny crash przy przełączeniu grupowania naprawiono przez osobne modele normalnego i grupowanego widoku; ponowne powtórzenia przeszły.
- Autosejw przeszedł test przełącznika w UI, starego formatu, niezależności sejwów, odczytu ustawienia, opóźnienia i kalendarza. Zmiana przełącznika zapisuje bieżący stan, co wyjaśnia interfejs.
- Pełna regresja aplikacji **nie jest potwierdzona jako zielona**. `30-10-work.md` zawiera jeszcze pełny smoke oraz dalszą diagnostykę dawnego PB `212,5` + Enter. Ten konkretny pierwotny crash nie został odtworzony ani jednoznacznie zdiagnozowany; nie badaj ponownie prywatnych danych wskazanego zawodnika.
- Windows: `powershell -ExecutionPolicy Bypass -File build-tools/wind-form-incremental.ps1 -BuildDirectory build-win-smoke`, następnie `python scripts/test-quick-ui-windows.py --log build-tools/<nazwa>.log --timeout 240 -- --simulation-v2-season-only` lub `--roster-only`. Runner tworzy izolowaną kopię danych; `--renders <folder>` zachowuje zrzuty. Zwykły build: ten sam skrypt z `-BuildDirectory build-win`. Nie uruchamiaj dwóch buildów w tym samym katalogu.
- Po zmianach tekstów uruchom `python -X utf8 scripts/check-translations.py` i uzupełnij PL/EN/CS. Nie przedstawiaj testów ograniczonych do wybranych ścieżek jako pełnej regresji.
