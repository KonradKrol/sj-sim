# Edytor klasyfikacji: osobny interfejs Qt Quick

## Organizacja

Zamiast automatycznej prezentacji drzewa formularza Widgets działa osobny formularz Quick. Nazwa i rodzaj klasyfikacji mają własną kartę; punktacja jest główną sekcją, a filtry zaawansowane osobną, zwijaną sekcją. Silnik naliczania punktów i format zapisów pozostają dotychczasowe.

## Punktacja

- Domyślnie nie ma wyboru ani nazwy sposobu. „Licz punkty na różne sposoby” odsłania wyróżniony panel wyboru, tworzenia kopii, nazwy i usuwania sposobu. Istniejące dodatkowe sposoby są ujawniane przy otwarciu klasyfikacji. Schowanie panelu wraca do edycji punktacji domyślnej, zachowując istniejące sposoby i powiązania konkursów.
- Mały znak zapytania obok przełącznika zawiera przekazane przez użytkownika wyjaśnienie z przykładami duetów, konkursów czteroosobowych i kwalifikacji.
- Jedna tabela i jeden zestaw poleceń obsługują oba źródła punktów. Wybór źródła zachowuje precyzyjne nazwy zależne od rodzaju klasyfikacji: punkty zawodnika za miejsce jego drużyny albo punkty kraju z indywidualnych wyników skoczków.
- Punkty edytuje się bezpośrednio w polach tabeli. Presety, dodanie/usunięcie ostatniego punktowanego miejsca i wyczyszczenie działają na wybranym źródle i wybranym sposobie.
- Nie ma ogólnego paska tabeli, szukania, sortowania ani poleceń Ctrl+Shift+A / Ctrl+Shift+D. Usunięto także same skróty ze sterownika klasyfikacji.

## Filtry

- „Filtry zaawansowane” zastępują poprzednią etykietę. Aktywne filtry odsłaniają się przy otwarciu istniejącej konfiguracji.
- Filtr wieku odsłania dopiero po włączeniu zakres i rok odniesienia oraz opcję uwzględniania nieznanego wieku.
- „Ogranicz liczbę skoczków z kraju” włącza wybór najlepszych/najsłabszych oraz pole „Skoczków na kraj”. Wyłączenie oznacza wszystkich, bez nieaktywnej „liczby N”. Wartość limitu pozostaje zachowana do ponownego włączenia.
- Osobne parametry konkursów mają własny przełącznik. Wyjaśnienia wieku, limitu i nadpisań są w tooltipach małych znaków „?”, dostępnych także po fokusie klawiatury.
- Klasyfikacje wykorzystane w rozegranych zawodach zachowują dotychczasowe blokady. Zmiana nazwy nadal jest dostępna.

## Weryfikacja

Testy `--classifications-only` obejmują odsłanianie sposobów, przełączanie jednej tabeli, zapis punktacji dodatkowej bez zmiany podstawowej, rzeczywiste kliknięcia przełączników wieku i kraju, ukrywanie niepotrzebnego limitu, brak dawnych skrótów oraz dotychczasowe sposoby, parametry, Cofnij/Ponów i presety kalendarza. `--workflow-only` sprawdza również responsywne renderowanie klasyfikacji i nawigację sezonu.
