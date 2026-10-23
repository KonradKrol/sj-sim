# Klasyfikacje: sposoby punktowania, filtry i kontekst konkursu

Klasyfikacja ma punktację **Domyślną** i dowolną liczbę nazwanych sposobów punktowania. Każdy sposób zawiera własne tabele punktów za miejsca oraz punktację dodatkową; może też używać not punktowych z zawodów. W edytorze klasyfikacji przycisk **Dodaj sposób (kopia bieżącego)** tworzy punkt wyjścia do kolejnej tabeli. Wiersze można dodawać i usuwać osobno w obu tabelach. Presety punktów wypełniają aktualnie edytowany sposób.

W kalendarzu wybierz konkurs, następnie **Powiązania** i klasyfikację. Wybierz dla niej sposób naliczania punktów. Wybór jest osobny dla każdej pary konkurs–klasyfikacja. Przykładowo w jednej klasyfikacji jeden konkurs może punktować 30 miejsc, drugi 40, a trzeci przyznawać 20 punktów za zwycięstwo. Pozostałe miejsca otrzymują zero punktów. Bez jawnego wyboru obowiązuje punktacja domyślna, również w starszych zapisach.

## Punktacja dodatkowa i Puchar Narodów

- Klasyfikacja indywidualna: podstawowa tabela punktuje indywidualne miejsce zawodnika. Dodatkowa tabela daje zawodnikowi punkty za miejsce jego drużyny w konkursie drużynowym. W trybie not punktowych wliczana jest własna nota zawodnika, a nie nota całej drużyny.
- Klasyfikacja drużynowa: podstawowa tabela punktuje miejsce drużyny w konkursie drużynowym. Dodatkowa tabela punktuje indywidualne wyniki zawodników danego kraju w konkursach indywidualnych. Te punkty sumują się z punktami za konkursy drużynowe, jak w Pucharze Narodów.

Etykieta przy tabeli zmienia się wraz z rodzajem klasyfikacji. Przycisk **?** wyjaśnia mechanizm po najechaniu, ustawieniu fokusu lub kliknięciu.

## Zaawansowane filtry

**Wiek** jest rocznikowy: rok odniesienia minus rok urodzenia. Obie granice wieku są włączne. Domyślnie nieznany wiek nie spełnia aktywnego filtra; można jawnie uwzględnić takich skoczków. Filtr jest stosowany do udziału w poszczególnych konkursach. W konkursie drużynowym cały skład musi spełniać filtr, aby drużyna otrzymała punkty za swoje miejsce.

**N najlepszych / N najgorszych z kraju**:

- W klasyfikacji indywidualnej wybór następuje według sumy punktów całej klasyfikacji. Inni zawodnicy są ukryci w tabeli i nie biorą udziału w kwalifikowaniu według klasyfikacji. Ich wyniki źródłowe pozostają zachowane, więc mogą wrócić do tabeli po zmianie swoich sum punktów.
- W klasyfikacji drużynowej ograniczenie dotyczy indywidualnych wyników każdego kraju w każdym konkursie indywidualnym. Selekcja następuje po filtrze wieku, według miejsca i noty konkursowej. Punkty za konkursy drużynowe są wliczane osobno.
- Remis nigdy nie powiększa limitu N. Rozstrzygnięcie wyboru jest deterministyczne, z użyciem nazwy zawodnika i jego identyfikatora. Jeżeli uprawnionych skoczków jest mniej niż N, uwzględniani są wszyscy dostępni.

## Opcjonalne parametry z konkursu

W klasyfikacji zaznacz **Pozwalaj konkursom nadpisywać parametry klasyfikacji**. W powiązaniach konkursu pojawią się osobno włączane nadpisania:

| Parametr | Znaczenie |
| --- | --- |
| Mnożnik punktów | Mnoży wkład tego konkursu do wskazanej klasyfikacji; zero też jest poprawną wartością. |
| Limit punktowanych miejsc | Dalsze miejsca dostają zero; 0 oznacza brak dodatkowego limitu. |
| Rok odniesienia | Rok stosowany do rocznikowego wieku w tym konkursie. |
| Minimalny / maksymalny wiek | Nadpisują granice już włączonego filtra wieku. |
| N wyników indywidualnych kraju | Nadpisuje liczbę wyników uwzględnianych w punktacji drużynowej z konkursu indywidualnego. |

Wyłączone pola dziedziczą ustawienia klasyfikacji. Nadpisania nie włączają samych filtrów; filtr wieku i wybór top/bottom N konfiguruje się w klasyfikacji. Wybór sposobu punktowania działa również bez włączenia parametrów kontekstu. Edycja wielu zaznaczonych konkursów pokazuje mieszane wartości i może ustawić wspólny sposób lub parametr. Zmiany podlegają cofaniu i ponawianiu w kalendarzu.

Po rozegraniu konkursów obowiązuje dotychczasowa blokada zmiany definicji klasyfikacji; do zmiany zasad można utworzyć jej kopię. Nazwę i pomoc można nadal otwierać.

## Zapis i presety

`scoring-options` w JSON klasyfikacji zawiera sposoby oraz filtry. `classification-scoring` w JSON konkursu zawiera wybrany sposób i jawne parametry, osobno według identyfikatora klasyfikacji. Brak nowych pól zachowuje dotychczasowe naliczanie punktów.

Preset kalendarza zawiera definicje klasyfikacji, sposoby punktowania, filtry, powiązania i parametry konkursów. Przy zapisie i odtworzeniu mapowane są nowe identyfikatory klasyfikacji. Preset nie przenosi rozegranych wyników ani tabel klasyfikacji i nie zmienia źródłowego sezonu. Przypisania sposobów mają stabilne identyfikatory niezależne od nazw wyświetlanych w UI.

## Weryfikacja na Windows

Qt 6.8.3 / MSVC 2022, Release. `--backend-only` sprawdza niezależne tabele konkursów, parametry, granice wieku rocznikowego, dokładny limit N przy remisach, sumowanie obu źródeł Pucharu Narodów, historię klasyfikacji oraz zgodność zapisu i presetów. `--classifications-only` dodatkowo sprawdza edytor, wybór sposobu i zmianę mnożnika w widocznym interfejsie Quick, cofanie/ponawianie oraz rzeczywiste okna zapisu i odtworzenia presetu kalendarza. Oba scenariusze zakończyły się kodem 0; test interfejsu wykonano w natywnym oknie Windows. Przeszły również natywne scenariusze `--navigation-only`, `--round-points-only` i `--packs-only`.

Gotowy `build-win/release/sj-sim.exe` uruchomiono bez ścieżki deweloperskiego Qt w `PATH`; wyświetlił okno i zamknął się kodem 0. Szerszy scenariusz `--competitions-only` przekroczył limit 180 sekund. Ponowienie pod debuggerem dotarło do konkursu drużynowego i zostało przerwane po ponad czterech minutach bez końcowego wyniku, więc nie jest zaliczonym sprawdzeniem pełnego przebiegu konkursów.
