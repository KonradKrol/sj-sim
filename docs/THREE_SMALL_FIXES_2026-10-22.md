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

## 3. Losowanie zwracało wartości spoza zakresu i przepełniało granice

`MyRandom::randomInt(7,7)` zwracało 0, a wariant zmiennoprzecinkowy miał ten sam błąd dla stałego niezerowego zakresu. `randomDouble` rzutowało granice na `int` po przemnożeniu przez dokładność: zakres 1,001–1,009 przy dwóch miejscach dziesiętnych stawał się przedziałem o identycznych granicach, dając niewłaściwy wynik (lub naruszając warunek generatora Qt w konfiguracji z asercjami). Osobny warunek `max == 0` błędnie zerował całe poprawne zakresy ujemne. W `randomInt` wyrażenie `max + 1` przepełniało typ przy `INT_MAX`.

Stały zakres zwraca teraz swoją jedyną wartość. Losowanie całkowite korzysta z rozkładu jednostajnego o domkniętych granicach bez obliczania `max + 1`. Dla liczb zmiennoprzecinkowych losowane są dostępne wartości o zadanej dokładności z zakresu `[min,max)`; gdy zakres nie zawiera takiej wartości, losowanie zachowuje granice kosztem dodatkowych cyfr. Obliczenia nie rzutują przeskalowanych granic na `int` i obsługują również duże skończone wartości.

Regresja przed poprawką kończyła się błędem stałego zakresu. Po poprawce przechodzą też 200-krotne sprawdzenia wąskich zakresów, niezaokrąglonych granic, przedziału ujemnego zakończonego zerem, `INT_MAX`, całego zakresu `int` i skrajnych skończonych granic `double`.

## Końcowa weryfikacja wszystkich trzech poprawek

- `scripts/test-quick-ui.sh --backend-only`: kod 0; każda nowa regresja wcześniej wykazała odpowiedni błąd.
- `scripts/test-quick-ui.sh --competitions-only`: kod 0; pełne konkursy indywidualne, KO i drużynowe, prezentacja na żywo, zakończenie i analiza sezonu.
- Ten sam zbudowany `sj-sim-ui-smoke --workflow-only`: kod 0; bezpośrednia edycja kalendarzy, zmiany masowe, kwalifikacje, presety, Cofnij/Ponów i narzędzia sezonów.
- Kompilacja alternatywnego interfejsu Widgets: kod 0.
- `git diff --check` przechodzi. Końcowe logi konkursów i kalendarzy nie zawierają `FAIL`, błędów typów/odwołań QML ani pętli powiązań.

Pierwsze dwa commity: `d78e4c7` (presety), `291ad73` (CSV). Poprawka losowania i ta końcowa weryfikacja są zapisane razem w trzecim commicie.
