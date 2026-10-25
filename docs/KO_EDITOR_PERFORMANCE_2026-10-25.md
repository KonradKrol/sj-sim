# Edytor KO: flagi, wyszukiwanie i przeciąganie

## Koszt flag podczas odświeżania

Poprzednie usunięcie podwójnego nadpisywania węzła KO nie usuwało kosztu przygotowania obrazków. Dla każdego zawodnika przy każdym odświeżeniu wczytywano flagę do QPixmap, po czym `quickPicture()` kodowało obraz PNG/base64. Zmieniane klucze pixmap i presja na cache sprawiały, że ta ścieżka pozostawała kosztowna również przy niezmienionej liście.

KO przekazuje teraz lokalne adresy SVG. QML ładuje flagi w rozmiarze wyświetlania i korzysta z własnego cache; bridge buforuje adresy i obsługuje brak pliku pustym obrazem. Nie zmieniono flag ani algorytmów gry.

Pomiar lokalny na tej samej liście 145 zawodników, 20 jawnych odświeżeń mostu: **3940 ms przed zmianą → 172 ms po zmianie** (około 96% mniej czasu, 23 razy krótsze odświeżanie tego scenariusza). Sama zmiana flag, przed przebudową wybierania zawodnika, dała 168 ms. To pomiar ścieżki odświeżania, nie FPS ani przyspieszenie całej gry. Nie używa się pomiaru jako sztywnego progu testu, zależnego od sprzętu.

## Wybór i zamiana zawodników

- Każdy zawodnik ma lekki wiersz z flagą, nazwą, uchwytem przeciągania i strzałką wyboru.
- Zamiast pełnego ComboBox z całą pulą w każdym miejscu działa jeden współdzielony picker otwierany na żądanie.
- Wyszukiwanie po nazwie i nazwisku ignoruje wielkość liter, obsługuje kilka słów, pusty wynik, strzałki, Enter oraz Escape. Filtr zachowuje oryginalny indeks zawodnika w puli.
- Wybranie osoby zamienia ją z dotychczasowym zawodnikiem w danym miejscu.
- Przeciąganie za uchwyt także zamienia zajęte miejsca, bez przesuwania innych zawodników. Wskazuje cel, pokazuje nazwę przeciąganej osoby i przewija listę przy krawędzi lub kółkiem.
- Gest jest obsługiwany poza recyklingowanym wierszem grupy, dzięki czemu przewijanie nie gubi przeciąganej osoby. Escape anuluje operację.

## Weryfikacja

`--ko-groups-only` oraz `--ko-groups-only --ko-stress` obejmują odpowiednio 31 i 145 zawodników, wielokrotne losowania, nieparzystą liczbę zawodników, stabilność dropdowna, wpisanego wyszukiwania i węzła podczas okresowych odświeżeń, pusty wynik wyszukiwania, wybór Enterem i rzeczywiste przeciągnięcie między grupami. Scenariusz renderuje oba motywy w różnych rozmiarach, w tym 360 px i powiększony tekst. Nie wykryto błędów QML ani rekurencyjnych layoutów.

Dodatkowy test rzeczywistym gestem potwierdza przewijanie przy krawędzi po opuszczeniu pierwotnego wiersza oraz anulowanie Escape bez zmiany składu grup. Końcowy powtórzony pomiar dużej listy: 164 ms / 20 odświeżeń.
