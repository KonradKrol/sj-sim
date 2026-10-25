# KO: okresowe przebudowywanie grup i dropdownów

## Przyczyna

Most Quick odświeża stan co 120 ms. Ogólne przejście po drzewie Widgets aktualizowało węzeł KOGroupsListView jako zwykłą kolumnę, po czym kompozycja ekranu ponownie aktualizowała ten sam obiekt jako dedykowany obszar grup KO. Nawet bez zmiany danych emitowano więc zmiany, a widok QML dostawał przejściowo pusty model grup, nazw i sposobów doboru. To powodowało niepotrzebne niszczenie/odtwarzanie kontrolek i zakłócało otwarte listy wyboru.

## Poprawka

Ogólne przejście rozpoznaje KOGroupsListView i zwraca jego stabilny węzeł bez przetwarzania ukrytego drzewa kontrolek. Dane nadal aktualizuje dedykowana kompozycja KO; rzeczywista zmiana metody lub zawodnika działa jak wcześniej. Dotyczy konfiguracji pojedynczego konkursu i tej samej ścieżki w sezonie. Nie zmieniono algorytmu losowania ani częstotliwości aktualizacji pozostałych ekranów.

## Regresja

Test przygotowania KO otwiera listę sposobów doboru, czeka przez okresowe odświeżenia i wymusza pięć kolejnych aktualizacji. Wymaga braku valuesChanged dla niezmienionego węzła i pozostawienia listy otwartej. Dotychczasowe scenariusze sprawdzają też wielokrotne zmiany metody, nieparzystą listę startową i zamianę zawodników pomiędzy grupami.

Wynik: `scripts/test-quick-ui.sh --ko-groups-only` — PASS, w tym nowe asercje stabilności węzła i otwartego dropdowna.
