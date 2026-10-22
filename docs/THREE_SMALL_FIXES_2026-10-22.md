# Trzy niewielkie poprawki o istotnych skutkach

Gałąź: `feature/qml-competition-config`; `main` bez zmian. Każda poprawka ma osobny commit i regresję.

## 1. Zapis presetu niszczył przypisania skoczni w pamięci

`SeasonCalendarPreset::getJsonObject` przyjmował preset przez wartość, ale jego kalendarz przechowuje wskaźniki do konkursów. Wywołania `setHill(nullptr)` modyfikowały rzeczywiste obiekty presetu. Zapis nie był więc operacją tylko do odczytu; późniejsze użycie tych danych traciło skocznię i mogło trafić na dereferencję pustego wskaźnika.

Poprawka usuwa `hill-id` z kopii struktury JSON, bez zmiany obiektów. Dotychczasowy format pliku i lista nazw/HS skoczni pozostają zachowane.

Regresja tworzy kalendarz z konkursem i treningiem, serializuje preset dwukrotnie i sprawdza zachowanie wskaźników, stabilny JSON oraz brak lokalnych identyfikatorów skoczni w pliku. Przed poprawką test kończył się błędem zachowania wskaźników. Po poprawce `scripts/test-quick-ui.sh --backend-only` przechodzi.

## 2. Automatyczny eksport CSV nadpisywał wcześniejsze wyniki

Nazwy pojedynczych konkursów zawierały czas z dokładnością do minuty. Dwa konkursy na tej samej skoczni kończone w tej samej minucie trafiały do tego samego pliku, otwieranego z nadpisaniem. Podobna kolizja była możliwa przy ponownym eksporcie zawodów sezonu.

Eksport otwiera plik z `NewOnly`, co chroni wcześniejszą zawartość również przed równoczesnym utworzeniem pliku. Przy zajętej nazwie wybiera kolejno `nazwa (2).csv`, `nazwa (3).csv`. Ścieżkę składa przez `QDir`, więc katalog nie wymaga końcowego separatora.

Regresja zakłada istniejący plik z oznaczoną zawartością i dwukrotnie eksportuje wyniki pod tę samą nazwę. Przed poprawką pierwszy eksport niszczył wcześniejszą treść. Po poprawce wcześniejszy plik pozostaje bez zmian i powstają dwa osobne, niepuste eksporty. `scripts/test-quick-ui.sh --backend-only` przechodzi.
