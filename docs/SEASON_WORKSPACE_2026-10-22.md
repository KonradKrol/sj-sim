# Sezony: kalendarze, klasyfikacje i konfiguracja konkursu

Zmiany na `feature/qml-competition-config`, bez modyfikacji `main`. Wersja: **1.3.0 beta-3**. Interfejs Widgets pozostaje dostępny.

## Lista do sprawdzenia jako użytkownik

1. **Kalendarze:** otwórz sezon i kartę Kalendarz. Na szerokim ekranie kalendarze mają własną listę po lewej; w mniejszym oknie — rozwijaną listę. Kliknij „Nowy kalendarz”: pojawia się od razu, jest wybrany i można wpisać nazwę. Dodaj konkurs lub wczytaj preset.
2. **Edycja i rozgrywanie:** wybierz inny kalendarz do edycji. Aktywny kalendarz i dolny pasek następnego wydarzenia nie zmieniają się. „Ustaw jako aktywny” jest osobnym przyciskiem; na liście widać oznaczenie aktywnego kalendarza. Przy bardzo małej wysokości okna operacje kalendarza są dostępne w menu „⋯”.
3. **Wiele konkursów:** „Dodaj wiele” pozwala utworzyć do 100 konkursów w jednej operacji. Nowe konkursy są zaznaczone razem. Zmień im skocznię, liczbę treningów, serię próbną, zasady lub klasyfikacje; zmiany dotyczą całego zaznaczenia. Ctrl/Shift i zaznaczanie przyszłych zawodów pozostają dostępne.
4. **Przeciąganie:** użyj uchwytu po lewej stronie wiersza. Przenosi konkurs wraz z jego treningami i serią próbną. Jeżeli przeciągany konkurs należy do wielokrotnego zaznaczenia, przenosi się cały zestaw, z zachowaniem kolejności. Linia wskazuje miejsce upuszczenia; lista przewija się przy krawędzi. Alternatywa klawiaturowa: Ctrl+↑/↓.
5. **Kwalifikacje:** ustaw źródło kwalifikacji, a następnie przestaw konkurs przed to źródło. Oba wiersze pokazują problem. Powiązanie jest zachowane jako oczekujące, a start zależnego konkursu jest zablokowany. Przywróć poprawną kolejność: powiązanie uaktywnia się automatycznie. Zapis i ponowne wczytanie również zachowują oczekujące powiązanie. Można świadomie zmienić źródło lub wybrać „Brak”.
6. **Usuwanie źródeł:** spróbuj usunąć konkurs używany przez inne zawody jako źródło kwalifikacji. Komunikat wskaże numery zależnych konkursów; najpierw trzeba zmienić ich powiązania. Dotyczy to także powiązań oczekujących.
7. **Cofnij/Ponów:** wykonaj kilka zmian masowych, przestawień i zmian nazwy, po czym cofaj i ponawiaj kolejne operacje. Historia obejmuje także tworzenie/usuwanie kalendarzy, aktywowanie innego kalendarza i zastąpienie wydarzeń presetem. Cofnięcie wraca do kalendarza, którego dotyczyła operacja. Historia nie może nadpisać rozegranych w międzyczasie zawodów.
8. **Klasyfikacje:** w wybranym kalendarzu przejdź do sekcji Klasyfikacje. Dodaj klasyfikację, nazwij ją, wybierz typ i punktację, wczytaj preset punktów i edytuj wartości. Nie ma osobnego przycisku zatwierdzania klasyfikacji: pola i tabela aktualizują dane bez dodatkowego kliknięcia. Obsługiwane są punkty ułamkowe i przecinek dziesiętny.
9. **Użyta klasyfikacja:** nazwa pozostaje edytowalna, punktacja i typ są zablokowane. „Utwórz kopię” tworzy nową klasyfikację bez historycznych wyników, z własną tożsamością i edytowalną punktacją. Zmiana aktywnego kalendarza od razu aktualizuje klasyfikację w Przeglądzie, także do stanu pustego.
10. **Dolny pasek:** poza Przeglądem pokazuje pełną flagę kraju skoczni, nazwę, rodzaj wydarzenia, HS i format indywidualny/drużynowy. Przycisk korzysta z aktualnego stanu aktywnego kalendarza. Dla pustego kalendarza prowadzi do jego edycji.
11. **Dialogi:** sprawdź potwierdzenia Tak/Nie, przyciski Anuluj/Zapisz oraz krótkie komunikaty. Krótka treść korzysta z szerokości dialogu i jest poniżej nagłówka. Długie treści przewijają się nad stale dostępnymi przyciskami. Angielski tryb aplikacji zachowuje angielskie tłumaczenia.
12. **Belka:** wejdź w konfigurację konkursu — pole od razu pokazuje konkretny numer wyliczonej belki. W pojedynczym konkursie wybór skoczni przelicza ten numer. W sekcji Wiatr zmień warunki: sugestia jest przeliczana i pokazuje różnicę względem ustawionej belki. „Zastosuj” wpisuje nowy numer; przycisk Auto również wpisuje wyliczoną liczbę, zamiast tekstu „Auto”.
13. **Górny pasek konkursu:** flaga przy skoczni, wiatr dodatni zielony, ujemny czerwony, zero neutralne. Zamiast „Start x/N” jest lider z punktami; w najwęższym układzie pozostają jego punkty. Wynik nowego lidera nie pojawia się przed zakończeniem prezentacji na żywo.
14. **Miejsce podczas lotu:** karta ostatniego skoku nie pokazuje miejsca po poprzedniej serii. Miejsce jest puste do ujawnienia aktualnego wyniku. Poprzednia pozycja pozostaje w karcie zawodnika na belce.

