# Pięć przebudowanych przepływów UX

Gałąź: `feature/qml-competition-config`. Zmiany dotyczą domyślnego interfejsu Qt Quick. Oryginalny interfejs Qt Widgets pozostaje dostępny przez `--widgets`.

## 1. Generator formy — jawne zastosowanie parametrów

**Problem:** zakończenie edycji pojedynczego parametru lub wczytanie presetu od razu nadpisywało ustawienia wszystkich zawodników. Trudno było przygotować zmianę tylko dla wybranych osób i przewidzieć moment jej zastosowania.

**Rozwiązanie:** sprawdzony wzorzec edycji zbiorczej: parametry są szkicem, osobny wybór „Wszyscy / Zaznaczeni” określa zakres, a „Zastosuj parametry” wykonuje zmianę. Przy niezastosowanym szkicu generowanie jest zablokowane, żeby nie uruchomić go ze starymi parametrami. Wczytany preset również wymaga zastosowania. Licznik wszystkich obejmuje pełną listę, niezależnie od wyszukiwania; zmiana liczby zaznaczonych nie przełącza zakresu na wszystkich.

**Obsługa:** zaznacz zawodników w tabeli, ustaw parametry lub wczytaj preset, wybierz zakres, kliknij „Zastosuj parametry”, następnie „Wygeneruj formę zawodników”. Bezpośrednia edycja komórek tabeli pozostaje dostępna. Na małym ekranie parametry mają własną zakładkę, a przyciski zastosowania i generowania pozostają przy dolnej krawędzi obszaru roboczego.

Zmiany C++ ograniczają się do jawnego zastosowania ustawień i ochrony przed uruchomieniem ze szkicem; algorytm losowania formy pozostaje ten sam. Widgets zachowuje dotychczasowe zachowanie edycji parametrów.

Commity: `e577e73`, `f9a0b90`, `5398f72`.

## 2. Tabele — widoczne wyszukiwanie i jednoznaczne zaznaczenie

**Problem:** wyszukiwanie ukryte za ikoną, brak bezpośredniego wyjścia z pustych wyników i niejasny zakres operacji zbiorczych. Kursor klawiatury wyglądał jak zaznaczenie, a wybranie komórki poza pierwszą kolumną nie podświetlało zawodnika.

**Rozwiązanie:** widoczne pole wyszukiwania w zwykłych tabelach, gdy pozwala na to miejsce; w mniejszych obszarach przycisk „Szukaj”. Ctrl+F kieruje do wyszukiwania aktywnej tabeli. Pusty wynik pokazuje „Brak wyników” i „Wyczyść filtr”, zamiast sugerować utratę danych. Zaznaczenie ma licznik i bezpośrednie czyszczenie. Zmiana filtra czyści zaznaczenie, żeby operacja zbiorcza nie objęła niewidocznych, wcześniej wybranych osób. Kursor ma osobny znacznik; wypełnienie wiersza oznacza faktyczne zaznaczenie choć jednej komórki tego wiersza.

**Obsługa:** wpisz tekst w wyszukiwanie lub użyj Ctrl+F; wyczyść filtr przyciskiem × albo akcją w pustym stanie. Po wyszukaniu zaznacz cele operacji. Licznik pokazuje liczbę wierszy, nie komórek. W niskich tabelach licznik trafia do nagłówka, żeby nie zabierać miejsca na dane.

Commity: `1611e61`, `8708819`.

## 3. Lista startowa — oddzielne akcje dla zapisanych składów

**Problem:** zastosowanie zapisanej listy wymagało przechodzenia przez zagnieżdżone menu. Dodanie zawodników, ich usunięcie i zastosowanie kolejności były słabo rozróżnione. Na najmniejszym ekranie nagłówki i paski zajmowały niemal cały obszar zawodników.

**Rozwiązanie:** wybór zapisanej listy i trzy bezpośrednie akcje: „Dodaj”, „Usuń”, „Kolejność”. Desktop pokazuje je w obszarze roboczym. Mały ekran ma przycisk „Listy” i jeden krótki panel z tymi samymi operacjami. Mniej powtarzalnych nagłówków oraz bardziej zwarty pasek pozostawiają miejsce na wiersze zawodników zarówno w składzie, jak i w bibliotece.

**Obsługa:** wybierz zapisany skład i cel operacji. „Kolejność” stosuje porządek listy; „Dodaj” i „Usuń” zmieniają jej członkostwo. Cofanie oraz ograniczenia kwalifikacji i rezerw pozostają obsługiwane przez istniejący backend.

Commity: `0679252`, `597e1dd`.

## 4. Konkurs — warunki w kontekście, opcje według zadania

**Problem:** długa lista drugorzędnych opcji utrudniała znalezienie warunków i kontroli skoku. Tryb na żywo wymagał szukania w opcjach, mimo że wpływa na podstawowy sposób prowadzenia konkursu.

