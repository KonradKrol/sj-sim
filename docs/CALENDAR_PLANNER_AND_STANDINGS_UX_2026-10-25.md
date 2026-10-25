# Kalendarz, planowanie weekendów i klasyfikacje — 25.10.2026

Gałąź: `feature/qml-competition-config`. Zachowano Qt Widgets i oznaczenie `2.0.0-beta`.

## Uzupełnienie — prostszy dostęp do terminów i automatyczny podgląd weekendu

Panel ma stały tytuł **Zaplanuj terminy**. Dwa widoczne przyciski rozdzielają działania: **Ustal terminy** dotyczy dat istniejących konkursów, a **Dodaj weekend** tworzy nowe wydarzenia. Pod wyborem jest odpowiedni nagłówek i krótki opis zakresu działania.

Kreator weekendów nie ma już przycisku „Podgląd weekendu”. Plan odświeża się po otwarciu i po zmianie daty, skoczni, zasad albo wariantu kwalifikacji. Przed zatwierdzeniem niczego nie dodaje. Błędna data usuwa nieaktualny plan i blokuje dodanie; poprawienie daty automatycznie przywraca podgląd. Po dodaniu przygotowany jest już podgląd kolejnego tygodnia. Cofnięcie lub zewnętrzne odświeżenie kalendarza nie pozostawia nieaktualnego planu.

Opcja niedzielna nazywa się **Bierz kwalifikacje z piątku + rozegraj serię próbną**. Jeśli pierwsze kwalifikacje przeniesiono na sobotę, tekst wskazuje sobotę; wybór wspólnych kwalifikacji pozostaje zachowany. Długi opis zawija się w polu i menu przy mniejszej szerokości.

Weryfikacja po tej zmianie: `scripts/test-quick-ui.sh --dates-only` — PASS, również automatyczne aktualizacje, zmiana piątek/sobota bez zgubienia wariantu, brak zmian przed dodaniem, błędna/poprawiona data, podgląd następnego tygodnia oraz responsive render w jasnym/ciemnym motywie i powiększonym tekście. Brak komunikatów o pętlach układu w tym przebiegu. Planowanie dat istniejących wydarzeń nadal ma osobne zatwierdzenie planu.

## Automatyczny zapis zasad

W edytorze konkursu zmiany zasad zapisują się bez przycisku „Zastosuj zasady”. Dotyczy to także treningów, serii próbnej i pól nowo dodanych serii. Zmiana konkursu, kalendarza lub celu edycji najpierw kończy oczekujący zapis. Opóźnione wywołanie poprzedniego edytora nie może anulować zapisu w nowym. Wczytywanie formularza i identyczne wartości nie tworzą dodatkowych kroków historii. Cofnij/Ponów oraz blokady rozegranych wydarzeń pozostają aktywne.

## Powiązania kwalifikacji

Wybór klasyfikacji i konkursu źródłowego filtruje rodzaj rywalizacji: indywidualny lub drużynowy. Indeksy przefiltrowanej listy wskazują właściwe obiekty, również gdy niekompatybilna klasyfikacja była wcześniej na liście. Zbiorczy wybór różnych typów nie proponuje niezgodnego źródła. Zachowane powiązania oczekujące nadal wymagają prawidłowej kolejności i rodzaju konkursu.

## Klasyfikacje i wkład kraju

- Tabele klasyfikacji nie oferują zaznaczania, zaznaczania wszystkich ani sortowania nagłówkiem. Pozostaje otwieranie szczegółów kraju dwuklikiem.
- Wyszukiwanie w klasyfikacji jest domyślnie schowane i otwierane na życzenie.
- Punkty są formatowane lokalnie: co najmniej jedna cyfra po separatorze, maksymalnie sześć; zbędne zera i artefakty precyzji są usuwane. Wartość używana w obliczeniach nie jest zaokrąglana.
- Podgląd wkładu kraju ma własny ekran: flaga, kraj, nazwa klasyfikacji, wyraźna suma i prosta tabela bez wyszukiwania. Brak danych otrzymał pusty stan.
- Podpowiedź filtra wkładu jest krótsza i zawiera przykład.