Zmiany kalendarza zapisują się od razu w danych otwartego sezonu. Przycisk „Zapisz do pliku” zapisuje cały stan na dysku.

## Poprawki techniczne istotne dla spójności

- Oddzielne wskazanie kalendarza edytowanego i aktywnego; edytory, lista klasyfikacji i blokady historii korzystają z właściwego kalendarza.
- Jeden przebieg aktualizacji następnego konkursu i widoków; pola liczbowe, listy wyboru i pola tekstowe zgłaszają zmiany adapterowi przez sygnały, zamiast czekać wyłącznie na okresowe odświeżanie.
- Tożsamość konkursów i klasyfikacji pozostaje stała podczas zmian kolejności oraz Cofnij/Ponów. Historia przechowuje obiekty potrzebne do odtworzenia i chroni rozegrane wyniki także przy pracy z innym kalendarzem.
- Oczekujące źródło kwalifikacji jest zapisywane niezależnie od aktywnego powiązania. Normalizacja przywraca je po zmianie kolejności; presety również przenoszą te odwołania na własne obiekty.
- Tabele punktacji faktycznie zmieniają dane między powiadomieniami o dodawaniu/usuwaniu wierszy. Wymiana presetu/reset mają poprawne powiadomienia modelu; wartości nie są obcinane do liczb całkowitych. Usuwanie nieważnych przypisań klasyfikacji nie modyfikuje kolekcji podczas iteracji po jej elementach.
- Podgląd belki jest deterministyczny i ograniczony zakresem pola. Korzysta z obliczeń odległości silnika bez losowania skoku, lądowania i not, bez zmiany wyników oraz bez nieograniczonej pętli prób.

## Weryfikacja

- Pełny `scripts/test-quick-ui.sh`: **kod 0, 350 renderów**. Konkursy indywidualne, KO i drużynowe, narzędzia sezonu, nawigacja i archiwum przechodzą cały przebieg.
- Rozmiary od 360 × 480 do 2560 × 1440, oba motywy oraz powiększony tekst. Osobny przegląd nowego edytora klasyfikacji obejmuje też jego edycję przy najmniejszym oknie.
- Rzeczywiste przeciągnięcie myszą przenosi dwa zaznaczone konkursy z zachowaniem kolejności. Testy sprawdzają kompletność treningów/serii próbnej po zmianach masowych i po Cofnij/Ponów.
- Źródła kwalifikacji: zawieszenie, automatyczne przywrócenie, zapis/odczyt z użyciem magazynu tożsamości, blokada przycisku i bezpośredniego uruchomienia zależnego konkursu.
- Aktywny/edytowany kalendarz, natychmiastowa zmiana klasyfikacji w Przeglądzie, ochrona później rozegranych zawodów, cofanie/ponawianie usunięcia kalendarza i zastąpienia presetem.
- Test kontraktu modelu punktacji (`QAbstractItemModelTester`) podczas edycji i zmiany klasyfikacji; zachowanie wartości 12,5 zamiast obcięcia do 12; bezpieczna zmiana nazwy i kopia użytej klasyfikacji.
- Automatyczny dobór belki: liczbowy numer domyślny, deterministyczne sugestie, zależność od kierunku wiatru i przycisk zastosowania. Zmiana wiatru pozostawia ustawioną belkę do świadomego zastosowania sugestii.
- Dialogi: polskie Tak/Nie, pełna szerokość krótkiej treści poniżej tytułu oraz widoczne działające przyciski długich komunikatów po wymuszonych zmianach geometrii. Szybkie kroki i przytrzymanie pól liczbowych nadal przechodzą testy.
- Prezentacja na żywo: brak numeru miejsca i straty w trakcie lotu, ujawnienie końcowego miejsca po wyniku oraz zachowana poprzednia pozycja zawodnika/drużyny na belce.
- Końcowa kompilacja Widgets-only: **kod 0**, bez zależności Qt Quick/QML.
- Brak brakujących angielskich tłumaczeń nowych ciągów Quick; `git diff --check` przechodzi. Końcowy log nie zawiera błędów typów, odwołań ani pętli powiązań QML.

Testy działają na Qt 5.15.3, z rendererem programowym offscreen. Nie obejmują wszystkich menedżerów okien i sterowników GPU. Zrzuty: `build-quick-ui/ui-renders/`.

## Commity implementacyjne

- `284a89e` — polskie standardowe akcje dialogów i poprawny pomiar tekstu.
- `e021c7b` — przebudowa sezonów, klasyfikacji, historii, przeciągania, kwalifikacji i dolnego paska.
- `33ff226` — Auto oraz sugestie belki zależne od wiatru.
- `da0924b` — górny pasek konkursu i puste miejsce w trakcie lotu.

## Korekta: liczbowy numer belki

Pole belki pokazuje wyliczony numer, bez specjalnej wartości tekstowej „Auto”. Przycisk automatycznego doboru wpisuje liczbę. W pojedynczym konkursie numer przelicza się po wyborze skoczni; w sezonie jest ustawiany po wczytaniu danych wydarzenia. Nowa sugestia w sekcji Wiatr pozostaje do zastosowania przez użytkownika, a różnica jest liczona względem aktualnego numeru.

Weryfikacja korekty: `scripts/test-quick-ui.sh --workflow-only` — kompilacja aplikacji i testów oraz regresje sezonów, liczbowej belki, kierunku wiatru i zastosowania sugestii. Wcześniejsza pełna weryfikacja 350 renderów dotyczy stanu przed tą korektą.
