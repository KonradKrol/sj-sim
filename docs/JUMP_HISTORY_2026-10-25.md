# Podgląd skoków i wyniki drużynowe

- Karta skoku pozwala wybrać wcześniejszą serię, jeśli zawodnik oddał więcej niż jeden skok. Zmiana aktualizuje dane wybranej próby.
- Shift+klik w kolumnę konkretnej serii otwiera jej skok w konkursie indywidualnym i drużynowym. Zwykłe kliknięcie otwiera najnowszy skok. Dla wiersza drużyny wybierany jest ostatni zawodnik, który skoczył w wskazanej serii.
- Tabela drużynowa pokazuje punkty serii i sumę. Usunięto z widoku Quick zbiorczą kolumnę odległości również przy rozwiniętych zawodnikach; odległość pozostaje w karcie skoku. Widok Widgets zachowuje swoją kolumnę.
- Dyskretna sekcja „Wagi” rozwija składniki sumy: punkty każdej serii, mnożnik, wkład i zaokrąglony wynik. Wybór wcześniejszej serii używa jej reguł wag. Dane nie są ujawniane podczas trwającego lotu.
- Stopka konkursu pokazuje podpowiedź Shift+kliku. Usunięto jedynie podpowiedź Alt+←, pozostawiając działający skrót cofania.
- Przed pierwszym skokiem serii oraz przy remisie po zaokrągleniu do 0,1 pkt karta nie pokazuje przewagi/straty.
- Ponowne otwieranie wyniku usuwa poprzednie strony szczegółów przed utworzeniem nowych; ich liczba nie rośnie przy kolejnych kliknięciach.

## Weryfikacja

Automatyczne scenariusze trzech serii obejmują konkurs indywidualny i drużynowy, rzeczywiste Shift+kliknięcia, przełączanie serii, obliczenia wag, stopkę, brak kolumny odległości oraz ograniczoną liczbę stron szczegółów. Osobne regresje obejmują pierwszy skok, remis i reset punktów. Zachowana jest kompilacja wersji Widgets.

Weryfikacja końcowa: `scripts/test-quick-ui.sh --jump-history-only` — PASS dla scenariuszy indywidualnego i drużynowego; `scripts/test-quick-ui.sh` — PASS, 399 responsywnych renderów, bez ostrzeżeń rekurencji layoutów i pętli bindingów. `make -C build-widgets-compat -j3` — zakończone kodem 0.
