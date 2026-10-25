# Reguły klasyfikacji, wkład zawodników i uproszczona punktacja

## Wiek w sezonie

Wiek rocznikowy używa roku rozpoczęcia sezonu (`Season::seasonNumber`). Jest stały przez cały sezon, również gdy daty konkursów przechodzą na następny rok. Przypisanie kalendarzy, zmiana sezonu, odczyt sezonu i edycja kalendarza wiążą klasyfikacje z tym rokiem. Podczas wczytywania rok zostaje przypisany przed przeliczeniem zapisanych wyników klasyfikacji.

Usunięto widoczne pole roku odniesienia i możliwość zmieniania go dla pojedynczego konkursu. Dawne pole w definicji pozostaje do zgodności formatu i dla definicji niezwiązanych jeszcze z sezonem. Granice wieku wynoszą 1–100, włącznie; nieznany wiek ma własny przełącznik. Znany rok urodzenia późniejszy niż rok sezonu nie jest traktowany jako brak danych.

Filtr decyduje o punktach danej klasyfikacji. Nie dodaje zakazu startu w konkursie. Nowy sezon ponownie ocenia wiek. W konkursie drużynowym dotychczasowa zasada wymaga, aby cały skład spełniał filtr, zanim kraj otrzyma punkty za miejsce drużyny.

## Wkład kraju z indywidualnych wyników

Filtr kraju działa wyłącznie w klasyfikacji drużynowej i wyłącznie dla wkładu z konkursów indywidualnych. Można uwzględnić tylko albo wykluczyć N najlepszych/najsłabszych zawodników kraju. W konkursach drużynowych ten filtr jest pomijany; UI mówi o tym bezpośrednio i wyjaśnia mechanizm w pomocy.

Wybór jest wykonywany dla każdego konkursu osobno, po sprawdzeniu kwalifikowalności wiekowej. Kolejny konkurs może mieć innych punktujących. Nie wybieramy TOP N według bieżącej sumy sezonu i nie usuwamy wcześniej przyznanych punktów, gdy zmieni się kolejność zawodników w następnym konkursie. Remisy nie zwiększają N; rozstrzygnięcie granicy pozostaje deterministyczne. N większe od liczby skoczków oznacza wszystkich przy uwzględnianiu albo nikogo przy wykluczaniu.

Dodano do JSON `country-exclude`, domyślnie `false` dla starszych zapisów i presetów. Indywidualne klasyfikacje ignorują dawne limity kraju i pokazują wszystkich zawodników spełniających reguły punktowania; ustawienia te nie mogą już ukrywać indywidualnych wyników ani pośrednio ograniczać kwalifikacji.

## Dwuklik na kraj

Dwukrotne kliknięcie kraju w klasyfikacji drużynowej otwiera podgląd wkładu: zawodników z punktami zdobytymi z ich wyników indywidualnych oraz osobną pozycję „Konkursy drużynowe”. Punktów za miejsce całej drużyny nie rozdzielamy arbitralnie pomiędzy jej członków.

Podgląd używa tego samego `Classification::contributions()` co klasyfikacja, włącznie ze sposobem, wiekiem, limitem kraju i mnożnikiem. Suma rozkładu odpowiada sumie kraju. W archiwum po wyborze stanu po konkursie X uwzględnia tylko ten zakres. Zapisane źródła są deduplikowane. Wartości zachowują ułamkowe punkty; tabela jest tylko do odczytu, bez sortowania, wyszukiwania i ogólnych akcji edycji.

## Interfejs punktacji

- Zamiast etykiety „Naliczanie punktów” jest sam wybór: „Klasyczne punkty za miejsca” albo „Naliczaj notę z konkursu”. Pomoc wyjaśnia dodawanie noty, np. 276,4 pkt, i zastosowanie w turniejach.
- Dwa segmenty „Konkurs indywidualny” / „Konkurs drużynowy” zastępują dropdown źródła. Pełna, precyzyjna nazwa wybranego źródła znajduje się pod segmentami, dzięki czemu nie wydłuża samych przycisków. Pomoc zależy od rodzaju klasyfikacji i wskazuje odpowiedni segment.
- Wybrany preset jest pokazywany z nazwy. Edycja punktów przełącza na „Własne”, o ile tabela nie odpowiada innemu znanemu presetowi. „Własne” jest stanem, nie presetem do wybrania; nie może przypadkowo wyczyścić punktacji. Gdy identyczne tabele mają różne nazwy, zachowany zostaje świadomie wybrany preset.
- Podstawowy sposób nazywa się „Domyślny”.
- Ustawienia konkursu udostępniają obecnie tylko mnożnik. Przełącznik brzmi „Pozwól zmieniać mnożnik punktów w konkursach”. Odczyt pozostałych dawnych nadpisań numerycznych pozostaje obsługiwany dla zgodności dotychczasowych zapisów; rok wieku zawsze wynika z sezonu.

## Przewijanie i model tabeli

Wstawienie/usunięcie wiersza płaskiej, nieprzefiltrowanej tabeli punktacji przekazuje teraz poprawne sygnały wstawienia/usunięcia do adaptera Quick. Nie resetuje całej listy ani wszystkich jej delegatów. Dla innych modeli i nietypowych mapowań pozostaje dotychczasowy mechanizm. Kontrakt adaptera weryfikuje `QAbstractItemModelTester`; dodanie 83. miejsca nie powoduje resetu i zachowuje pozycję siatki i formularza. Ochrona scrolla wygasa również wtedy, gdy polecenie nie zmienia modelu, aby nie przechowywać starej pozycji do kolejnej operacji.

## Weryfikacja

`--classifications-only` sprawdza reguły wieku i zapis sezonu przez przejście roku, wszystkie warianty filtra kraju, zgodność JSON/presetów, rzeczywiste kontrolki, nazwy presetów i „Własne”, zmianę nazwy sposobu w miejscu, model i przewinięcie po dodaniu miejsca, brak filtra kraju dla indywidualnych klasyfikacji oraz dwuklik z rozkładem 600 pkt i historycznym rozkładem 100 pkt. Dodatkowe powtórzenia sprawdzają stabilność przewijania. `--history-dns-only` obejmuje geometrię pełnego tła dropdownów i powrót do konkursu po podglądzie. Pełna regresja renderuje responsywne ekrany; alternatywa Widgets pozostaje budowana.

Wyniki końcowe: pełny zestaw **399 renderów responsywnych — PASS**, bez błędów JavaScript, pętli bindingów i rekursywnych układów. Dedykowany zestaw klasyfikacji oraz dwa powtórzenia regresji UI/scrolla — PASS; ostatni przebieg obejmuje również odczyt rzeczywistych sum po przejściu roku i renderowany podgląd wkładu. Budowa Qt Widgets — PASS.