**Rozwiązanie:** opcje pogrupowano w „Przebieg / Warunki / Skok”. Kliknięcie odczytu wiatru lub śniegu otwiera od razu „Warunki”, z generowaniem wiatru i ustawieniami generatorów. Przełącznik na żywo jest na głównym pasku. Wyniki, lista startowa, aktualny zawodnik i ostatni skok pozostają w głównym obszarze, poza zakładkami opcji. Pasek sterowania mieści się w jednym wierszu w szerokim oknie, aby zachować miejsce na tabele.

**Obsługa:** kliknij wiatr/śnieg, aby zmienić warunki; „Opcje” prowadzi do pozostałych narzędzi konkursu. Escape lub × zamyka panel. Akcje nadal wywołują oryginalne kontrolery konkursu.

Commity: `191138e`, `4a49261`.

## 5. Kalendarz — zaznaczanie oddzielone od nawigacji

**Problem:** kliknięcie konkursu od razu przenosiło do edycji na małym ekranie. Utrudniało to zaznaczenie kilku wydarzeń i rozpoznanie zakresu zmian.

**Rozwiązanie:** pojedyncze kliknięcie zaznacza i pozostawia listę na miejscu. Ctrl/Shift umożliwia wybór wielu wydarzeń. „Edytuj”, strzałka przy wydarzeniu, Enter lub dwuklik otwierają edycję. Licznik i czyszczenie wyboru są widoczne. Desktop zachowuje układ listy obok inspektora. Fokus wraca do właściwego wiersza po odświeżeniu modelu.

**Obsługa:** zaznacz wydarzenia, sprawdź licznik, kliknij „Edytuj”. Zmiany pól dotyczą wskazanej grupy. Ochrona rozegranej historii, kopiowanie bloków zawodów, cofanie i reguły poszczególnych wydarzeń pozostają zachowane.

Commity: `4e1c614`, `77811ae`.

## Testy

Nowe scenariusze znajdują się w `tests/quick-ui-smoke.cpp`, głównie w `uxRegressions`, z dodatkowymi sprawdzeniami w przebiegu konkursu i testach geometrii. Uruchomienie celowane: `scripts/test-quick-ui.sh --ux-only`; pełne: `scripts/test-quick-ui.sh`.

Testy sprawdzają rzeczywiste kliknięcia i klawiaturę, zakres zastosowania formy, ochronę przed generowaniem ze szkicem, zmianę filtra i pusty stan, liczbę zaznaczonych, operacje zapisanych list i cofanie, wybór kilku wydarzeń bez nawigacji, Enter do edycji oraz bezpośredni dostęp do trybu na żywo i generowania wiatru. Kontrole geometrii obejmują przypięte akcje generatora, użyteczny obszar zawodników i jednoczesną widoczność krytycznych danych konkursu na laptopie.

Końcowa walidacja:

- `scripts/test-quick-ui.sh --competitions-only`: zaliczony konkurs indywidualny, KO, drużynowy, zamknięcie podczas prezentacji na żywo i analiza sezonu.
- Pełny zestaw pod GDB oraz ponowny zwykły `scripts/test-quick-ui.sh`: oba zaliczone, każdy z **316 liczonymi renderami**, dodatkowymi wariantami powiększonego tekstu oraz scenariuszami rzeczywistych operacji. Zwykły przebieg zakończył się kodem 0. Rozmiary: 1440 × 900, 1024 × 640, 520 × 600 i 360 × 480; jasny i ciemny motyw.
- Kompilacja `make -j4` w `build-widgets-compat`: kod 0; `ldd` potwierdza brak zależności Qt Quick/QML w tym wariancie.
- Ekstrakcja `lupdate source/UI/Quick` i porównanie z katalogiem angielskim: żadnych brakujących tłumaczeń aktualnych ciągów Quick.
- `git diff --check`: bez błędów.

Sprawdzono wizualnie m.in. konkurs KO przy 1024 × 640, generator przy 1440 × 900 i parametry przy 360 × 480, zapisane składy na desktopie i bibliotekę zawodników w najmniejszym oknie, kalendarz oraz wyszukiwanie w bazie danych. Zrzuty pełnego zestawu są w `build-quick-ui/ui-renders/`.

### Ograniczenie diagnostyki

Pierwszy pełny przebieg po wznowieniu zakończył się komunikatem `stack smashing detected` podczas przechodzenia do testów sezonu. Osobny scenariusz sezonu pod GDB, kolejny pełny przebieg pod GDB oraz ponowny pełny przebieg bez debuggera przeszły poprawnie. Przyczyny tego jednorazowego przerwania **nie ustalono i nie uznaje się go za naprawiony błąd**. W razie ponownego wystąpienia potrzebny jest stos wywołań z tego samego przypadku. Wcześniejsza powtarzalna regresja geometrii konkursu KO została naprawiona w `4a49261` i przeszła oba końcowe pełne przebiegi.

Automatyczne testy korzystają z Qt 5.15.3 i renderera programowego offscreen; nie stanowią testu wszystkich systemowych menedżerów okien ani sterowników GPU. Historia przerwania zadania znajduje się w `UX_REWORK_CHECKPOINT_2026-10-21.md`.
