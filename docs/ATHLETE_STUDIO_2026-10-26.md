# Nowy model i edytor skoczka — 26.10.2026

## Gdzie znaleźć

**Baza danych → Zawodnicy → Nowy edytor.** Przełącznik „Klasyczny” przywraca dotychczasowy widok. Nowy edytor jest niezależnym miejscem przygotowania przyszłego modelu skoczka; jego dane nie są jeszcze używane przez konkursy, sezony ani playground.

## Pięć koncepcji interfejsu

Wspólne pozostają dane i podstawowe reguły wyboru. Każda koncepcja ma własną kompozycję, paletę, typografię akcentową i oprawę perków:

- **Emblematy:** ciemny sportowy ekran z limonkowym OVR, metalowymi odznakami i widocznym zestawem przypisanych perków.
- **Naszywki:** papierowy, typograficzny profil z podkreślonymi polami, rdzawym akcentem i kolorowymi, zwartymi naszywkami w trzech parach. Ciemny motyw ma odpowiednio dobrane barwy.
- **Żetony:** zielony warsztat z trzema miejscami na przypisane perki i kolekcją sześciu żetonów; zaokrąglone pola, punktowe akcenty, monospaced OVR.
- **Karta:** kolekcjonerska karta ze złotą oprawą, zdjęciem albo inicjałami, edytowalnymi wartościami i odznakami przypiętymi do karty; obok dostępne perki.
- **Postać:** ekran zawodnika z autorską wektorową sylwetką narciarza, opcjonalnym zdjęciem i przypisanymi odznakami; fioletowa oprawa oraz kolekcja perków poniżej.

**Imię, nazwisko i kraj są na początku każdej wersji.** Lądowanie wybiera się z pięciu nazwanych opcji. Zawodnika wybiera się z wyszukiwalnej listy u góry; stała boczna lista nie zmniejsza obszaru projektów. Zmiana wersji zachowuje profil, parametry i perki.

Nie ma akapitów opisowych przy perkach. Wszystkie sześć symboli jest dostępnych od razu, w przeciwstawnych parach. Wybrane odznaki są dodatkowo przypisane do zawodnika, zależnie od koncepcji: na karcie, przy sylwetce albo w osobnych miejscach. Brak zdjęcia jest obsłużonym stanem, bez konieczności uzupełniania pustego portretu.

Skala UI, powiększanie tekstu i ograniczenie animacji korzystają z ustawień Sj.Sima. Zawartość edytora przewija się w ograniczonym obszarze. Nowe projekty mogą wizualnie odbiegać od reszty interfejsu — są pięcioma kierunkami do oceny.

## Nowe parametry

- Imię, nazwisko, kraj.
- Opcjonalne zdjęcie i rok urodzenia.
- Rekord życiowy w metrach, bez skoczni. Puste pole oznacza brak rekordu.
- OVR 1–100 i forma 1–100. Można przeciągać suwak lub wpisać liczbę.
- Lądowanie: Niechlujne, Niestaranne, Poprawne, Eleganckie, Stylista.
- Trzy niezależne pary perków: Błyszczy na treningach / Oszczędza siły; Lotnik / Specjalista małych skoczni; Regularny / Nieprzewidywalny.

Kliknięcie wybiera lub wyłącza perk; wybranie przeciwieństwa zastępuje poprzedni. Zwykłe kliknięcie nowego perka wybiera poziom I. Przycisk **I ↑** przy wybranej odznace wzmacnia ją do II; kliknięcie **II** przywraca poziom I. Poziom II ma złote ornamenty i osobne oznaczenie. Opis działania jest dostępny po najechaniu lub naciśnięciu **F1** na odznace; nie zajmuje miejsca w układzie. Przypisaną odznakę można odpiąć kliknięciem. Zmiany można cofać i ponawiać. Nowy model nie zawiera dawnych „cech charakterystycznych”.

Opisy perków określają docelowe zachowanie; **efekty nie są jeszcze podłączone do symulacji**. W tym etapie nie zmieniono fizyki, losowania, punktacji, rozwoju sezonowego ani klasy `Jumper`.

## Paczki, zapis i zdjęcia

Nowe paczki mają osobny format `sj-sim-athlete-pack`, wersję 1 i identyfikatory UUID. Są zapisywane w systemowym katalogu danych aplikacji, w `athlete-studio/<UUID>/pack.json`. Klasyczne `userData`, paczki, zawodnicy i sezony pozostają poza tym zapisem.

Każda zmiana jest zapisywana atomowo przez QSaveFile; błąd zapisu pozostawia poprzedni model i pokazuje komunikat. Cofnij/Ponów obejmuje ostatnie 100 zmian bieżącej paczki w sesji edytora, w tym usuwanie profilu. Zdjęcia nie są usuwane przy cofnięciu, aby historia mogła je przywrócić.

Menu **•••** udostępnia tworzenie i zmianę nazwy, skopiowanie klasycznego skoczka, import oraz eksport.

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

Test dedykowany sprawdza zakresy i format danych, wykluczanie perków, I/II, Cofnij/Ponów, zachowanie przy błędzie zapisu, niezależność klasycznej bazy, kopiowanie, ponowne wczytanie, przenośność zdjęć, odmowę nadpisania eksportu, rzeczywiste kliknięcia i wpisywanie OVR, pięć koncepcji na 360/520/1024/1440 px, oba motywy, większy tekst, zdjęcia i brak zdjęć. Dla każdej koncepcji sprawdza także kolejność danych osobowych, wybór odznaki, poziom II, zastąpienie przeciwieństwem, pomoc F1, brak opisów w widocznym układzie i szerokość trzycyfrowego OVR.

Weryfikacja lokalna (Linux, Qt 5.15.3): przebudowane pięć koncepcji — dedykowane testy PASS, 51 renderów. Wcześniejsza weryfikacja wspólnego modelu i integracji: dedykowane testy nowego modelu i interfejsu — PASS; pełny dotychczasowy zestaw — PASS, 407 renderów; budowa `CONFIG+=widgets_only` — PASS. Testy nowego edytora używają oddzielnych katalogów, bez modyfikowania rzeczywistych danych gracza. Windows/Qt 6 wymagają sprawdzenia na docelowym środowisku.

## Architektura

`source/athletes/AthleteProfile.*` — nowy model i walidacja. `AthleteStudioController.*` — osobne paczki, historia zmian, zdjęcia i kopiowanie. `Athlete*.qml` — UI i wektorowe ikony. Dotychczasowe katalogi otrzymują wyłącznie opcjonalny backend i przełącznik w sekcji zawodników; klasyczne edytory pozostają dostępne.
