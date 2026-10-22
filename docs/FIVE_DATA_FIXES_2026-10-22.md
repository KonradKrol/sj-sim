# Pięć kolejnych poprawek danych — 22.10.2026

Gałąź: `feature/qml-competition-config`. Ten zestaw jest kontynuacją wcześniejszych poprawek kopiowania kalendarzy i presetów; nie zastępuje zbiorczego `RELEASE_NOTES_VS_MAIN.md`.

## 1. Ułamkowe punkty znikały po ponownym odczycie presetu

**Objaw:** zapisany preset z punktami np. 12,5; 7,25; 0,5 po odczycie tracił wartości niecałkowite. Edytor i model obsługiwały liczby rzeczywiste, ale odczyt JSON nadal używał `toInt()`.

**Zmiana:** `PointsForPlacesPreset::getFromJson()` odczytuje `double`, zgodnie z typem mapy punktów. Zero pozostaje prawidłową wartością.

**Sprawdzenie:** regresja zapisu i odczytu pojedynczego presetu oraz całej kolekcji globalnej; przed poprawką test wykazał utratę danych. Test użytkownika: zapisać preset z ułamkowymi punktami, ponownie uruchomić aplikację i otworzyć preset.

Commit: `ccf8b0d`.

## 2. Eksport CSV zmieniał nazwiska i uszkadzał kolumny

**Objaw:** zamiana kropki dziesiętnej na przecinek obejmowała cały wiersz, więc `A. Kowalski` stawało się `A, Kowalski`. Średnik, cudzysłów lub nowa linia w nazwie rozbijały strukturę CSV. Nieznany kod reprezentacji nie miał użytecznej nazwy w eksporcie.

**Zmiana:** liczby formatują się oddzielnie od tekstu. Pola tekstowe zawierające separatory są cytowane, a cudzysłowy podwajane. Nieznana reprezentacja ma nazwę równą swojemu kodowi. Zachowano separator średnikowy, przecinek dziesiętny i dotychczasową kolejność kolumn.

**Sprawdzenie:** test nazwy z kropką, średnikiem, cudzysłowem i nową linią, ułamkowych punktów/odległości oraz nieznanego kodu kraju; przed poprawką test nie przechodził. Test użytkownika: wyeksportować wyniki zawodnika z kropką w nazwie i otworzyć CSV w arkuszu kalkulacyjnym.

Commit: `284360f`.

## 3. Wczytanie sezonu mogło odtwarzać niewłaściwy następny konkurs

**Objaw:** zapisany indeks następnego konkursu był uznawany za poprawny, jeśli mieścił się w kalendarzu. Mógł jednak wskazywać rozegrane zawody albo pomijać wcześniejszy nierozgrany konkurs.

**Zmiana:** po odczycie sezon ustala następne wydarzenie na podstawie faktycznych stanów wydarzeń w aktywnym kalendarzu. Ukończony kalendarz pozostaje ukończony nawet przy starym indeksie w pliku.

**Sprawdzenie:** pełny odczyt JSON z indeksem wskazującym rozegrany konkurs, dalszy konkurs, wartością ujemną i poza zakresem oraz ukończonym kalendarzem. Test przed poprawką wskazał odtwarzanie nieprawidłowego wydarzenia. Test użytkownika: rozegrać część sezonu, zapisać, wczytać i sprawdzić następne wydarzenie.

Commit: `1171608`.

## 4. Zmiana narodowości usuwała historyczne punkty z zawodów drużynowych

**Objaw:** indywidualna klasyfikacja oparta na punktach konkursowych porównywała kraj historycznej drużyny z bieżącą narodowością zawodnika. Po zmianie kraju znikały jego wcześniej zdobyte punkty.

**Zmiana:** historyczny udział dopasowuje się po rzeczywistym zawodniku zapisanym w wynikach skoków drużyny. Aktualny kraj nie usuwa tego udziału.

**Sprawdzenie:** zawodnik zdobywa 123,5 pkt w drużynie POL, następnie zmienia kraj na NOR; przeliczenie klasyfikacji zachowuje 123,5 pkt. Drugi zawodnik bez skoków nie otrzymuje cudzych punktów. Przed poprawką test wykazał wyzerowanie sumy. Test użytkownika: zmienić kraj zawodnika po drużynowych zawodach i sprawdzić indywidualną klasyfikację punktów konkursowych.

Commit: `5b9f147`.

## 5. Anulowanie konfiguracji następnego sezonu nie wycofywało zmian

**Objaw:** konfigurator pracował na wskaźnikach do aktywnych skoczków i skoczni. Anulowanie pozostawiało ich edycję w zapisie. Przy zatwierdzeniu nie przenoszono natomiast dodanych obiektów i zmian zasad. Edytory mogły nadal pokazywać dane globalne zamiast danych sezonu.

**Zmiana:** następny sezon przygotowuje się na osobnych kopiach roboczych. Anulowanie je odrzuca. Zatwierdzenie przenosi edycje i nowe obiekty, zachowując tożsamość istniejących skoczków/skoczni i odwołania historycznych kalendarzy. Nowi zawodnicy otrzymują ustawienia niestabilności formy. Settery odświeżają listy oraz edytory; zapis edycji zachowuje identyfikatory. Nie tworzy się zbędnych kopii bazy globalnej dla następnego sezonu. Edytor zasad zostaje odłączony przed zastąpieniem ich kolekcji.

**Sprawdzenie:** rzeczywiste otwarcie konfiguratora i anulowanie po zmianach, a następnie osobne otwarcie i zatwierdzenie zmian oraz nowych skoczków, skoczni i zasad. Test sprawdza tożsamości obiektów, rok sezonu i dane po obu operacjach. Przed poprawką test wykazał zmianę żywego zapisu mimo anulowania. Test użytkownika: w konfiguracji następnego sezonu zmienić zawodnika/skocznię i anulować; następnie powtórzyć, dodać obiekty i zatwierdzić.

Commity: `84e1729`, uzupełnienie bezpieczeństwa edytora zasad `cd3cdba`.

## Weryfikacja

Regresje 1–4 przechodzą w `scripts/test-quick-ui.sh --backend-only`, regresja 5 w `--workflow-only`. Każdą z pięciu usterek odtworzono testem przed zmianą produkcyjną. Końcowy pełny przebieg i zgodność Widgets opisuje zbiorczy wykaz zmian.
