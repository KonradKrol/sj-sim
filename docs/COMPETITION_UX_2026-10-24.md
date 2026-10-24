# Konkursy i edytor kalendarza — 24.10.2026

Zmiany na `feature/qml-competition-config`; main pozostaje bez zmian. Numer aplikacji pozostaje **1.3.0 beta-3**. Ta partia kontynuuje stan z `33605a6` po zmianach z Windows.

## Awans i kwalifikacje

- Wspólny algorytm wyznacza najlepsze i najgorsze możliwe miejsce po skokach pozostałych zawodników. Pewny awans oraz pewne odpadnięcie nie zależą już od pojawienia się zielonej linii ani od tego, czy istnieje wynik dokładnie na granicznym miejscu.
- Przykład: przy miejscach 1, 2, 2, 4 i limicie 3 zawodnik na miejscu 4 odpada. Remis na granicy może powiększyć zwykłą następną serię; nie oznacza dopuszczenia kolejnego odrębnego miejsca.
- Osobne stany dotyczą następnej serii i następnego konkursu. Jeżeli ten sam konkurs jest źródłem kwalifikacji dla kilku wydarzeń, obowiązuje najbliższy przyszły, nierozgrany i nieodwołany konkurs zgodnego rodzaju w kolejności kalendarza.
- Pewną kwalifikację do konkursu oznacza neutralne **Q**. Pewny awans do następnej serii ma łagodniejsze zielone tło oraz znak ✓; odpadnięcie ma szare tło i przygaszony tekst. Wiersz pozostaje dostępny. Zielony kolor Qt Quick jest zmieszany w 20% z kolorem powierzchni.
- Zawodnicy odpadający w poprzedniej serii pozostają wyszarzeni także po przejściu do następnej. Brak kolejnej serii ani konkursu nie tworzy sztucznego oznaczenia awansu.
- Liczba faktycznie rozgrywanych serii uwzględnia wyjątkowe ograniczenie liczby serii. Techniczna seria przechowująca limit kwalifikacji nie pojawia się jako rzeczywista druga seria.
- W KO rozdzielono pewność wygrania grupy od szansy na lucky loser. Nieukończona grupa nie eliminuje zawodnika przedwcześnie; słabszy przegrany może zostać wyeliminowany przed zakończeniem wszystkich pojedynków.
- Drużyna nie dostaje pewnego wyniku awansu po pierwszych grupach jej zawodników. Pojedynczy zerowy DSQ/DNS członka nie dyskwalifikuje automatycznie całej drużyny z kwalifikacji.
- Indywidualne DNS/DSQ są wykluczane z pola kwalifikacji przed ponownym wyznaczeniem miejsc. Zachowana jest ochrona upadku po przekroczeniu 95% HS tam, gdzie obowiązuje.
- Kwalifikacje do KO wybierają dokładnie limit zawodników; zwykłe kwalifikacje zachowują remisy na granicy. Filtry nie dublują uczestników ani nie wychodzą poza istniejące wyniki przy małym polu.
- Modele wyników udostępniają osobne role stanów awansu. Odczyt koloru komórki nie sortuje już wyników i nie zmienia kolejności podczas renderowania.

## Ex aequo i belka

- Porównanie punktów w rankingu i rozpoznawanie remisu używa tej samej wartości w dziesiątych punktu. Usunięto rozbieżność pomiędzy surowym `double`, widoczną sumą i miejscem zawodnika.
- Remis wymaga także zgodnego etapu punktacji. Uwzględnia to wieloseryjne zawody oraz resetowanie lub przenoszenie punktów. Sortowanie jest stabilne przy całkowitej równości.
- Zmiana belki trenera nie wywołuje pośrednio zmiany belki jury i bazowej belki serii. Ponowne ustawienie i anulowanie dialogu zachowuje poprawną bazę; ręczne ustawienie jury usuwa poprzedni override trenera.
- Baza serii zmienia się tylko przed pierwszą próbą w całej serii, a nie przy indeksie zero w kolejnej grupie drużynowej. Obsługa belek jest sprawdzana dla pierwszego i ostatniego zawodnika.

## Lista startowa, liczby i ekran konkursu

