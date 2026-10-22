# Największy awans na pasku konkursu

Gałąź: `feature/qml-competition-config`; `main` bez zmian. Wersja aplikacji: 1.3.0 beta-3.

## Zachowanie

Na górnym pasku zamiast lidera jest największy awans względem końca poprzedniej serii, np. „Największy awans: Jan Kowalski ↑12”. Przy węższym oknie etykieta skraca się do „Awans”. Numer serii, flaga i skocznia, wiatr oraz śnieg pozostają dostępne.

- W pierwszej serii i przed pierwszym rzeczywistym awansem pole nie dodaje pustego komunikatu.
- Kandydatem jest zawodnik, który oddał skok w bieżącej serii. Poprzednie miejsca są obliczane na całej stawce po poprzedniej serii, z remisami 1,1,3. Bieżące miejsce odpowiada aktualnej tabeli wyników i do końca serii jest prowizoryczne.
- Przy takim samym największym awansie wybierany jest zawodnik z wyższym bieżącym miejscem, następnie według nazwy. „(+N)” sygnalizuje pozostałych; podpowiedź po najechaniu pokazuje wszystkich oraz miejsca przed/po.
- W drużynówce podstawą są sumy pełnej poprzedniej serii drużyny, a bieżące miejsca aktualizują się także w kolejnych grupach.
- Prezentacja na żywo zachowuje poprzedni wskaźnik w etapach lotu, odległości, lądowania i not. Zmiana pojawia się wraz z ujawnieniem wyniku.
- To statystyka do odczytu. Nie modyfikuje punktacji, wyników, kolejności startu ani zasad awansu do następnej serii.

## Do sprawdzenia jako użytkownik

1. Rozegraj co najmniej dwie serie. W drugiej obserwuj „Największy awans” zamiast informacji o liderze.
2. Włącz „Na żywo”: wskaźnik nie powinien zdradzać wyniku podczas lotu.
3. Najedź na statystykę: sprawdź miejsca poprzednie i bieżące, a przy remisie także pozostałych zawodników.
4. Sprawdź drużynówkę i mniejsze okno. Na końcu kliknij „Zakończ konkurs” — powrót do konfiguracji ma działać bez crasha naprawionego w `145738d`.

## Weryfikacja

- `scripts/test-quick-ui.sh --competitions-only`: kod 0. Pełne konkursy indywidualne, KO, drużynowe, bezpieczne przerwanie prezentacji na żywo oraz analiza sezonu.
- Testy statystyki: puste wyniki, pierwsza seria, brak skoku w nowej serii, jednakowe awanse, remisy miejsc, porównanie trzeciej serii z drugą i pełna poprzednia seria drużynowa.
- Regresja ujawnienia na żywo sprawdza zamrożenie wskaźnika; końcowy kontekst porównywany jest ze statystyką wyników. Pasek nie zawiera już kontekstu lidera.
- Konkursy kończą się przez kliknięcie widocznego przycisku, po czym test sprawdza zniszczenie okna i jego adapterów przed zwolnieniem danych.
- Widoki konkursów przechodzą dotychczasowe rozmiary i motywy; sprawdzony zrzut `build-quick-ui/ui-renders/live-results-0-1024-0.png` pokazuje statystykę przy 1024 px.
- Końcowa kompilacja Widgets-only: kod 0. `git diff --check` przechodzi.
