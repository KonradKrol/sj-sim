# Crash po zakończeniu pojedynczego konkursu

Gałąź: `feature/qml-competition-config`. `main` pozostaje bez zmian.

## Przyczyna i odtworzenie

Po powrocie z `CompetitionManagerWindow::exec()` konfigurator usuwał menedżera i skocznię, ale pozostawiał okno konkursu, jego modele i adapter Quick. Quick przenosi okna do głównego okna aplikacji, więc nie znikały wraz z lokalnym przebiegiem konfiguracji. Późniejsze odświeżenie odwoływało się do zwolnionego menedżera.

W regresji odtworzono pełny konkurs i kliknięcie widocznego „Zakończ konkurs”. Następnie jawne odświeżenie zachowanego adaptera wywołało SIGSEGV (kod 139). Debugger wskazał `QuickBridge::refresh()` i wywołanie przez nieważny wskaźnik. Wcześniejszy test zamykał konkurs przez `accept()` i nie sprawdzał życia okna po powrocie.

## Poprawka

- Okna konkursu i kwalifikacji mają zakres życia krótszy od danych symulacji. Ich adaptery, sceny i odroczone wywołania są usuwane przed zwolnieniem menedżera.
- Menedżery mają właścicieli RAII, a kopie skoczni żyją na stosie. Wcześniejszy powrót po anulowanych kwalifikacjach również zwalnia zasoby.
- Aktualizacja rekordu życiowego pomija zawodnika, którego nie znaleziono w globalnej bazie, zamiast dereferencji `nullptr`.
- Regresja używa rzeczywistego przycisku zakończenia i sprawdza przez `QPointer`, że okno nie przetrwało zakończonej symulacji.

## Walidacja

`scripts/test-quick-ui.sh --ux-only` — kod 0 po poprawce, wraz z pełnym konkursem indywidualnym, prezentacją na żywo i powrotem do konfiguracji. Końcowe `scripts/test-quick-ui.sh --competitions-only` również kończy się kodem 0: konkursy indywidualne, KO i drużynowe, przerwanie prezentacji na żywo i analiza sezonu. Zakończenie korzysta z rzeczywistego przycisku, a `QPointer` potwierdza usunięcie okna. Kompilacja Widgets-only przechodzi.