## Planowanie dat istniejących konkursów

Ścieżka: **Kalendarz → Zaplanuj terminy**.

Wyraźnie wybiera się zaznaczone albo wszystkie nierozegrane konkursy oraz datę pierwszego wydarzenia. Każdy rytm ma krótki przykład. Pole liczby dni pojawia się wyłącznie przy rytmie „Co kilka dni”. Termin treningów ma nazwane warianty zamiast nieopisanego pola liczbowego.

Dodano opcję **Tylko brakujące daty**, zachowującą istniejące terminy. Podgląd zmian poprzedza zapis. Przesuwanie zaznaczonych dat o dzień lub tydzień ma osobną sekcję; usuwanie dat i pozostałe rzadsze akcje są w menu.

## Dodawanie kompletnych weekendów

Ścieżka: **Kalendarz → Zaplanuj terminy → Dodaj weekend**. Funkcja tworzy nowe wydarzenia; planowanie już istniejących pozostaje w sekcji „Terminy”. Wybiera się datę, skocznię, preset zasad i dwa warianty kwalifikacji. Data jest dopasowana do piątku przypadającego w wybranym dniu lub później.

| Pierwsze kwalifikacje | Niedziela | Piątek | Sobota | Niedziela |
| --- | --- | --- | --- | --- |
| Piątek | Osobne kwalifikacje | 2 treningi i kwalifikacje | Seria próbna i konkurs | Kwalifikacje i konkurs |
| Piątek | Wspólne kwalifikacje | 2 treningi i kwalifikacje | Seria próbna i konkurs | Seria próbna i konkurs |
| Sobota | Osobne kwalifikacje | Bez wydarzeń | Kwalifikacje i konkurs | Kwalifikacje i konkurs |
| Sobota | Wspólne kwalifikacje | Bez wydarzeń | Kwalifikacje i konkurs | Seria próbna i konkurs |

Podgląd pokazuje trzy dni przed utworzeniem. Powiązania kwalifikacji ustawiają się automatycznie. Oba konkursy główne zachowują wybrany preset zasad; kwalifikacje i sesje pomocnicze mają jedną serię. Klasyfikacje można potem przypisać obu zaznaczonym konkursom w Powiązaniach. Całość tworzy jeden krok Cofnij/Ponów. Po dodaniu data następnego weekendu przesuwa się o tydzień. Zmiana skoczni lub presetu po podglądzie wymaga ponownego podglądu.

## Weryfikacja

- Testy logiki czterech wariantów weekendów: przełom roku, treningi i serie próbne, źródła kwalifikacji, brak zmian przed zatwierdzeniem, atomowe Cofnij/Ponów.
- Test rzeczywistego formularza: podgląd, utworzenie, zaznaczenie obu konkursów i cofnięcie; obrazy w jasnym/ciemnym motywie i małych rozmiarach oraz przy powiększonym tekście.
- Testy automatycznego zapisu zasad, szybkiego przełączania konkursów, dynamicznych pól nowych serii i cofania.
- Testy zgodności źródeł kwalifikacji, formatowania w polskiej/angielskiej lokalizacji i podglądu wkładu bez zaznaczania/wyszukiwania.

Uruchomione polecenia: `scripts/test-quick-ui.sh --dates-only`, `--workflow-only`, `--classifications-only` oraz kompilacja `build-widgets-compat`. Wszystkie zakończyły się powodzeniem. Testy uruchamiano poza danymi użytkownika, z programowym renderowaniem Qt Quick.

Pełny `scripts/test-quick-ui.sh`: **PASS, 407 renderów responsywnych** oraz scenariusze konkursów i sezonów. Log: `/tmp/sj-full-weekend-final.log`. W środowisku offscreen wystąpiły także komunikaty wtyczki o `propagateSizeHints`, pustych pixmapach i pięć komunikatów `Polish loop detected` podczas zmian układu; wynik PASS nie oznacza całkowicie czystego logu Qt. Oddzielne testy dat i klasyfikacji przeszły bez komunikatów o pętlach układu.
