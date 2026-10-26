# Nowy model i edytor skoczka — 26.10.2026

## Gdzie znaleźć

**Baza danych → Zawodnicy → Nowy edytor.** Przełącznik „Klasyczny” przywraca dotychczasowy widok. Nowy edytor jest niezależnym miejscem przygotowania przyszłego modelu skoczka; jego dane nie są jeszcze używane przez konkursy, sezony ani playground.

## Trzy widoki

- **Karta:** sportowa karta zawodnika z OVR, inicjałami albo zdjęciem, krajem i rekordem; obok edycja danych, OVR, formy i lądowania, niżej perki w parach.
- **Profil:** szeroki nagłówek z nazwiskiem i OVR; parametry w jednej kolumnie, perki w drugiej. Układ nie potrzebuje zdjęcia; pojawia się ono dopiero po dodaniu.
- **Studio:** OVR i forma na początku, szeroki obszar do wybierania perków, dane osobowe i lądowanie niżej. Układ nastawiony na dostrajanie profilu.

Wybór widoku nie zmienia danych. Motywy, skala UI, powiększanie tekstu i ograniczenie animacji korzystają z obecnych ustawień Sj.Sima. Na wąskich ekranach lista i edytor są przełączane, a zawartość edytora przewijana. Brak zdjęcia jest normalnym stanem: karta ma inicjały, sportowy wzór i flagę.

## Nowe parametry

- Imię, nazwisko, kraj.
- Opcjonalne zdjęcie i rok urodzenia.
- Rekord życiowy w metrach, bez skoczni. Puste pole oznacza brak rekordu.
- OVR 1–100 i forma 1–100. Można przeciągać suwak lub wpisać liczbę.
- Lądowanie: Niechlujne, Niestaranne, Poprawne, Eleganckie, Stylista.
- Trzy niezależne pary perków: Błyszczy na treningach / Oszczędza siły; Lotnik / Specjalista małych skoczni; Regularny / Nieprzewidywalny.

Kliknięcie wybiera lub wyłącza perk; wybranie przeciwieństwa zastępuje poprzedni. Zwykłe kliknięcie nowego perka wybiera poziom I. Osobne przyciski I/II wybierają poziom bez wyłączania perka. Poziom II ma złotą ramkę, oznaczenie i wyróżniony przycisk. Zmiany można cofać i ponawiać. Nowy model nie zawiera dawnych „cech charakterystycznych”.

Opisy perków określają docelowe zachowanie; **efekty nie są jeszcze podłączone do symulacji**. W tym etapie nie zmieniono fizyki, losowania, punktacji, rozwoju sezonowego ani klasy `Jumper`.

## Paczki, zapis i zdjęcia

Nowe paczki mają osobny format `sj-sim-athlete-pack`, wersję 1 i identyfikatory UUID. Są zapisywane w systemowym katalogu danych aplikacji, w `athlete-studio/<UUID>/pack.json`. Klasyczne `userData`, paczki, zawodnicy i sezony pozostają poza tym zapisem.

Każda zmiana jest zapisywana atomowo przez QSaveFile; błąd zapisu pozostawia poprzedni model i pokazuje komunikat. Cofnij/Ponów obejmuje ostatnie 100 zmian bieżącej paczki w sesji edytora, w tym usuwanie profilu. Zdjęcia nie są usuwane przy cofnięciu, aby historia mogła je przywrócić.

Menu **Paczka…** udostępnia tworzenie i zmianę nazwy, skopiowanie klasycznego skoczka, import oraz eksport.

Zdjęcie można wybrać z folderu `jumpersImages` albo innego folderu. Edytor zapisuje własną kopię PNG o rozmiarze najwyżej 1600 px na bok, pod nazwą wyprowadzoną ze skrótu pliku. W profilu jest tylko ścieżka względna `portraits/<skrót>.png`. Eksport tworzy nowy folder z manifestem i zdjęciami. Do przeniesienia paczki skopiuj **cały folder**, nie sam `pack.json`. Import tworzy osobną kopię paczki; wcześniejszych danych nie nadpisuje.

## Kopiowanie klasycznego skoczka

Przed utworzeniem kopii widoczne jest wyjaśnienie:

- Imię, nazwisko, kraj, rok urodzenia, rekord i dostępne zdjęcie są kopiowane.
- OVR = średnia techniki wybicia i lotu, podzielona przez dotychczasowy zakres umiejętności × 100, zaokrąglona i ograniczona do 1–100.
- Forma jest przeskalowana przez ten sam zakres do 1–100.
- Lądowanie jest przybliżeniem: stara wartość / 5, zaokrąglona i ograniczona do pięciu stopni 0–4. Nie jest to obietnica identycznych not.
- **Perki zaczynają od zera.** Pozostałe stare parametry i cechy nie są przenoszone. Oryginał pozostaje nietknięty.

## Sprawdzenie

```bash
scripts/test-quick-ui.sh --athlete-studio-only
scripts/test-quick-ui.sh
```

Test dedykowany sprawdza zakresy i format danych, wykluczanie perków, I/II, Cofnij/Ponów, zachowanie przy błędzie zapisu, niezależność klasycznej bazy, kopiowanie, ponowne wczytanie, przenośność zdjęć, odmowę nadpisania eksportu, rzeczywiste kliknięcia i wpisywanie OVR, trzy widoki na 360/520/1024/1440 px, oba motywy i większy tekst.

Weryfikacja lokalna (Linux, Qt 5.15.3): dedykowane testy nowego modelu i interfejsu — PASS; pełny dotychczasowy zestaw — PASS, 407 renderów; budowa `CONFIG+=widgets_only` — PASS. Testy nowego edytora używają oddzielnych katalogów, bez modyfikowania rzeczywistych danych gracza. Windows/Qt 6 wymagają sprawdzenia na docelowym środowisku.

## Architektura

`source/athletes/AthleteProfile.*` — nowy model i walidacja. `AthleteStudioController.*` — osobne paczki, historia zmian, zdjęcia i kopiowanie. `Athlete*.qml` — UI i wektorowe ikony. Dotychczasowe katalogi otrzymują wyłącznie opcjonalny backend i przełącznik w sekcji zawodników; klasyczne edytory pozostają dostępne.
