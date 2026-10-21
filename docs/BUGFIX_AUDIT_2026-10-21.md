# Audyt UX i logiki — 21 października 2026

Gałąź: `feature/qml-competition-config`. Zakres: 10 błędów UX/logiki oraz 3 dodatkowe przypadki awarii. Każdy błąd ma osobny commit. Zmiany nie obejmują `main`.

## 01. Błędne miejsca po remisie w historii klasyfikacji

- **Objaw:** dla wyników 100, 100, 80 historia mogła pokazywać miejsca 1, 1, 2 zamiast 1, 1, 3. Dotyczyło zawodników i drużyn.
- **Przyczyna:** pomocnicze rankingi archiwum nie powiększały bloku remisujących; w wariancie drużynowym występowało także porównanie `add == 1` zamiast aktualizacji licznika.
- **Poprawka:** oba warianty stosują ranking konkursowy z pomijaniem miejsc zajętych przez remisujących.
- **Test:** remis dwóch liderów i zawodnik/drużyna za nimi; wymagane 1, 1, 3.
- **Kod:** `source/competitions/CompetitionResults.cpp`.
- **Commit:** `6233bdb`.

## 02. Filtr zakresu statystyk wskazywał inny konkurs

- **Objaw:** po połączeniu kalendarzy z częściowo rozegranymi sezonami wybór końca zakresu mógł uwzględnić nierozgrany konkurs z poprzedniego kalendarza zamiast wydarzenia widocznego w polu wyboru.
- **Przyczyna:** lista pokazywała tylko rozegrane wydarzenia, natomiast odtwarzanie wybranego indeksu liczyło pełne kalendarze i nagłówki sezonów inaczej.
- **Poprawka:** każdy widoczny wpis ma bezpośrednie powiązanie z konkursem. Nagłówki są nieaktywne; pusty zakres nie udaje pierwszego konkursu.
- **Test:** kalendarz z rozegranym konkursem i przyszłym wydarzeniem, drugi kalendarz z rozegranym konkursem; sprawdzone oba końce zakresu, filtrowanie kalendarza i nagłówek.
- **Kod:** `source/UI/Seasons/Stats/FilteringWidgets/CompetitionsRangeComboBoxesWidget.*`.
- **Commit:** `6f44d3b`.

## 03. Przebudowa filtrów klasyfikacji mogła uszkodzić kontrolki

- **Objaw:** kolejne przebudowy zostawiały stare checkboxy, gubiły mapowanie zaznaczeń i mogły doprowadzić do odwołania do usuniętej kontrolki.
- **Przyczyna:** wektor kontrolek był skracany w trakcie iteracji po nim.
- **Poprawka:** usunięcie całego starego zestawu przed wyczyszczeniem wektora; zabezpieczenia braku źródła i indeksowania zaznaczeń.
- **Test:** pięć kolejnych przebudów trzech klasyfikacji, dokładna liczba kontrolek i zaznaczenie wszystkich trzech.
- **Kod:** `source/UI/Seasons/Stats/FilteringWidgets/ClassificationsCheckBoxesWidget.*`.
- **Commit:** `91a1e6e`.

## 04. Generator formy akceptował sprzeczne lub nieliczbowe parametry

- **Objaw:** tekst w komórce mógł stać się zerem; minimum mogło przekraczać maksimum. W takiej konfiguracji generowanie nie respektowało sensownego przedziału formy.
- **Przyczyna:** konwersja do liczby bez sprawdzenia powodzenia i niezależna walidacja granic.
- **Poprawka:** walidacja roli, indeksu, konwersji, skończoności i zakresów; odrzucanie odwróconych granic w tabeli. Zmiana zbiorcza jednej granicy dopasowuje drugą, jeśli przedział byłby sprzeczny.
- **Test:** tekst `abc`, NaN, minimum 80/maksimum 40 i zbiorcze ustawienie granic.
- **Kod:** `source/UI/FormGenerator/JumperFormGeneratorsSettingsTableModel.cpp`, `JumpersFormGeneratorConfigWindow.cpp`.
- **Commit:** `55e6d16`.

## 05. Zerowa dyskwalifikacja w manipulatorze gwarantowała DSQ