- Przeciąganie zawodnika obsługuje stała warstwa poza wirtualizowanym wierszem. Gest działa dalej, gdy wiersz źródłowy znika wskutek przewijania.
- Przy krawędzi listy działa przyspieszające automatyczne przewijanie, a podczas przeciągania można też używać kółka myszy. Podgląd zachowuje nazwisko, flagę i liczbę zaznaczonych osób; Escape anuluje przenoszenie.
- Pole liczby skoków zachowuje roboczy wpis mimo odświeżania backendu. Wolne wpisywanie kilku cyfr nie jest kasowane. Plus/minus najpierw uwzględnia wpisaną wartość. Wspólny komponent obejmuje również pola planowania dat.
- Jednoseryjne kwalifikacje pokazują „Kwalifikacje”; jednoseryjny konkurs „Seria 1/1”. Wieloseryjne wydarzenia zachowują rzeczywisty licznik.
- Podpowiedź przycisku nie pojawia się od samego focusu, naciskania Spacji ani po kolejnych kliknięciach skoku. Pozostaje celowe najechanie oraz F1; opis dostępności pozostaje zachowany.
- Usunięto wyszukiwanie z bieżącej listy startowej oraz wyników grup KO. Pozostałe tabele zachowują wyszukiwanie.

## Kalendarz i flagi

- Lista konkursów używa stabilnego modelu Qt z wstawianiem, przesuwaniem i zmianami pojedynczych wierszy. Dodanie wydarzenia nie resetuje modelu ani pozycji przewijania; działa także Cofnij/Ponów.
- Zmniejszono wysokość kart i odstępy: zwykły wiersz ma około 48 px, kompaktowy około 32 px, z miejscem na dodatkowe dane, gdy są potrzebne. Daty mają dzień tygodnia. Zamiast etykiet „Konkurs/Kwalifikacje” kwalifikacje mają małe Q po prawej.
- Wybór skoczni przez wyszukiwarkę nie zrywa powiązania aktualnej wartości z backendem. Zmiana zaznaczonego konkursu od razu aktualizuje skocznię i zasady; dotyczy też kalendarza z datami.
- Edytor zasad jest odświeżany przed powiadomieniem warstwy Quick o zmianie wyboru.
- Pola inspektora są przypięte do dostępnej przestrzeni pod nagłówkiem. Usunięto przypadek, w którym mechanizm układu przydzielał im wysokość zero po zmianie skoczni, pozostawiając pustą kartę i nieklikalny wybór.
- Nagłówek kalendarza na małym ekranie zużywa mniej miejsca, dzięki czemu lista pozostaje używalna.
- Dodano flagi w kartach wydarzeń, nagłówku inspektora, widokach planowania dat, wyborach skoczni i zawodników oraz listach składów. Brak dostępnego pliku flagi nie tworzy pustego dużego elementu.

## Testy i odtworzenie

Nowe regresje obejmują:

- wyczerpujące porównanie stanów awansu dla 81 układów wyników, czterech limitów, dwóch zasad remisów i możliwych zakończeń pozostałych prób, w tym DNS;
- wczesne odpadnięcie, remisy graniczne, niezależne limity konkursu i serii, odwołanie celu, KO/lucky loser, drużyny i czystość odczytu modelu;
- remisy ułamkowe, reset punktów i filtry wejścia do konkursu;
- rzeczywisty dialog belki trenera, jego anulowanie oraz zmianę jury na początku i końcu serii;
- długie przeciąganie poza pierwotny widoczny wiersz, kółko, podgląd i cofnięcie;
- powolne wpisanie 123 z odświeżaniem, następnie 124/123 przyciskami; podpowiedzi po kliknięciu, Spacji i F1;
- stabilność modelu kalendarza z `QAbstractItemModelTester`, scroll po dodaniu, cofanie, przełączanie skoczni i zasad, wyszukiwarkę, flagi, Q oraz dzień tygodnia;
- renderowanie oznaczeń kwalifikacji, małe okna i oba motywy.

Uruchomienie pełnego zestawu: `scripts/test-quick-ui.sh`. Dostępne są też `--workflow-only`, `--ux-only`, `--roster-inputs-only`, `--dates-only`, `--backend-only`. Wyniki końcowego uruchomienia oraz ograniczenia opisuje checkpoint z tej daty.

### Wynik końcowy

Pełny zestaw zakończył się kodem 0 i wykonał 375 renderów. Osobny końcowy test przestrzeni edytora przeszedł dla szerokości 1440, 1024, 800, 520 i 360 px, z datami oraz bez dat, po zmianach konkursów i zakładek. Wariant Widgets-only zbudował się poprawnie. Szczegóły środowiska, logi i ograniczenia zapisano w [checkpointcie](RESUME_2026-10-24_COMPETITION_UX.md).
