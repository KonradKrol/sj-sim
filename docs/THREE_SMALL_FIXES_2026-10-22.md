# Trzy niewielkie poprawki o istotnych skutkach

Gałąź: `feature/qml-competition-config`; `main` bez zmian. Każda poprawka ma osobny commit i regresję.

## 1. Zapis presetu niszczył przypisania skoczni w pamięci

`SeasonCalendarPreset::getJsonObject` przyjmował preset przez wartość, ale jego kalendarz przechowuje wskaźniki do konkursów. Wywołania `setHill(nullptr)` modyfikowały rzeczywiste obiekty presetu. Zapis nie był więc operacją tylko do odczytu; późniejsze użycie tych danych traciło skocznię i mogło trafić na dereferencję pustego wskaźnika.

Poprawka usuwa `hill-id` z kopii struktury JSON, bez zmiany obiektów. Dotychczasowy format pliku i lista nazw/HS skoczni pozostają zachowane.

Regresja tworzy kalendarz z konkursem i treningiem, serializuje preset dwukrotnie i sprawdza zachowanie wskaźników, stabilny JSON oraz brak lokalnych identyfikatorów skoczni w pliku. Przed poprawką test kończył się błędem zachowania wskaźników. Po poprawce `scripts/test-quick-ui.sh --backend-only` przechodzi.