- **Objaw:** ustawienie dokładnego prawdopodobieństwa DSQ na 0 dyskwalifikowało zawodnika zamiast wyłączyć DSQ.
- **Przyczyna:** zero było zamieniane na mianownik 1, czyli pewną dyskwalifikację.
- **Poprawka:** dokładne 0 wyłącza DSQ tak samo jak zerowa wartość bazowa; 1 nadal gwarantuje DSQ, a -1 dziedziczy ustawienie bazowe.
- **Test:** rzeczywiste symulacje skoku dla wszystkich trzech ustawień, w tym 0 przy bazowej pewnej dyskwalifikacji.
- **Kod:** `source/simulator/JumpSimulator.cpp`.
- **Commit:** `07d6e30`.

## 06. „Pozostaw zaznaczonych” usuwało wszystkich zaznaczonych

- **Objaw:** zaznaczenie wszystkich zawodników generatora formy i wybranie „Pozostaw zaznaczonych” usuwało całą listę.
- **Przyczyna:** dopełnienie zaznaczenia obliczano tylko wtedy, gdy zaznaczonych wierszy było mniej niż wszystkich. W przeciwnym przypadku wykonywano zwykłe usuwanie zaznaczenia.
- **Poprawka:** konsekwentna obsługa zaznaczenia/dopełnienia oraz prawidłowe powiadomienie o zmianie modelu. Brak zaznaczenia nie zmienia listy.
- **Test:** zaznaczeni wszyscy, pojedyncza komórka jednego zawodnika i puste zaznaczenie.
- **Kod:** `source/UI/FormGenerator/JumpersFormGeneratorConfigWindow.cpp`, `JumperFormGeneratorsSettingsTableModel.cpp`.
- **Commit:** `2993270`.

## 07. Statystyki tworzyły NaN przy braku ocen lub ważnych skoków

- **Objaw:** średnie oraz procenty mogły zawierać NaN; ranking zawierał liczby, których nie można porównywać poprawnie. Przykłady: wyłączone noty, wyłączone rekompensaty wiatrowe, wszystkie skoki DSQ.
- **Przyczyna:** dzielenie przez liczbę pomiarów równą zero.
- **Poprawka:** pomijanie nieokreślonych średnich w rankingach; brak danych zbiorczych jest oznaczony kreską, bez dzielenia przez zero.
- **Test:** wyłączone składniki punktacji, pole z samymi DSQ, brak nieokreślonych wartości w etykietach.
- **Kod:** `source/UI/Seasons/Stats/SimulationRatingsWindow.cpp`.
- **Commit:** `2e4f5cc`.

## 08. Mały konkurs lub remis zwycięzców wywracał statystyki

- **Objaw:** statystyki konkursu z mniej niż 10 wynikami mogły się zamknąć awaryjnie. Remis na pierwszym miejscu powodował dodatkowo brak wskaźnika rekordu przewagi.
- **Przyczyna:** etykiety odwoływały się do konkursu z największą przewagą, mimo że nie znaleziono takiego konkursu. Dla zerowej przewagi nigdy nie przypisywano maksimum.
- **Poprawka:** rekord nieistniejący jest oznaczony jako niedostępny; zerowa przewaga jest prawidłowym rekordem. Opis wskazuje faktyczny sezon i kalendarz.
- **Test:** konkurs dwóch zawodników z remisem; przewaga zwycięzcy 0, brak porównania do dziesiątego miejsca.
- **Kod:** `source/UI/Seasons/Stats/SimulationRatingsWindow.cpp`.
- **Commit:** `698feb4`.

## 09. Brak presetów punktacji wywracał rankingi

- **Objaw:** usunięcie wszystkich presetów lub brak wybranego presetu prowadziły do indeksowania pustej kolekcji.
- **Przyczyna:** bezwarunkowy odczyt presetu według indeksu, również -1.
- **Poprawka:** sprawdzenie obecności presetu. Bez niego klasyfikacja za miejsca pozostaje pusta, ale średnie i klasyfikacja według punktów konkursowych działają.
- **Test:** wyczyszczenie kolekcji presetów i pola wyboru; oba tryby klasyfikacji.
- **Kod:** `source/UI/Seasons/Stats/SimulationRatingsWindow.cpp`.
- **Commit:** `38e8e31`.

## 10. Puste filtry pokazywały poprzednie statystyki

