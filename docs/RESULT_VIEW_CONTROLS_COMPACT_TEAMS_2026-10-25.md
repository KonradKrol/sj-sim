# Sterowanie wynikami, wydajność podglądu i lista drużyn

## Przełączanie wyników bez zabierania wysokości tabeli

Wybór serii, klasyfikacji drużynowej/indywidualnej i sposobu rozwijania drużyn został przeniesiony do istniejącego paska aplikacji. Na szerokim ekranie wybory są dostępne bezpośrednio; w węższym oknie otwiera je przycisk „Wyniki”. Menu jest renderowane w obszarze strony, więc nie jest obcinane wysokością osobnego widoku nagłówka. Wybór zamyka menu.

Nagłówek tabeli zawiera teraz sam tytuł. Nie dochodzi dodatkowy wiersz kontrolek nad wynikami, a wysokość paska aplikacji pozostaje dotychczasowa. Dla małych ekranów etykiety nawigacji ustępują miejsca istotnym akcjom z zachowaniem nazw dostępności.

Po wybraniu historycznej serii pozostają kolumny, lista startowa, bieżący zawodnik i sterowanie w swoich miejscach. Elementy bieżącego przebiegu są nieaktywne i przygaszone, a tabela pokazuje wybrane wyniki. Zarezerwowanie poprzedniej wysokości kart ogranicza przesuwanie tabeli przy krótszej karcie historycznej. Wyraźny przycisk **Wróć do konkursu** przywraca „Teraz”; pojawia się wyłącznie w podglądzie.

Dopóki nie zapisano pierwszej próby następnej serii, ostatnia zakończona seria nie jest dublowana jako dodatkowy wybór. Po jednoseryjnym konkursie zostaje „Teraz”. W archiwum nadal można jawnie wybrać wszystkie zapisane serie.

## Przyczyna wielosekundowego opóźnienia

Widoczny `QStandardItemModel` był czyszczony i uzupełniany wiersz po wierszu. Każda taka zmiana powodowała pełny reset adaptera Quick oraz odtworzenie delegatów tabeli. Dochodziły sygnały rozwijania grup i drugi przebieg budowy podczas odświeżania mostu.

Nowy model jest przygotowywany poza widokiem. Po wypełnieniu następuje pojedyncza podmiana wraz ze stanem rozwinięcia drzewa; poprzedni model i model zaznaczenia są zwalniane bezpiecznie przez `deleteLater()`. Zapamiętanie klucza stanu podczas zmiany wyboru usuwa drugi zbędny przebieg. Kliknięcia dotyczące zastąpionego modelu są odrzucane. Tę samą metodę zastosowano w historii klasyfikacji sezonowych.

Pomiar tej samej sceny Qt Quick, 40 drużyn, 160 zawodników i 3 serie:

| Zmiana | Przed | Po | Resety adaptera przed → po |
| --- | ---: | ---: | ---: |
| Wyniki po pierwszej serii | 22 348 ms | 99 ms | 884 → 1 |
| Drużyny → indywidualnie | 14 249 ms | 104 ms | 644 → 1 |

Czasy obejmują stałe 40 ms oczekiwania testu na obsługę UI. To pomiar wskazanej operacji lokalnie, nie FPS ani przyspieszenie całej gry. Regresja wymaga najwyżej dwóch resetów przy przełączeniu, bez niestabilnego progu czasowego zależnego od sprzętu.

## Lista drużyn przed konkursem

- Jedna kompaktowa pozycja na drużynę: flaga, nazwa i skrócony skład w drugim polu. Pełny skład jest dostępny również w podpowiedzi.
- Nie ma rozwiniętych osobnych wierszy każdego skoczka ani sortowania nagłówkami.
- Skład nie jest edytowalny w samej liście. „Skład”, Enter lub podwójne kliknięcie otwierają dotychczasowy edytor drużyny.
- Usuwanie i przesuwanie drużyn pozostają w prostym pasku listy. Znikają powielone instrukcje dawnego drzewa.
- Wyszukiwanie otwiera się na żądanie i znajduje drużynę także po nazwisku członka składu. Można je ponownie zamknąć i wyczyścić filtr.

## Wyszukiwanie tylko z konkretnym zastosowaniem

Wspólny komponent tabeli domyślnie nie ma wyszukiwania. Pozostaje ono jawnie włączone w katalogach zapisów i danych, generatorze formy, klasyfikacjach oraz listach wyboru zawodników/skoczni i dużym archiwum zawodów. Lista drużyn udostępnia je na żądanie. Nie pojawia się automatycznie w tabelach wyników skoków, konkursu, KO ani liczbowych ustawień i punktacji.

## Oznaczenia i teksty

- Próby DSQ otrzymują jawne „DSQ”, analogicznie do DNS: w tabeli indywidualnej, seriach zawodników drużyny, podglądach i karcie skoku.
- Miniwyniki grupy KO zachowują sumę punktów i dodają znacznik bieżącej próby DNS/DSQ. Nie przenoszą go z dawnej serii na zawodnika oczekującego na bieżący skok.
- Informacja przed skokiem brzmi **Traci X pkt do lidera**. Reguły przeliczania różnicy, ukrywania remisu i pierwszego zawodnika pozostają stosowane.

## Weryfikacja

- `--history-perf-only --history-perf-assert`: powtarzalny pomiar i limit liczby resetów.
- `--history-dns-only`: wybór serii z paska/menu, powrót przyciskiem, brak zbędnego wyboru przed startem kolejnej serii, wszystkie trzy rodzaje konkursów, archiwum, DNS i DSQ.
- `--team-setup-compact-only`: liczba wierszy, podsumowanie składu, brak sortowania i stałego pola wyszukiwania, filtrowanie oraz wejście do odrębnej edycji drużyny.
- Pełne regresje Qt Quick i alternatywa Widgets; kontrola pamięci scenariuszy historii.

Wyniki końcowe: pełny zestaw **399 renderów responsywnych — PASS**; test historii i DNS/DSQ dla trzech rodzajów konkursów oraz rzeczywiste kliknięcia wyboru/powrotu — PASS; kompaktowa lista drużyn z otwarciem i zamknięciem edytora składu — PASS. Test wydajności ponownie uzyskał **102/107 ms i po jednym resecie**. Kontrola AddressSanitizer scenariuszy historii zakończyła się normalnie bez zgłoszenia błędu pamięci. Alternatywny wariant Qt Widgets zbudowany poprawnie.