- **Objaw:** po zmianie filtra na taki, który nie ma wyników, pozostawały stare tabele i rekordy. Można je było uznać za dane nowego zakresu lub wyeksportować.
- **Przyczyna:** obsługa pustego wyniku wyświetlała komunikat i wracała bez wyczyszczenia modeli.
- **Poprawka:** czyszczenie rankingów, rekordów, klasyfikacji, danych eksportu i faktów; bez blokującego komunikatu przy każdej zmianie filtra. Tabele średnich mają własne modele już od inicjalizacji.
- **Test:** przejście z zapełnionego wyniku na pusty; wszystkie tabele i etykiety muszą odzwierciedlać brak danych.
- **Kod:** `source/UI/Seasons/Stats/SimulationRatingsWindow.*`.
- **Commit:** `642a618`.

## 11. Dodatkowa awaria: pusty kalendarz lub brak końca zakresu

- **Objaw:** wybór pustego kalendarza albo brak prawidłowych granic zakresu powodowały awarię zamiast pustych statystyk.
- **Przyczyna:** `first()` na pustym kalendarzu, indeks -1 przy zmianie wyboru oraz odczyt skoczni przez pusty wskaźnik początku/końca zakresu.
- **Poprawka:** walidacja indeksów i granic; pusty lub odwrócony zakres zwraca pusty wynik. Oba ekrany statystyk obsługują puste kalendarze.
- **Test:** zmiana pełnego kalendarza na pusty oraz wywołania zakresu z pustymi wskaźnikami.
- **Kod:** `source/competitions/CompetitionInfo.cpp`, `source/UI/Seasons/Stats/SimulationRatingsWindow.cpp`, `JumperStatsWindow.cpp`.
- **Commit:** `8511b0f`.

## 12. Dodatkowa awaria: niekompletne lub odłączone dane skoku

- **Objaw:** szczegóły skoku bez zawodnika/skoczni albo z powiązaniem do konkursu bez odpowiedniego wyniku mogły spowodować awarię.
- **Przyczyna:** niekontrolowane puste wskaźniki oraz wyznaczanie serii z indeksu -1.
- **Poprawka:** czytelny stan braku danych, nieaktywne udostępnianie i bezpieczny brak miejsca w serii. Ten sam widok potrafi ponownie pokazać kompletne dane.
- **Test:** brak obu powiązań, brak samej skoczni, ponowne wypełnienie poprawnym skokiem i skok spoza wyników wskazanego konkursu.
- **Kod:** `source/UI/ResultsShowing/JumpDataDetailedInfoWindow.cpp`, `source/simulator/JumpData.cpp`.
- **Commit:** `37d1b8f`.

## 13. Dodatkowa awaria: DNS i puste wykresy zawodnika

- **Objaw:** wynik zawodnika bez zapisanych skoków powodował odczyt pierwszego elementu pustej listy; puste wykresy i nieaktualne indeksy wskazywanych punktów również wymagały zabezpieczeń.
- **Przyczyna:** założenie, że każdy wynik ma skok i każdy wykres ma osie; mapowanie punktów nie uwzględniało zawsze filtrów i braków danych.
- **Poprawka:** pomijanie pustych wyników, kontrola osi i indeksów, bezpieczne etykiety braku danych. Średnia miejsc dotyczy faktycznie wyświetlanych wyników.
- **Test:** wynik DNS bez skoków, a następnie pusty kalendarz w statystykach tego zawodnika.
- **Kod:** `source/UI/Seasons/Stats/JumperStatsWindow.cpp`.
- **Commit:** `3872878`.

## Odtworzenie testów

```sh
scripts/test-quick-ui.sh --audit-only
scripts/test-quick-ui.sh --audit-data-only
scripts/test-quick-ui.sh --audit-form-only
scripts/test-quick-ui.sh --audit-stats-only
scripts/test-quick-ui.sh
```

Testy korzystają z jednorazowych kopii danych. Pełny przebieg zawiera również 13 nowych regresji. Walidacja odbywa się na Qt 5.15.3 z rendererem programowym offscreen; nie jest to test wszystkich sterowników graficznych ani wszystkich możliwych uszkodzonych zapisów.

## Wynik weryfikacji

- Pełny zestaw integracyjny: zakończony kodem 0; 292 widoki responsywne.
- Nowe testy regresyjne 01–13: zaliczone w pełnym przebiegu.
- Brak błędów ładowania QML i ostrzeżeń o pętlach wiązań w tym przebiegu.
- Wariant Widgets-only: kompilacja zakończona kodem 0; bez zależności Qt Quick/QML.
- Gałąź `main`: identyczny hash przed pracą i po poprawkach.
