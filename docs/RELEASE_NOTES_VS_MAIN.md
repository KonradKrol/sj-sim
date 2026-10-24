# Sj.Sim — pełny wykaz zmian względem main

Dokument źródłowy do przygotowania release notes dla 2.0.0 albo innego numeru wydania. Opisuje końcowy stan gałęzi, a nie tylko ostatnie poprawki. Obejmuje również zmiany techniczne i narzędziowe, które nie muszą trafić do publicznej zapowiedzi.

## Zakres porównania i wersja

- Baza: main, commit **05f2e48d8e92c7b9b180f18abe250b3c7e61d24c**, opis „Set version label to 1.3.0-beta2 and fix Ctrl+A button focus”.
- Gałąź zmian: **feature/qml-competition-config**.
- Stan porównania kodu i dokumentacji: **5af8f1b**. Pełny hash, zestawienie commitów i manifest plików są w załącznikach.
- Data zestawienia: **24 października 2026**.
- Aktualne oznaczenie w kodzie aplikacji: **2.0.0-beta**, ustawione na wyraźne polecenie użytkownika po zakończeniu audytu silnika.
- main nie był modyfikowany. Podstawą porównania są diff, historia commitów, kod końcowy i raporty testów.
- Starsze raporty opisują kolejne etapy prac. W razie rozbieżności stan końcowy opisany tutaj ma pierwszeństwo. Szczególnie istotne korekty są zebrane w sekcji „Rozwiązania zastąpione podczas prac”.

## Krótki materiał do publicznych release notes

- Nowy domyślny interfejs całej aplikacji w Qt Quick, z własnym sportowym stylem, nową typografią, jasnym i ciemnym motywem.
- Wspólna nawigacja w jednym oknie oraz mniej zagnieżdżonych dialogów.
- Przebudowane listy startowe, wybór zawodników, obsługa zapisanych składów i kolejności.
- Nowy ekran konkursu: najważniejsze dane razem, odległości w tabelach, pełna karta skoku, bezpośredni dostęp do warunków i narzędzi.
- Przebudowane sezony: niezależna edycja i aktywowanie kalendarzy, zmiany masowe, przeciąganie bloków, wielokrotne Cofnij/Ponów oraz zachowywanie zależności kwalifikacji.
- Czytelniejszy generator formy z jawnym zastosowaniem parametrów do wszystkich lub zaznaczonych zawodników.
- Poprawione linie prowadzenia i awansu, obsługa wiatru i punktacji, aktualizacja listy startowej oraz prezentacja na żywo.
- Poprawki awarii, statystyk, eksportu wyników, presetów, kopiowania wydarzeń i losowania.
- Mniejszy interfejs na życzenie, przebudowane wyniki pojedynczych skoków i nowa strona „O autorze”.
- Przyspieszona automatyczna symulacja konkursów i pojedynczych skoków, z zachowanym anulowaniem.
- Zachowany alternatywny interfejs Qt Widgets.

To skrót do redakcji, nie deklaracja usunięcia wszystkich istniejących błędów.

## 1. Domyślny interfejs, styl i wygląd

### 1.1. Nowa warstwa Qt Quick

- Standardowy projekt qmake uruchamia nowy interfejs Qt Quick.
- Widoki nowego interfejsu znajdują się w source/UI/Quick i zostały zbudowane niezależnie od wcześniejszego eksperymentu source/UI/Competition/Quick.
- Zachowano istniejące kontrolery, modele i komponenty Qt Widgets. Nowy widok wywołuje ich operacje przez adaptery.
- Uruchomienie z parametrem --widgets wybiera dotychczasowy interfejs. CONFIG+=widgets_only tworzy wariant bez zależności Qt Quick/QML.
- Nie przeprowadzono migracji projektu do Qt 6. Weryfikacja opisana w raportach używa Qt 5.15.3.
- Zakres nowej prezentacji obejmuje stronę startową, konfigurację i przebieg konkursu, pojedyncze skoki i wyniki, sezony, kalendarze, klasyfikacje, statystyki, bazy danych, ustawienia, generator formy, szczegóły skoku, pomoc i dodatkowe okna.

### 1.2. Charakter wizualny i typografia

- Przyjęto sportowy, alpejski kierunek: granat, kobalt, ciepłe jasne tła i wyrazisty akcent chartreuse.
- Dodano wspólny system kolorów semantycznych, typografii, odstępów, obramowań, stanów kontrolek i hierarchii.
- Duże nagłówki korzystają z Barlow Condensed ExtraBold, tekst i kontrolki z Barlow, a dane liczbowe z IBM Plex Mono.
- Fonty są dostarczane z aplikacją wraz z licencjami SIL Open Font License, więc wygląd nie zależy od ich instalacji w systemie.
- Wyróżniki obejmują asymetryczne narożniki, numerowane oznaczenia sekcji, wyraźne akcje główne i spokojniejszą prezentację akcji pomocniczych.
- Etykiety polskie i angielskie mają krótki, rzeczowy charakter.
- Dodano jasny i ciemny motyw, trwały wybór skali tekstu oraz ograniczenia animacji.

### 1.3. Strona startowa i wspólne elementy

- Strona startowa używa dotychczasowego logo Sj.Sim; nie zastąpiono go nowym znakiem.
- Bezpośrednio pokazuje trzy tryby: Sezony, Pojedynczy konkurs i Pojedyncze skoki.
- Układ kart dopasowuje się do szerokości i wysokości; duża oprawa strony znika w niskim oknie, aby zachować miejsce na działania.
- Dodano skróty Ctrl+1, Ctrl+2, Ctrl+3 do uruchamiania trybów.
- Baza, ustawienia, pomoc, „O autorze” i zgłaszanie problemu pozostają dostępne ze strony startowej.
- Usunięto niejasne oznaczenie „01 — 03”. Nowa strona autora zawiera portret, biografię, wyróżnione Predazzo Edition, Ski Jump Draft, GitHub/LinkedIn z ikonami oraz podziękowania w osobnym dialogu. Na małym ekranie cały pomniejszony portret jest obok powitania, a treść poniżej korzysta z pełnej szerokości.
- Wygląd jest ustawieniem całej aplikacji, a nie osobnym panelem powielanym w kolejnych oknach.
- Jedna wspólna stopka zawiera oznaczenie wersji i beta. Ekrany zadań oraz krótkie dialogi nie powielają własnych pasków wyglądu i wersji.
- Wersja pochodzi ze wspólnej stałej C++, używanej również przez dotychczasowy interfejs i udostępnianie wyników.

## 2. Nawigacja, responsywność i dostępność

### 2.1. Mniej dialogów

- Duże zadania otwierają się jako strony jednego głównego okna, ze wspólnym Wstecz i ścieżką nawigacji.
- Edytory, generator formy, szczegóły i narzędzia sezonu nie tworzą kolejnych nakładających się głównych okien Quick.
- Powrót zachowuje stan strony nadrzędnej.
- Zachowano kontrakt istniejących kontrolerów C++: ich pętle dialogów i wyniki zatwierdzania nadal obsługują dotychczasowy przebieg programu.
- Potwierdzenia, krótkie wybory wartości i postęp pozostają modalne. Systemowe wybieranie plików pozostaje natywne.
- Zamknięcie strony zwalnia jej scenę Quick; ponowne otwarcie odtwarza widok z aktualnych danych.
- Własność przeniesionych kontrolerów jest powiązana z ich pierwotnym właścicielem.

### 2.2. Dopasowanie do miejsca

- Duże ekrany korzystają z układów listy obok edytora, a mniejsze z przewidywalnych przejść między listą i edycją lub zakładek.
- Formularze, listy, wyniki i długie treści mają własne obszary przewijania.
- Najważniejsze dane konkursu są dostępne razem; nie przeniesiono wyników, listy startowej ani karty skoku do konkurujących zakładek.
- Drugorzędne nagłówki, akcje i odstępy są bardziej zwarte w niskich oknach.
- Istotne listy wykorzystują pozostałą wysokość, zamiast kończyć się w połowie dostępnego obszaru.
- Wyniki pojedynczych skoków mają dedykowany układ bez ogólnego limitu wysokości tabeli. Statystyki są obok na desktopie i w osobnej sekcji w wąskim oknie.
- Opcjonalny „Kompaktowy interfejs” zmniejsza kontrolki i odstępy niezależnie od tekstu. Skala tekstu oferuje 75/85/100/115/130%; ustawienia są zapisywane, a panel dostępności mieści się w małym oknie.
- Wiersze zakładek obsługują przepełnienie, a pola wyboru otwierają listę w górę, gdy poniżej brakuje miejsca.

### 2.3. Dialogi i pola liczbowe

- Standardowe przyciski dialogów mają polskie etykiety, m.in. Tak, Nie, Anuluj, Zapisz, Zamknij, Zastosuj i Ponów. Angielska wersja używa tłumaczeń.
- Zachowano role i identyfikatory przycisków; zmiana etykiety nie zmienia działania potwierdzenia.
- Krótka wiadomość korzysta z szerokości dialogu i jest umieszczona pod tytułem, bez sztucznego zawijania kilku słów do wielu wierszy.
- Tytuł i długa treść mogą się przewijać niezależnie od stale dostępnych przycisków na dole.
- Przyciski dialogu mogą przejść do kolejnego rzędu w małym oknie.
- Rozmiar krótkich okien jest ponownie ograniczany po zmianach natywnej geometrii i po zmniejszeniu głównego okna.
- Przyciski +/− działają od razu przy każdym naciśnięciu. Przytrzymanie rozpoczyna powtarzanie po 250 ms, dalej co 40 ms.
- Obsługiwane są kroki ułamkowe, granice wartości, sufiksy i lokalne formaty liczb.

### 2.4. Klawiatura i dostępność

- Kontrolki pokazują fokus, a operacje mają dostęp klawiaturą.
- W tabelach strzałki nawigują po komórkach, F2 rozpoczyna edycję, spacja przełącza poprawne komórki zaznaczane.
- Ctrl+F otwiera wyszukiwanie aktywnej tabeli, Ctrl+A zaznacza dane, Insert/Delete używają akcji źródłowych tam, gdzie są dozwolone.
- Niestandardowe kontrolki mają nazwy i role dostępności; skreślone noty sędziowskie mają również opis odrzucenia.
- Skala tekstu i ograniczenie animacji są ustawieniami trwałymi.
- Automatyczne testy obejmują także powiększony tekst. Nie jest to certyfikat pełnej zgodności z konkretnym standardem dostępności.

## 3. Tabele, katalogi i edytory danych

- Adaptery tabel odczytują istniejące modele na żądanie, zamiast przepisywać całą bazę do osobnej logiki widoku.
- Zachowano edycję komórek, sortowanie, zaznaczanie, role zaznaczanych pól, dwuklik i akcje kontekstowe źródła.
- W zwykłych tabelach pole wyszukiwania jest widoczne, gdy pozwala na to miejsce; mniejsze obszary mają przycisk Szukaj.
- Pusty filtr pokazuje stan Brak wyników i Wyczyść filtr, umożliwiając szybki powrót do danych.
- Zmiana filtra czyści stare zaznaczenie, aby operacja zbiorcza nie objęła niewidocznych rekordów.
- Licznik zaznaczonych dotyczy wierszy, nie liczby komórek.
- Kursor klawiatury i faktyczne zaznaczenie mają odmienne oznaczenia. Zaznaczenie komórki poza pierwszą kolumną nadal podświetla właściwy wiersz.
- Niskie tabele przenoszą drugorzędne kontrolki do nagłówka.
- Rozróżniono edycję liczby/tekstu od checkboxów, także dla modeli, których domyślne flagi dopuszczają zaznaczanie, ale nie zwracają stanu checkboxa.
- Edytor bazy wykorzystuje pełną wysokość list zawodników, skoczni, zasad i presetów punktacji.
- Na desktopie lista i otwarty formularz dzielą szerokość mniej więcej 38%/62%. Ukryty edytor nie rezerwuje pustej kolumny.
- Długi formularz przewija się niezależnie od listy.
- Podobny układ obejmuje katalogi sezonu, w tym zawodników, skocznie, zasady, klasyfikacje i zapisane listy.
- Lista zapisów symulacji również rośnie wraz z wysokością okna.
- Edytory zawodnika dzielą dane, umiejętności i charakterystyki, a edytory skoczni dane, punktację, parametry i charakterystyki.
- Wskaźniki edytorów bazy zaczynają od nullptr, a indeks ostatnio otwieranego elementu od -1, zamiast niezdefiniowanych wartości.
- Zmiana składu w edytorze drużyny przebudowuje właściwy model członków, tak aby widok korzystał z aktualnej kolejności.

## 4. Listy startowe, zapisane składy i drużyny

- Zastąpiono przenoszenie między starymi listami wspólnym obszarem wyboru i kolejności zawodników.
- Biblioteka obsługuje wyszukiwanie, filtr kraju i bezpośrednie zaznaczanie członkostwa.
- Wybrany skład jest osobną uporządkowaną listą, z Ctrl/Shift, zaznaczeniem grupy, usuwaniem i cofnięciem.
- Dodano przeciąganie całych zaznaczonych grup z zachowaniem kolejności.
- Dostępne są również ruchy klawiaturą, podanie pozycji i alternatywa kliknięcia uchwytu z potwierdzeniem miejsca docelowego.
- Desktop pokazuje bibliotekę i skład obok siebie. Wąskie okno rozdziela wybór i kolejność na widoki.
- Ograniczono nagłówki i paski w najmniejszym oknie, zachowując użyteczne miejsce na zawodników.
- Zapisana lista ma jawne działania Dodaj, Usuń, Kolejność. W małym oknie są dostępne w krótkim panelu Listy.
- Zachowano import list, kolejność na podstawie wyników, oryginalne narzędzia kwalifikacji oraz kontrolki KO.
- Obsługa zespołów korzysta ze wspólnego edytora kolejności, z rozróżnieniem startujących i rezerw.
- Bieżące klucze widoku identyfikują obiekty zawodników, a nie wyłącznie UUID. Chroni to przed pominięciem zawodników z historycznych danych współdzielących zerowy UUID.
- Ten mechanizm nie przepisuje masowo tożsamości zawodników w zapisach.

## 5. Konfiguracja konkursu i wiatr

### 5.1. Przygotowanie konkursu

- Konfiguracja ma uporządkowane sekcje, stałą akcję rozpoczęcia i edytory dostosowane do szerokości.
- W sezonie skocznia, zasady i typ wydarzenia pochodzą z kalendarza i są przedstawione informacyjnie.
- Nie trzeba ponownie wybierać parametrów ustalonych w kalendarzu; przygotowanie skupia się na zawodnikach, belce i warunkach.
- Zachowano ważność wydarzenia ustaloną w kalendarzu.
- Walidacja źródłowa nadal sprawdza gotowość list i składów.

### 5.2. Wyliczona belka

- Pole belki pokazuje od razu konkretny numer wyliczony dla przygotowanego wydarzenia; nie wyświetla specjalnej wartości tekstowej Auto.
- W pojedynczym konkursie wybór skoczni ponownie wylicza belkę.
- Przycisk automatycznego doboru wpisuje wyliczoną liczbę do pola.
- Sugestia korzysta z najsilniejszego zawodnika, parametrów skoczni, zasad, średniego profilu wiatru i śniegu.
- Podgląd jest deterministyczny; nie losuje zakończenia skoku ani not i nie zapisuje wyników.
- Użyto ograniczonego wyszukiwania w zakresie pola, z celem około 97% HS, zamiast nieograniczonej pętli prób.
- Sekcja Wiatr pokazuje aktualną sugestię, różnicę względem ustawionej belki i dyskretny przycisk Zastosuj.
- Zmiana wiatru przelicza sugestię, ale pozostawia ustawioną liczbę do świadomego zastosowania.

### 5.3. Edytor wiatru

- Widać podsumowania wszystkich sektorów i cztery parametry aktualnie wybranego sektora.
- Obsługiwane są zmiany pojedynczego sektora, zastosowanie ustawień do wszystkich, kopiowanie sektora, profile Przedni/Tylny/Cisza, losowanie i cofnięcie.
- Losowanie pozwala określić kierunek i zmienność lub użyć ich wartości losowych.
- Poprawiono zapis zbiorczy ustawień: kontrolki nie wskazują już na lokalną zmienną, która przestaje istnieć po zamknięciu operacji.
- Zmiany są przenoszone do prawidłowych kontrolek przed zatwierdzeniem. Anulowanie zachowuje poprzedni profil.
- Po zaakceptowaniu nowych ustawień w konkursie aktualizują się rzeczywisty wiatr i linie docelowe.

## 6. Ekran konkursu

### 6.1. Hierarchia i wykorzystanie przestrzeni

- Główny obszar obejmuje zawodnika na belce, kartę oglądanego skoku, wyniki i listę startową.
- Drugorzędne narzędzia wydzielono do kategorii Przebieg, Warunki, Skok.
- Każda kategoria ma bezpośredni przycisk w pasku sterowania. Kliknięcie odczytu wiatru lub śniegu otwiera od razu Warunki.
- Tryb Na żywo ma przełącznik w głównym pasku.
- W szerokim oknie kontrolki mieszczą się w jednym rzędzie.
- Karty mają wysokość wynikającą z treści i są wyrównane do góry. Krótsza karta nie rozciąga tła do wysokości sąsiedniej.
- Ograniczono łączną szerokość kart do 1052 px, kartę skoku do 720 px, a zawodnika do 320 px. Tabele wykorzystują pozostałe miejsce szerokiego monitora.
- W wąskim układzie karty są w jednej kolumnie, z kartą skoku wcześniej.
- Panel opcji nie przewija ani nie przeciąga pustego obszaru, gdy wszystko się mieści. Po zmianie dłuższej treści na mieszczącą się pozycja wraca do początku. Przewijanie pozostaje włączone przy rzeczywistym przepełnieniu, z zatrzymaniem na krawędziach; sprawdzają to rzeczywiste gesty i kółko myszy.

### 6.2. Górny pasek

- Flaga kraju jest przy nazwie skoczni i HS.
- Odczyt wiatru ma kolor zielony dla wartości dodatniej, czerwony dla ujemnej i neutralny dla zera.
- Usunięto tekst Start x/N.
- Pozostają informacje o serii, grupie drużynowej i śniegu.
- Zamiast powielania lidera dodano największy awans względem końca poprzedniej serii, np. Jan Kowalski ↑12.
- Statystyka pojawia się od drugiej serii, po rzeczywistym skoku i dodatnim awansie.
- Poprzednie miejsca są obliczane na całej stawce z poprawną obsługą remisów 1,1,3. Od różnicy miejsc odejmuje się liczbę zawodników oczekujących na skok w aktywnej serii/grupie; DNS i DSQ nie są oczekującymi. Dzięki temu nie pojawia się pozorny awans wynikający wyłącznie z niepełnej serii.
- Przy równym awansie wskaźnik wybiera wyżej sklasyfikowanego, następnie nazwę. Dopisek (+N) sygnalizuje pozostałych; podpowiedź pokazuje wszystkich oraz miejsca przed i po.
- W drużynówce podstawą jest pełna poprzednia seria drużyny. Wskaźnik aktualizuje się także podczas grup.
- W trybie na żywo nowa wartość czeka na ujawnienie wyniku.

### 6.3. Karta oglądanego skoku

- Usunięto mały nagłówek Ostatni skok. Karta może pokazywać również wybrany inny skok.
- Usunięto zmieniające się nagłówki Lot, Odległość, Lądowanie, Noty, Wynik; dane nadal są ujawniane etapami.
- Bez otwierania opcji widać zawodnika z flagą, wyraźne miejsce, odległość, punkty, pięć not i ich sumę, wiatr, belkę, lądowanie oraz składniki i sumę rekompensat.
- Pole „W serii” pokazuje miejsce za oglądany skok w jego serii, również jeśli wcześniej wyłączono tę daną w szczegółach Widgets. Dla drużyn dostępne są też miejsca w grupie i konkursie, suma punktów, DSQ i informacja 95% HS — gdy istnieją w danych. Miejsce w serii jest ujawniane dopiero z wynikiem skoku.
- Opcjonalne informacje silnika wybrane w ustawieniach również trafiają do karty.
- Pięć not ma oddzielne pola. Dokładnie jedna najniższa i jedna najwyższa są skreślone; jednakowe noty nie powodują odrzucenia większej liczby ocen.
- Wiatr i suma rekompensat mają niezależne kolory oraz znaki. Uwzględnia to sytuację, w której znak rekompensaty jest przeciwny do znaku wiatru.
- Strata do lidera jest dyskretna i należy do oglądanego wyniku, a nie do następnego zawodnika.
- Karta zawodnika na belce pokazuje miejsce i stratę punktową po poprzedniej serii. Wyniki z bieżącej serii nie zmieniają tej różnicy. Strata jest dodatnia, przewaga nad najlepszym rywalem ujemna, remis wynosi zero. W pierwszej serii informacja jest ukryta.
- Podczas lotu miejsce w karcie skoku pozostaje puste/niewyznaczone, a końcowy wynik nie jest ujawniany przedwcześnie.
- Miejsce i strata są przekazywane jako bezpieczne podsumowanie wartości, bez przechowywania w adapterze niestabilnego wskaźnika do sortowanego wektora.

### 6.4. Wyniki i lista startowa

- W domyślnych wynikach indywidualnych widać odległości z poszczególnych serii i sumę punktów. Punkty pojedynczej serii pozostają opcjonalne.
- W drzewie wyników drużynowych dodano odległości zawodników, z oznaczeniem upadku.
- Aktualna grupa KO pozostaje przy liście startowej.
- Zmieniono aktualizację modelu listy startowej: zmiana składu, kolejności, serii lub grupy publikuje spójny snapshot pod prawidłowym resetem modelu.
- Zmiana zwykłego statusu skoku używa powiadomienia o danych.
- Kursor i widoczny zawodnik podążają za faktycznym stanem menedżera.
- Naprawiono rozmiary i położenie panelu KO, aby nie zasłaniał głównych wyników ani karty skoku.

## 7. Silnik punktacji, linie docelowe i prezentacja na żywo

### 7.1. Linie prowadzenia i awansu

- Wyznaczanie celu odbywa się co pół metra z uwzględnieniem bieżącej sumy punktów, a nie tylko ostatniego skoku.
- Uwzględniane są wyniki indywidualne i drużynowe, remisy, pozostałe miejsca awansu, KO i lucky losers.
- Obliczenia respektują włączone rekompensaty wiatrowe i za belkę oraz zasadę 95% HS przy belce trenera.
- Nieistniejący cel jest oznaczony kreską, np. dla pierwszego zawodnika, zamiast fikcyjnego 0 m.
- Linie są obliczane przed pierwszym skokiem i odświeżane po zmianie warunków lub belki, również dla pierwszego zawodnika późniejszej serii.
- Po skoku na żywo cel jest odświeżany dla następnego zawodnika.
- Noty pozostają estymacją na podstawie ostatnich skoków. Linia nie zna przyszłej decyzji sędziów.

### 7.2. Wiatr i punktacja

- Usunięto losowość z obliczania średniego wiatru; losowość pozostaje w generatorze warunków.
- Punktacja i widoczne zakresy sektorów korzystają z jednego obliczenia długości sektora, także dla jednego sektora i ułamkowych wartości K.
- Ważenie ekspozycji na wiatr przy krótszym skoku nie modyfikuje surowego profilu wiatru.
- Poprawiono proporcję częściowego sektora używaną do punktacji.
- Zmierzony wiatr pozostaje zapisany także przy wyłączonych punktach rekompensaty.
- Metadane 95% HS są zachowane również przy wyłączonych punktach za belkę.
- Prezentowany próg 95% HS zaokrągla się w górę do dozwolonego pół metra.
- Poprawiono znalezienie obu skrajnych ocen przy rosnącym zestawie not sędziowskich.
- Dokładne ustawienie prawdopodobieństwa DSQ na 0 w manipulatorze wyłącza DSQ; 1 nadal oznacza pewność, a -1 dziedziczenie wartości bazowej.
- Ujemne indeksy odczytu wyników są odrzucane.

### 7.3. Tryb na żywo

- Wyniki i ich adaptery są zamrożone podczas kolejnych etapów prezentacji skoku.
- Dodatkowe informacje karty i wskaźnik największego awansu nie zdradzają końcowego wyniku przed ujawnieniem.
- Zachowano preferencje widoczności informacji o skoku.
- Zamknięcie w trakcie prezentacji kończy wewnętrzne oczekiwanie bez czekania na pełną animację.
- Zredukowano powtarzalne komunikaty o przejściu do grupy, serii i zakończeniu w nowym interfejsie — właściwy stan i akcja są dostępne w ekranie.

## 8. Sezony i kalendarze

### 8.1. Edytowany kalendarz a aktywny kalendarz

- Wybór kalendarza do oglądania i edycji nie przełącza już kalendarza rozgrywanego sezonu.
- Aktywacja odbywa się jawnym przyciskiem Ustaw jako aktywny, z oznaczeniem aktywnego kalendarza.
- Szeroki ekran ma listę kalendarzy po lewej, mniejszy rozwijaną listę. Przy bardzo małej wysokości akcje dodatkowe trafiają do menu.
- Nowy kalendarz pojawia się od razu, jest wybrany, ma edytowalną nazwę i dostęp do dodawania konkursów lub presetu.
- Kalendarz i jego klasyfikacje stanowią wspólny obszar pracy z sekcjami Konkursy i Klasyfikacje.
- Usunięcie ostatniego kalendarza czyści modele i wskazanie edycji.
- Poprawiono powiadomienia listy kalendarzy: dodanie/usunięcie danych odbywa się pomiędzy właściwymi sygnałami modelu, bez podwójnej zmiany kolekcji.
- Podgląd klasyfikacji w Przeglądzie synchronizuje się od razu po aktywacji, także do pustego stanu.

### 8.2. Edycja i synchronizacja

- Kliknięcie konkursu zaznacza go; nie otwiera automatycznie edytora w małym oknie.
- Edytuj, Enter, dwuklik lub strzałka wiersza otwierają edycję. Powrót prowadzi na listę.
- Ctrl/Shift wybiera wiele konkursów, z widocznym zakresem i czyszczeniem zaznaczenia.
- Fokus jest zachowywany po odświeżeniu danych.
- Zmiany pól działają bez dodatkowego zatwierdzania pojedynczego konkursu i obejmują całe zaznaczenie.
- Dotyczy to m.in. skoczni, zasad, typu, ważności, treningów, serii próbnych, klasyfikacji i kwalifikacji.
- Dodaj wiele tworzy od 1 do 100 konkursów w jednej operacji i zaznacza nowe elementy.
- Treningi i seria próbna zachowują własne zasady i ważność. Wybranie reguł niestandardowych otwiera ich edytor bez dodatkowego szukania.
- Zmiany kontrolek liczbowych, list wyboru, pól tekstowych i przełączników zgłaszają odświeżenie przez sygnały. Pozostaje również okresowy fallback dla danych natywnych.
- Rozdzielono aktualizację kalendarza edytowanego, aktywnego i granicy rozegranej historii.
- Zmiany trafiają od razu do danych otwartego sezonu. Zapisz do pliku nadal zapisuje cały stan na dysku; nie należy opisywać tej przebudowy jako automatycznego zapisu każdej operacji na dysk.

### 8.3. Przestawianie, kopiowanie i usuwanie

- Przeciąganie uchwytu przestawia cały blok: treningi, serię próbną i główny konkurs.
- Przeciągnięcie elementu wielokrotnego zaznaczenia przenosi cały wybrany zestaw z zachowaniem kolejności.
- Widać miejsce upuszczenia, a lista przewija się przy krawędziach. Dostępna jest też alternatywa klawiaturowa Ctrl+↑/↓.
- Kopie konkursów w nowym edytorze Quick dostają nowe tożsamości i nie zawierają rozegranej historii.
- Kopiowanie zachowuje ustawienia wszystkich elementów bloku, w tym ważność treningów i serii próbnej, ich skocznie, wyjątkową liczbę serii i przypisania.
- Edycja kopii wydarzenia nie modyfikuje oryginału.
- Wewnętrzne źródła kwalifikacji zaznaczonego zestawu są przepinane na odpowiednie kopie, także dla relacji oczekujących.
- Źródła poza kopiowanym zestawem pozostają źródłami zewnętrznymi. Nie przenosi się powiązań zwrotnych kopii do oryginalnego bloku.
- Usunięcie źródła kwalifikacji jest blokowane z listą zależnych konkursów; najpierw trzeba zmienić zależności.
- Rozegrana historia i zależności potrzebne rozegranym wydarzeniom są chronione przed niedozwolonymi zmianami.

### 8.4. Kwalifikacje i zmiana kolejności

- Źródłem może być wcześniejszy konkurs odpowiedniego typu. Konkurs N nie wybiera przyszłego konkursu jako normalnego nowego źródła.
- Poprawiono odwrócone porównania przy wyborze źródła w nowym interfejsie.
- Po przeniesieniu źródła za zależny konkurs powiązanie jest zawieszane, a nie usuwane.
- Oba odpowiednie wiersze sygnalizują problem. Zależnego konkursu oraz związanego bloku przygotowawczego nie można rozpocząć, dopóki warunek nie jest poprawiony.
- Zabezpieczenie działa również przy bezpośrednim wywołaniu akcji kontrolera, poza stanem przycisku.
- Przywrócenie prawidłowej kolejności automatycznie uaktywnia zachowaną relację.
- Oczekujące źródło pozostaje możliwe do świadomej zmiany lub wyczyszczenia.
- Zapis i ponowny odczyt zachowują oczekujące powiązanie przez tożsamość wydarzenia.
- Presety i duplikowane bloki poprawnie mapują źródła na nowe obiekty.
- Porządkowanie przypisań klasyfikacji nie usuwa już elementów wektora w trakcie iteracji w sposób unieważniający tę iterację.

### 8.5. Cofnij/Ponów

- Dodano wielopoziomową historię operacji, działającą również pomiędzy edytowanymi kalendarzami bieżącego sezonu.
- Historia obejmuje pola, operacje masowe, przestawianie, kopiowanie, usuwanie, nazwy, tworzenie/usuwanie kalendarzy, aktywowanie innego kalendarza, klasyfikacje i zastępowanie presetem.
- Cofnięcie przywraca właściwy edytowany kalendarz oraz zaznaczenie, zamiast działać na przypadkowo aktualnej liście.
- Tożsamości konkursów i klasyfikacji oraz powiązania pozostają stabilne.
- Historia nie może nadpisać zawodów rozegranych lub odwołanych po wykonaniu snapshotu.
- Zmiana sezonu i niezgodne zewnętrzne zmiany unieważniają odpowiednią historię.
- Zachowane obiekty potrzebne do Cofnij/Ponów są uwzględniane przy zwalnianiu danych, również podczas zamykania edytora.

### 8.6. Dolny pasek następnego wydarzenia i narzędzia

- Pasek korzysta z aktywnego kalendarza, niezależnie od aktualnie edytowanego.
- Pokazuje pełną flagę kraju skoczni, nazwę, rodzaj wydarzenia, HS i format indywidualny/drużynowy.
- Dla treningu uwzględnia właściwy rodzaj/indeks wydarzenia.
- Akcja od razu odzwierciedla aktualny stan. Pusty kalendarz prowadzi do edycji, zamiast przypadkowo do przejścia sezonu.
- Generator formy ma bezpośrednie wejście oraz wejście w Narzędziach.
- Naprawiono menu narzędzi: jawne wstawianie pozycji i atomowa aktualizacja zapobiegają znikaniu wpisów podczas odświeżania i kliknięcia.

## 9. Klasyfikacje i presety kalendarzy

### 9.1. Edytor klasyfikacji

- Klasyfikacje są edytowane we wspólnym obszarze wybranego kalendarza.
- Lista i formularz wykorzystują dostępną wysokość; małe okno ma przejście między listą i edycją.
- Nazwa, typ i punktacja aktualizują dane bez osobnego przycisku zatwierdzania całej klasyfikacji.
- Wskaźnik klasyfikacji startuje w poprawnym stanie, a zastosowanie edycji opiera się na tożsamości obiektu zamiast dawnym indeksie wiersza.
- Zmiana kalendarza nie pozostawia formularza podpiętego do innej klasyfikacji.
- Użyta w rozegranych wynikach klasyfikacja pozwala zmienić nazwę, ale blokuje zmianę typu i punktacji.
- Utwórz kopię tworzy nową tożsamość bez historycznych wyników i pozwala edytować reguły punktowania.
- Usuwanie użytej lub powiązanej klasyfikacji jest blokowane.
- Model tabeli punktacji faktycznie zmienia dane między powiadomieniami wstawiania/usuwania wierszy.
- Zmiana presetu/punktów używa resetu modelu. Obsługiwane są punkty ułamkowe i lokalny przecinek dziesiętny zamiast obcięcia do liczb całkowitych.
- Nieprawidłowe indeksy presetów i dane nieliczbowe/nieskończone są odrzucane.
- Zmiana źródła podglądu wyników klasyfikacji emituje reset modelu.

### 9.2. Bezpieczniejsze presety

- Zastąpienie kalendarza presetem zachowuje pełny blok wydarzeń i może być cofnięte/ponowione.
- Rozegrane lub odwołane wydarzenia blokują zastąpienie presetem.
- Import odtwarza klasyfikacje oraz mapuje powiązania wydarzeń i każdą skocznię na obiekty bieżącego zapisu.
- Brak skoczni pasującej nazwą/HS ma kontrolowany fallback do skoczni dostępnej w zapisie.
- Anulowanie wyboru nie zastępuje danych ani nie niszczy historii.
- Serializacja presetu usuwa lokalne identyfikatory skoczni wyłącznie z JSON-a. Nie zeruje już żywych przypisań skoczni w obiekcie presetu.
- Wyniki skopiowanych konkursów są przypinane do nowych wydarzeń zarówno przy tworzeniu, jak i imporcie presetu. Nowe skoki nie wskazują na oryginalny konkurs ani szablon.
- Zachowano nazwane referencje skoczni, niezależną ważność i zasady wydarzeń pomocniczych.

## 10. Generator formy

- Zmieniono układ na ograniczony do okna obszar tabeli z widoczną akcją generowania.
- Na małym/niskim ekranie parametry mają własną sekcję, a przyciski nie znikają poniżej obszaru.
- Parametry są szkicem; Wszyscy/Zaznaczeni określa zakres, a Zastosuj parametry wykonuje zmianę.
- Zmiana zaznaczenia nie przełącza zakresu automatycznie na wszystkich.
- Wczytanie presetu również przygotowuje szkic i wymaga zastosowania.
- Niezastosowany szkic blokuje generowanie ze starymi parametrami.
- Licznik wszystkich zawodników odnosi się do całej listy, niezależnie od filtra.
- Zachowano bezpośrednią edycję ustawień w komórkach tabeli.
- Odrzucane są nieliczbowe wartości, NaN i sprzeczne granice.
- Zbiorcza zmiana granicy dostosowuje drugą granicę, gdy przedział byłby odwrócony.
- Pozostaw zaznaczonych poprawnie zachowuje wszystkich, także przy zaznaczeniu całej tabeli. Brak zaznaczenia nie usuwa zawodników.
- Obsługa zastępowania presetu wyszukuje presety generatora formy, a nie inną kolekcję.
- Widoki wyboru presetów mieszczą się w aplikacji, mają widoczne przyciski i obsługę długiej listy klawiaturą.
- Generowanie wymaga poprawnego zapisu i co najmniej jednego zawodnika.
- Wariant Widgets zachowuje wcześniejsze automatyczne stosowanie parametrów; jawny szkic jest przebiegiem nowego interfejsu.

## 11. Statystyki, archiwa, wykresy i szczegóły

- Archiwalne miejsca zawodników i drużyn poprawnie pomijają miejsca po remisach: 1,1,3.
- Filtry początku/końca zakresu wskazują bezpośrednio konkurs z widocznego wpisu, również po połączeniu kalendarzy i przy nierozgranych wydarzeniach.
- Nagłówki sezonów/kalendarzy nie udają pozycji konkursu.
- Przebudowa filtrów klasyfikacji usuwa stary komplet kontrolek bez unieważniania iteracji i duplikowania checkboxów.
- Brak ocen, rekompensat lub poprawnych skoków nie tworzy średnich NaN. Nieokreślone wartości są pomijane lub oznaczane kreską.
- Statystyki małych konkursów, również poniżej dziesięciu wyników, nie wymagają nieistniejącego dziesiątego miejsca.
- Remis zwycięzców jest prawidłową zerową przewagą; opisy wskazują właściwy sezon i kalendarz.
- Brak presetów punktacji nie wywraca rankingu. Ranking za miejsca pozostaje pusty, a inne dostępne zestawienia działają.
- Pusty wynik filtra czyści tabele, rekordy, dane eksportu i etykiety, zamiast pozostawiać poprzednie dane.
- Pusty kalendarz, brak granic i odwrócony zakres dają bezpieczny pusty wynik.
- Szczegóły skoku bez zawodnika/skoczni lub bez wyniku w danym konkursie mają stan braku danych i wyłączone niedostępne akcje.
- Odczyt miejsca skoku spoza wyników nie indeksuje przez -1.
- DNS bez zapisanych skoków oraz puste wykresy zawodnika nie odczytują pierwszego elementu pustej listy.
- Wykresy sprawdzają osie i indeksy punktów. Średnie odnoszą się do faktycznie pokazanych danych.
- Quick prezentuje istniejące serie wykresów, obsługując zoom/reset i wybór punktów, także klawiaturą, z zachowaniem źródłowych akcji inspekcji.

## 12. Trwałość danych, awarie i losowanie

### 12.1. Crash po Zakończ konkurs

- Odtworzono błąd odświeżenia pozostawionego adaptera po zwolnieniu menedżera konkursu; debugger wskazał QuickBridge::refresh.
- Okna konkursu i kwalifikacji są niszczone przed danymi symulacji.
- Menedżery mają właścicieli RAII, a kopie skoczni kontrolowany zakres życia.
- Wcześniejszy powrót po anulowanych kwalifikacjach zwalnia te zasoby.
- Aktualizacja rekordów omija zawodnika nieobecnego w globalnej bazie zamiast dereferencji nullptr.
- Regresja kończy konkurs przez widoczny przycisk i sprawdza usunięcie okna oraz adapterów, zamiast tylko wywoływać accept.

### 12.2. Eksport CSV

- Powtarzająca się nazwa, np. dwa pojedyncze konkursy tej samej minuty, nie nadpisuje poprzednich wyników.
- Eksport używa tworzenia wyłącznie nowego pliku. Przy kolizji dodaje kolejno (2), (3) itd.
- Zabezpiecza to również przed równoczesnym utworzeniem pliku o wybranej nazwie.
- Katalog jest składany przez QDir i nie wymaga końcowego separatora.
- Błąd otwarcia podaje faktyczny plik i przyczynę systemową.

### 12.3. Zakresy losowania

- Stały zakres, np. 7–7, zwraca 7 zamiast nieprawidłowego 0.
- Przedziały ujemne zakończone zerem nie są sprowadzane do zera.
- Bardzo wąskie przedziały dziesiętne nie są niszczone przez rzutowanie granic na int.
- Dokładność dziesiętna jest zachowana tam, gdzie zakres zawiera wartości na tej siatce. W przeciwnym razie generator zachowuje zakres z dodatkowymi cyframi.
- Losowanie całkowite obejmuje obie granice; zmiennoprzecinkowe przedział [min,max).
- INT_MAX nie wymaga obliczenia przepełniającego max+1. Obsługiwany jest pełny zakres int i duże skończone zakresy double.
- Zastąpiono budowanie tysiąca pomocniczych losowań bezpośrednim pobraniem próbki. Zmiana dotyczy poprawności i kosztu tych narzędzi losowania, nie obietnicy odtworzenia dawnych losowych wyników.

## 13. Format danych i zgodność

- Zachowano modele sezonów, konkursów i list oraz istniejący interfejs Widgets.
- Dodano opcjonalne pole pending-advancement-competition-id dla zachowywanych kwalifikacji oczekujących.
- Starsze pliki bez tego pola są nadal obsługiwane przez nowy kod.
- Starsza wersja aplikacji nie obsługuje nowego pola; po otwarciu i ponownym zapisaniu przez nią oczekujące relacje mogą zostać pominięte. Nie należy obiecywać pełnej zgodności w obie strony bez osobnych testów.
- Presety eksportują skocznie przez ich istniejące referencje nazwy/HS, bez lokalnych identyfikatorów z konkretnego zapisu.
- Nowe kopie wydarzeń i klasyfikacji otrzymują nowe tożsamości.
- Poprawki punktacji dotyczą kolejnych obliczeń; nie wykonano masowego przepisywania historycznych wyników zapisanych w istniejących plikach.
- Atomowy zapis i część zabezpieczeń wczytywania sezonu istniały już na porównywanym main. Nie należy przedstawiać ich w całości jako nowych funkcji tej gałęzi.
- Publiczne potwierdzenie zgodności powinno opierać się także na rzeczywistych zapisach użytkowników, oprócz testowych kopii danych.

## 14. Architektura i narzędzia deweloperskie

- Dodano QuickUi/QuickBridge i stabilne węzły prezentacji do odwzorowania istniejących kontrolek na Quick.
- QuickTableModel zachowuje kontrakt modeli, edycję, zaznaczenie, sortowanie i filtr, bez tworzenia drugiego niezależnego silnika danych.
- QuickRoster i QuickCalendar obsługują przebudowane przebiegi list i kalendarzy.
- Dodano wspólny motyw, własne kontrolki, powierzchnie, adaptery wykresów oraz zasoby fontów.
- Zamrożenie modeli podczas prezentacji na żywo jest sterowane sygnałem etapu z kontrolera konkursu.
- Typy z zależnością cykliczną, np. zagnieżdżony Node w obszarze klasyfikacji kalendarza, są ładowane przez dynamiczny Loader, aby nie unieważniać typu QML.
- Pomocnik geometrii Widgets pomija kontrolery przeniesione do głównego okna, zamiast traktować je jak niezależne okna ekranowe.
- Dodano skrypty budowania, uruchamiania i testowania nowego interfejsu.
- Zachowano opcjonalny wcześniejszy eksperyment konkursowy i jego skrypty/testy. Nie jest domyślnym interfejsem i nie należy prezentować go jako drugiej równoległej nowej wersji produktu.
- Dodano lokalne instrukcje narzędzi Qt w .agents/skills oraz skills-lock.json: QML, UI design, CMake, testy, review, dokumentacja, profiler, Canvas2D i narzędzia Figma wraz z zasobami/licencjami.
- Same instrukcje Qt 6, Canvas2D czy Figma nie oznaczają użycia tych technologii w działającym wydaniu. Są zasobami deweloperskimi.
- Rozszerzono angielski katalog tłumaczeń dla nowych ekranów, kontrolek, metryk i błędów.

## 15. Testy i dowody

### 15.1. Zakres automatyczny

- Testy działają na jednorazowych kopiach baz, ustawień i zapisów.
- Przechodzą przez rzeczywiste kontrolery i modele, z kliknięciami myszą, klawiaturą, menu i dialogami.
- Obejmują pełne konkursy indywidualne, KO i drużynowe, automatykę, skoki na żywo, zamknięcie w trakcie prezentacji i zakończenie przyciskiem.
- Porównują każdy widoczny wiersz i kursor listy startowej ze stanem menedżera.
- QAbstractItemModelTester sprawdza kontrakty modeli listy startowej i punktacji.
- Obejmują wszystkie cztery narzędzia sezonu, generator formy obiema ścieżkami, nawigację, kalendarze, klasyfikacje, presety, zależności, historię i przeciąganie grup.
- Sprawdzają zakres formy, szkic parametrów, wyszukiwanie, zaznaczenie, zapisane listy, rezerwy i KO.
- Obejmują bazę danych, ustawienia, wyniki pojedynczych skoków, wykresy, szczegóły, postęp i jego anulowanie.
- Sprawdzają przełączanie języka polskiego/angielskiego oraz brak brakujących aktywnych tłumaczeń Quick w poprzednich audytach katalogu.
- Przegląd geometrii obejmuje rozmiary od 360×480 do 2560×1440 w odpowiednich scenariuszach, oba motywy i powiększony tekst.
- Dodatkowe regresje wykazują utratę skoczni podczas serializacji, nadpisanie CSV, błędne losowanie, błędnego właściciela skopiowanych wyników, utratę ustawień bloku i nieprzepięte kwalifikacje.
- Dla sześciu wcześniejszych błędów kopiowania/eksportu/losowania oraz pięciu kolejnych usterek danych najpierw wykonano regresję na wadliwym kodzie, a następnie sprawdzono poprawkę.

### 15.2. Końcowy wynik

| Sprawdzenie po ostatnich poprawkach | Wynik |
| --- | --- |
| Pełny scripts/test-quick-ui.sh | Kod 0; **367 responsywnych renderów** i scenariusze integracyjne. |
| Regresje kopiowania/presetów oraz pięciu kolejnych usterek danych | Kod 0; przed poprawkami każda wykazała opisany błąd. |
| Wyszukiwanie skoczni, poprzednia strata, miejsce skoku i korekta awansu | Zaliczone w pełnym przebiegu, także w małych rozmiarach i Na żywo. |
| Wszystkie cztery akcje automatycznej symulacji i odczyt cech | Zaliczone w pełnym przebiegu; pomiary w --simulation-benchmark. |
| Historia, aktywne/oczekujące kwalifikacje i import presetu | Zaliczone osobno w --workflow-only i w końcowym pełnym przebiegu. |
| Konkursy indywidualne, KO i drużynowe, Na żywo i zakończenie | Zaliczone w pełnym przebiegu. |
| Widgets-only, make -j2 w build-widgets-compat | Kod 0. |
| FAIL, TypeError, ReferenceError i pętle powiązań w końcowym logu | Nie wystąpiły. |
| git diff --check | Bez błędów. |

Pełny log tego przebiegu: /tmp/sj-author-single-final-full.log; log kompilacji Widgets: /tmp/sj-author-single-final-widgets.log. Po końcowym przestawieniu małego portretu autora ponowiono --author-only, kod 0: /tmp/sj-author-header-final.log. To pliki pomocnicze, które system może usunąć. Wyniki i zakres weryfikacji są dlatego zapisane również tutaj i w commitach.

### 15.3. Ograniczenia i historia diagnostyki

- Testy używają Qt 5.15.3 z rendererem programowym offscreen. Nie sprawdzają wszystkich GPU, systemów operacyjnych ani natywnych menedżerów okien.
- Liczba renderów dotyczy zdefiniowanego zestawu scenariuszy, nie wszystkich możliwych stanów aplikacji.
- Nie przeprowadzono pełnej certyfikacji dostępności ani gwarancji bezbłędności całego trybu sezonów.
- We wcześniejszym etapie odnotowano jednorazowe „stack smashing detected”. Następne zwykłe przebiegi i przebiegi pod GDB przeszły; przyczyny tego konkretnego incydentu nie ustalono. Nie należy utożsamiać go automatycznie z później odtworzonym i naprawionym crashem zakończenia konkursu.
- Ostrzeżenia istniejącego kodu C++ i pluginu offscreen nie są równoznaczne z błędami testów. Raport nie deklaruje kompilacji bez żadnych ostrzeżeń.
- Całość nadal wymaga normalnego testowania długich sezonów i zapisów użytkowników przed wydaniem stabilnym.

## 16. Lista kontrolna do testów wydania

1. Uruchom domyślny interfejs oraz alternatywny --widgets.
2. Sprawdź stronę startową, logo, trzy tryby i ich skróty.
3. Zmień motyw, skalę tekstu i ograniczenie animacji, zamknij i uruchom aplikację ponownie.
4. Przejdź przez bazę i długi formularz; powiększ oraz zmniejsz okno.
5. Sprawdź wyszukiwanie, brak wyników, zaznaczenie wiersza przez komórkę, F2, spację i czyszczenie filtra.
6. Zbuduj listę startową z biblioteki, zmień kolejność myszą i klawiaturą, zastosuj zapisaną listę, cofnij.
7. Sprawdź drużynę z rezerwami i ręczną edycję KO.
8. W konfiguracji obejrzyj liczbową belkę; zmień wiatr, porównaj różnicę i zastosuj sugestię.
9. Rozegraj konkurs indywidualny, KO i drużynowy, także automatycznie.
10. Sprawdź odległości w wynikach, aktualizację listy startowej i cele prowadzenia/awansu.
11. Sprawdź pełną kartę skoku, flagę, noty, znaki wiatru/rekompensat, miejsce i stratę.
12. Włącz Na żywo: miejsce i największy awans nie mogą zdradzać wyniku w trakcie lotu.
13. Sprawdź największy awans od drugiej serii, remis awansów i podpowiedź miejsc.
14. Otwórz każdą kategorię opcji bez pośredniego menu; panel nie powinien przesuwać się, gdy wszystko się mieści.
15. Kliknij Zakończ konkurs; rozpocznij kolejny konkurs z konfiguracji. Sprawdź też kwalifikacje i anulowanie.
16. Wykonaj kilka eksportów o tej samej nazwie i potwierdź zachowanie wcześniejszych plików.
17. Utwórz kalendarz sezonu, dodaj wiele wydarzeń, masowo ustaw skocznie, treningi, serie próbne, reguły i klasyfikacje.
18. Przełącz kalendarz edytowany, sprawdź niezmieniony aktywny, a następnie jawnie aktywuj drugi.
19. Przeciągnij pojedynczy blok i zaznaczony zestaw, także na małym ekranie.
20. Ustaw kwalifikację, przenieś źródło za cel, zapisz/wczytaj, przywróć kolejność i sprawdź odblokowanie startu.
21. Spróbuj usunąć źródło z zależnościami.
22. Skopiuj blok z niestandardową ważnością treningów i serii próbnej; zmień kopię i sprawdź niezależność oryginału.
23. Skopiuj dwa powiązane konkursy i sprawdź, że kwalifikacja kopii wskazuje skopiowane źródło.
24. Cofnij/Ponów kilka operacji, także przełączając kalendarze i zastępując presetem.
25. Zapisz i wczytaj preset; rozegraj zawody z zaimportowanego kalendarza.
26. Sprawdź klasyfikację z wynikami: nazwa edytowalna, reguły zablokowane, kopia edytowalna.
27. W generatorze formy zastosuj szkic tylko do zaznaczonych; wczytaj preset i sprawdź jawne zastosowanie.
28. Otwórz wszystkie narzędzia sezonu, archiwa i statystyki, również dla pustych zakresów, małej stawki, remisów i DNS.
29. Sprawdź długi komunikat rekordu, mały dialog presetu i widoczność jego przycisków.
30. Wykonaj dłuższą rozgrywkę z ponownym uruchamianiem i wczytywaniem rzeczywistych zapisów.
31. Otwórz „O autorze” w 360×480: portret obok powitania, pełny kadr, projekty i wszystkie linki, osobne podziękowania.
32. Włącz kompaktowy interfejs i tekst 75/85%, uruchom aplikację ponownie i sprawdź zapamiętanie.
33. Dodaj serię w zasadach i sprawdź domyślne sortowanie bez zmiany poprzednich serii.
34. Uruchom i anuluj N skoków / Serię / Konkurs; wróć ze Skoku do Przebiegu/Warunków i sprawdź brak zbędnego przewijania.
35. W Pojedynczych skokach dobierz belkę dla różnych wiatrów, wykonaj dużą partię i przerwij kolejną, następnie powiększ okno wyników w pionie i sprawdź wszystkie sześć kategorii statystyk.

## 17. Rozwiązania zastąpione podczas prac

Ta sekcja chroni przed skopiowaniem do release notes nieaktualnego opisu z wcześniejszego commitu.

| Wcześniejszy etap | Końcowe zachowanie |
| --- | --- |
| Auto jako specjalna wartość pola belki | Pole zawiera wyliczony numer; automatyczny dobór jest akcją. |
| Lider w górnym pasku | Największy awans względem poprzedniej serii, pomniejszony o liczbę oczekujących. |
| Nagłówek Ostatni skok i nazwy etapów na żywo | Nagłówek usunięty; ujawnianie danych zachowane. |
| Poprzednie miejsce w karcie skoku podczas lotu | Miejsce karty skoku niewyznaczone; poprzednie miejsce pozostaje przy zawodniku na belce. |
| Blokowanie zmiany kolejności psującej źródło kwalifikacji | Zmiana dozwolona, relacja zachowana jako oczekująca, start zależnego wydarzenia zablokowany. |
| Usuwanie nieprawidłowego przyszłego powiązania | Istniejące powiązanie zachowane pod maską do poprawienia kolejności. |
| Wybór kalendarza jednocześnie zmienia aktywny | Edycja i jawna aktywacja są oddzielne. |
| Jednokrokowe cofnięcie | Wielopoziomowe Cofnij/Ponów pomiędzy kalendarzami. |
| Osobna główna karta edycji klasyfikacji w Quick | Klasyfikacje w obszarze wybranego kalendarza. |
| Dodatkowe okna z własnym wyglądem i wersją | Jedna nawigacja, wspólny wygląd i oznaczenie wersji. |
| Bezpośrednie accept w teście końca konkursu | Kliknięcie widocznego przycisku i kontrola zniszczenia adapterów. |
| Przepisanie starego eksperymentu konkursowego | Nowa domyślna warstwa source/UI/Quick; eksperyment pozostaje opcjonalny. |

## 18. Powiązane raporty szczegółowe

- [Autor, dostępność, wyniki i optymalizacja pojedynczych skoków](AUTHOR_AND_SINGLE_JUMPS_2026-10-22.md).
- [Pięć kolejnych poprawek danych: punkty, CSV, odczyt sezonu, historia drużynowa i następny sezon](FIVE_DATA_FIXES_2026-10-22.md).
- [Wyszukiwanie skoczni i aktualne informacje konkursu](COMPETITION_AND_HILL_SEARCH_2026-10-22.md).
- [Optymalizacja masowej symulacji i silnika — pomiary przed/po](SIMULATION_PERFORMANCE_2026-10-22.md).

- [Migracja i architektura](../QUICK_UI_MIGRATION.md).
- [Pierwsze 10 błędów i 3 dodatkowe awarie](BUGFIX_AUDIT_2026-10-21.md).
- [Pięć przebudowanych przepływów UX](UX_IMPROVEMENTS_2026-10-21.md).
- [Wykorzystanie miejsca i szczegóły karty skoku](SPACE_AND_LAST_JUMP_2026-10-21.md).
- [Wcześniejsze dopracowanie konkursu](COMPETITION_REFINEMENTS_2026-10-21.md) — z korektami końcowymi opisanymi tutaj.
- [Przebudowa sezonów i kalendarzy](SEASON_WORKSPACE_2026-10-22.md).
- [Największy awans](COMPETITION_ADVANCEMENT_2026-10-22.md).
- [Crash zakończenia pojedynczego konkursu](COMPETITION_FINISH_CRASH_2026-10-22.md).
- [Serializacja presetu, CSV i losowanie](THREE_SMALL_FIXES_2026-10-22.md).
- [Trzy dodatkowe poprawki kopiowania kalendarzy](CALENDAR_COPY_FIXES_2026-10-22.md).
- [Historyczny punkt przerwania prac](UX_REWORK_CHECKPOINT_2026-10-21.md).

Dalsze załączniki są wygenerowane z Git dla wskazanego commitu. Pozwalają odnaleźć wszystkie zmienione pliki, również narzędzia i wcześniejszy eksperyment, bez opierania się na pamięci z rozmowy.


## 19. Uzupełnienia po wznowieniu prac

### 19.1. Wyszukiwanie skoczni

- Pole wyszukiwania znajduje się wewnątrz wyboru skoczni, wyłącznie w edytorze kalendarzy i konfiguracji pojedynczego konkursu.
- Obsługuje klawiaturę, brak wyników, anulowanie i ponowne otwarcie pełnej listy; wybór używa właściwego indeksu źródłowego.
- Zmiana skoczni uczestniczy w kalendarzowym Cofnij. Wyszukiwarka mieści się również w konfiguracji przy 360×480.

### 19.2. Pięć kolejnych usterek danych

1. Presety punktów zachowują wartości ułamkowe po odczycie JSON.
2. CSV zachowuje kropki w nazwiskach, cytuje separatory/cudzysłowy/nowe linie i obsługuje nieznany kod kraju.
3. Wczytanie sezonu ustala pierwsze nierozgrane wydarzenie według stanu kalendarza zamiast ufać staremu indeksowi.
4. Zmiana obecnej narodowości zawodnika nie usuwa historycznych punktów z jego skoków drużynowych.
5. Konfiguracja następnego sezonu używa kopii roboczej. Anulowanie odrzuca zmiany; zatwierdzenie zachowuje edycje, dodanych skoczków/skocznie/zasady oraz tożsamość istniejących obiektów. Edytory wskazują dane sezonu; edytor zasad jest odłączany przed zastąpieniem ich kolekcji.

Dla każdej usterki wykonano test odtwarzający błąd przed poprawką. Szczegóły, commity i scenariusze użytkownika zawiera raport pięciu poprawek.

### 19.3. Wydajność

- Automatyczne symulacje grupują aktualizacje prezentacji zawodnika i listy startowej, zachowując obliczenia i kwalifikacje każdego skoku.
- Odczyt cech korzysta z poprawnego hasha i wyszukiwania bez kopiowania/odłączania całego zbioru.
- Release wyłącza `qDebug` na etapie kompilacji; ostrzeżenia i błędy nadal są dostępne, a Debug zachowuje logi.
- Pomiar: około 130 skoków w konkursie indywidualnym — mediana 1538 → 67 ms (około 23×); sam silnik, 10 000 skoków — 254 → 87 ms (około 2,9×). To wyniki lokalnego scenariusza, z naturalną zmiennością losowań, bez gwarancji czasu na każdym komputerze.
- Powtarzalny tryb pomiarowy: `scripts/test-quick-ui.sh --simulation-benchmark`. Regresje wszystkich czterech akcji automatycznych należą do pełnego smoke testu i trybu `--bulk-only`.

### 19.4. Dodatkowe sprawdzenie wydania

- Wyszukać skocznię w obu miejscach, anulować inny wybór, cofnąć zmianę w kalendarzu.
- Sprawdzić opcje po przejściu z dłuższej do krótszej treści; mieszcząca się zawartość nie powinna się przesuwać.
- Porównać największy awans z liczbą jeszcze oczekujących skoczków; w drugiej serii sprawdzić poprzednią stratę i miejsce oglądanego skoku w serii, również Na żywo.
- Sprawdzić pięć scenariuszy danych z raportu oraz automatyczny cały konkurs, serię, kilka skoków i grupę drużynową.
- Otworzyć „O autorze”, sprawdzić zdjęcie obok powitania na małym ekranie, oba projekty, profile społecznościowe i osobny dialog podziękowań.

### 19.5. Autor i dostępność

- Nowy ekran autora korzysta ze zdjęcia użytkownika oraz własnej typografii i kolorów aplikacji. Predazzo Edition ma pierwszeństwo i wyróżnienie; Draft zawiera opis rywalizacji online, drużyny gracza i testów beta w 2025. Zachowano zaproszenie do płatnej współpracy i usunięto niepotrzebne końcowe wyliczenie projektów.
- Ikony i dokładne linki GitHub/LinkedIn są pakowane z aplikacją. Podziękowania otwierają osobny, widoczny dialog, zamiast rozsuwać biografię.
- Końcowy układ małego ekranu to cały portret 84×112 albo 120×160 obok powitania oraz pełna szerokość opisu/projektów poniżej. Wcześniejsze ułożenie zdjęcia nad tekstem zostało zastąpione.
- Kompaktowy interfejs, mniejsze rozmiary tekstu, zapis ustawień, dostępny w małym oknie panel wyglądu i usunięcie „01 — 03” uzupełniają wcześniejszą responsywność.

### 19.6. Dalsze poprawki obsługi konkursów

- Nieaktywne Loadery opcji nie zachowują wysokości po odejściu ze Skoku. Krótsze zakładki przestają przewijać i wracają do początku.
- Bezpośrednie N skoków / Seria / Konkurs zastępują nadrzędne menu; w drużynówce jest też Grupa. Panel opcji zamyka się po wyborze, a anulowanie operacji zachowuje stan zawodów.
- Nowe serie mają domyślnie sortowanie listy startowej. Wcześniejsze serie zachowują własne ustawienia; poprawiono także pomylone przypisania ustawień KO do flagi sortowania.

### 19.7. Pojedyncze skoki: belka, wyniki i wydajność

- Dobór belki działa również poza konkursem i zapisuje konkretną liczbę, na podstawie aktualnego zawodnika, skoczni i bazowego wiatru; ręczna wartość pozostaje do kliknięcia.
- Dedykowany ekran wyników wykorzystuje pełną pozostałą wysokość. Zrównoważono kolumny, uporządkowano szczegóły, zachowano wybór/sortowanie i wszystkie sześć kategorii szczegółów/wykresów. Wąskie okna mają oddzielną sekcję statystyk, bez odbierania wysokości tabeli.
- Trzy optymalizacje obejmują grupowanie aktualizacji postępu, ponowne użycie próbek normalnych i odbicie symetrycznego rozkładu połówkowego oraz przygotowanie planu wiatrów/buforów raz na partię. Nie zmieniono współczynników silnika; zmieniła się kolejność zużywania losowych próbek. Zmienny wiatr nadal powstaje dla każdego skoku.
- Lokalna mediana trzech prób: 1000 skoków ze stałym wiatrem 170 → **36 ms**, ze zmiennym 173 → **38 ms**. Koszt przygotowania i pokazania wyników jest dodatkowy. Największy zmierzony zysk daje grupowanie postępu; nie izolowano osobnego zysku pozostałych dwóch zmian. Nie odtworzono 13 sekund użytkownika; pomiary offscreen nie gwarantują takiego czasu na jego GPU.
- Postęp pozostaje możliwy do przerwania, częściowe wyniki są zachowane, a historia wiatrów nie zmienia się po kolejnych partiach.
- Ważność w `SingleJumpsManager` ma zdefiniowane domyślne 5 zamiast niezainicjalizowanej liczby. Ustawienia z konfiguratora pozostają nadrzędne.
- Szczegółowe scenariusze, commity, czasy wszystkich etapów i testy: [raport autora i pojedynczych skoków](AUTHOR_AND_SINGLE_JUMPS_2026-10-22.md).

### 19.8. Przygotowanie środowiska Windows

- Dodano [WINDOWS_SETUP.MD](../WINDOWS_SETUP.MD) w katalogu głównym: komplet zależności, Qt 6.8.3/MSVC 2022 x64 zgodne z konfiguracją CI, instalacja ręczna i przez aqt, środowisko PowerShell, qmake, dane obok pliku wykonywalnego, testy, diagnostyka i pakowanie QML z `--qmldir`. Instrukcja nie zmienia konfiguracji aplikacji; nie wykonywano jej na Windowsie w tej sesji.

## 20. Awans, remisy i ergonomia konkursów — 24.10.2026

- Wspólne wyznaczanie pewnego awansu i pewnego odpadnięcia na podstawie możliwych wyników pozostałych zawodników. Poprawione remisy graniczne, DNS/DSQ, drużyny i KO/lucky loser; dokładny limit kwalifikacji do KO.
- Niezależne oznaczenia następnej serii i kwalifikacji do najbliższego konkursu: neutralne Q, łagodniejsza zieleń, szare odpadnięcie zachowujące czytelny i dostępny wiersz.
- Spójne liczenie ex aequo w dziesiątych punktu i według etapu punktacji, także przy resetach punktów. Stabilna kolejność równych wyników.
- Belka trenera nie zmienia pośrednio belki jury ani bazy serii; poprawna obsługa pierwszego i ostatniego zawodnika oraz kolejnych grup drużynowych.
- Przeciąganie na liście startowej nie gubi źródła podczas szybkiego przewijania. Kółko i przewijanie przy krawędzi działają z podglądem nazwiska, flagi i wielokrotnego zaznaczenia.
- Pola liczbowe zachowują wpisywany tekst mimo odświeżania. Plus/minus uwzględnia roboczy wpis; dotyczy też liczby skoków i wspólnych pól planowania.
- Jednoseryjne kwalifikacje mają właściwy nagłówek. Usunięto wyszukiwanie z bieżącej listy startowej i wyników KO oraz podpowiedź zasłaniającą ekran po kliknięciu lub Spacji.
- Stabilny model kalendarza zachowuje pozycję przewijania po dodaniu wydarzenia. Gęstsze karty, dzień tygodnia przy dacie, Q zamiast zbędnych etykiet i więcej flag skoczni/zawodników.
- Wybór skoczni i zasad aktualizuje się po przełączeniu konkursu, także po wyszukiwaniu i w trybie dat. Inspektor wykorzystuje przestrzeń pod nagłówkiem zamiast losowo ściskać pola do zerowej wysokości.
- Szczegóły, przykłady, zakres regresji i ograniczenia: [COMPETITION_UX_2026-10-24.md](COMPETITION_UX_2026-10-24.md).

## 21. Uproszczony start symulacji i końcowe wyniki — 24.10.2026

- Jednoekranowe tworzenie symulacji: nazwa, rok i wybór paczek skoczków, skoczni i zasad. Bez edycji baz przed sezonem. Widać liczebność paczek, dostępne są daty zawodów oraz zapamiętanie zatwierdzonych wyborów.
- Wybrane paczki są kopiowane do nowego zapisu bez przełączania globalnie aktywnej bazy. Puste paczki blokują tworzenie niekompletnej symulacji. Zachowano konfigurator kolejnego sezonu i komponenty Widgets.
- Puste stany przeglądu sezonu, kalendarzy, klasyfikacji, list zawodników, wyników i wyszukiwania wyjaśniają następny krok. Przyciski pustych stanów mieszczą się także w niskich viewportach; narzędzia bez zastosowania nie zajmują miejsca pustego kalendarza.
- „Utwórz kalendarz” od razu tworzy kalendarz i przechodzi do jego sekcji z edycją nazwy. Przejścia działają także wtedy, gdy ukryta karta Widgets miała już ten sam indeks. Rozróżniono brak kalendarza, brak aktywnego kalendarza i brak konkursów.
- Przed skokiem straty/przewagi liczone są z punktów po wagach bieżącej serii względem rywali po tych samych zakończonych seriach, bez dodatkowych skoków oddanych już w bieżącej serii. KO ma osobną stratę do lidera grupy. Całkowity reset punktów i pierwsza seria wyłączają takie informacje; prezentacja na żywo nie ujawnia ich przed wynikiem.
- Kliknięcie wyniku grupy KO pokazuje kartę właściwego skoczka. Wiersz grupy nie jest używany jako indeks ogólnych wyników.
- Usunięto znacznik ✓ awansu. Q kalendarza znajduje się po prawej stronie treści, z wyraźnym marginesem od krawędzi i odstępem od przycisku edycji.
- Podium pojawia się wyłącznie po zakończeniu konkursu, także zakończonego odwołaniem serii. Kolory wynikają z miejsc z uwzględnieniem ex aequo. Zakończenie pośredniej serii nie tworzy podium, a odwołanie pierwszej nie tworzy miejsc z pustych wyników.
- Raport szczegółowy i wyniki testów: [SIMULATION_ONBOARDING_2026-10-24.md](SIMULATION_ONBOARDING_2026-10-24.md).

## 22. Audyt silnika i równoległe partie skoków — 24.10.2026

- Dziesięć niezależnie odtworzonych i naprawionych błędów: stan JumpData, kontekst symulatora, DSQ przy zerowym mianowniku, kara wysokości lotu, niekompletne wymuszone noty, rekompensata podwyższonej belki trenera, niezależna flaga automatycznej punktacji za metr w JSON, luka dla K169,5, normalizacja kierunków wiatru i zapętlający się ucięty rozkład normalny.
- Każda poprawka ma osobny commit i wybieralną regresję. Ogon losowania sprawdzono również statystycznie.
- Jedna optymalizacja async/multithread: duże niezależne partie pojedynczych skoków od4096 prób, do4 pracowników, z zachowaniem anulowania i wcześniejszych wyników. Snapshoty ustawień i własne RNG zapobiegają współdzieleniu mutowanego stanu.
- W izolowanym pomiarze20k skoków mediana zmalała z359ms dla jednego pracownika do236ms dla czterech, około34%. To pomiar jądra obliczeń, nie całego interfejsu. Nie równoleglono sekwencji zawodników w tym samym konkursie ani drobnych składników pojedynczego skoku.
- Szczegóły każdego błędu, commity i logi: [ENGINE_AUDIT_2026-10-24.md](ENGINE_AUDIT_2026-10-24.md).

## 23. Wersja 2.0.0-beta i końcowe korekty przeglądu oraz strat

- Po audycie silnika ustawiono wspólne oznaczenie aplikacji na **2.0.0-beta**, z osobnym znacznikiem beta w Qt Quick. Commit `b04f612`.
- Przycisk „Dodaj konkurs w kalendarzu” w przeglądzie pustego aktywnego kalendarza rzeczywiście tworzy konkurs, zaznacza go i otwiera edycję. Korzysta z istniejącego backendu i historii zmian. Commit `0fc093d`.
- Skorygowano wcześniejszą decyzję o porównywaniu do lidera z dodatkowym skokiem: przed skokiem obie straty — ogólna i KO — dotyczą tego samego etapu. Wyniki poprzednich serii przeliczane są wagami bieżącej, a jej skoki są pomijane u wszystkich. Usuwa to zawyżenia typu180pkt zamiast10pkt. Zachowano minus przy przewadze i ukrywanie po całkowitym resecie.
- Regresje obejmują kolejne skoki lidera, trzecią serię z niejednakowymi wagami i liczbę skoków przypadającą na serię drużynową.

## Załącznik A. Dokładny zakres Git

- Baza: **05f2e48d8e92c7b9b180f18abe250b3c7e61d24c**.
- Snapshot kodu i dokumentacji: **5af8f1b80c93d27c90a82c6a3fd2fe97851a30ac**.
- Commitów od bazy: **148**.
- Plików zmienionych w tym zakresie: **467**.
- Statystyki obejmują także fonty, skills i dokumentację; nie oznaczają rozmiaru samej logiki gry.
- Dalsza aktualizacja tego zestawienia jest dodatkiem dokumentacyjnym do wskazanego snapshotu.

| Obszar | Plików |
| --- | ---: |
| Dokumentacja etapów i migracji | 32 |
| Domyślny interfejs Qt Quick i jego zasoby | 100 |
| Instrukcje i zasoby narzędzi deweloperskich | 167 |
| Konfiguracja projektu i pozostałe pliki | 6 |
| Kontrolery, modele i logika C++ | 123 |
| Skrypty budowania, uruchamiania i testowania | 6 |
| Testy | 15 |
| Tłumaczenia | 1 |
| Wcześniejszy opcjonalny eksperyment konkursowy | 17 |

Porównanie można odtworzyć poleceniami:

~~~sh
git diff --stat 05f2e48d8e92c7b9b180f18abe250b3c7e61d24c 5af8f1b80c93d27c90a82c6a3fd2fe97851a30ac
git diff --name-status 05f2e48d8e92c7b9b180f18abe250b3c7e61d24c 5af8f1b80c93d27c90a82c6a3fd2fe97851a30ac
git log --reverse --oneline 05f2e48d8e92c7b9b180f18abe250b3c7e61d24c..5af8f1b80c93d27c90a82c6a3fd2fe97851a30ac
~~~

Zestawienie można odświeżyć przez `python3 scripts/update-release-inventory.py --base main --snapshot HEAD`. Opisy zachowania i wyniki testów należy uzupełniać na podstawie faktycznych zmian oraz weryfikacji.

## Załącznik B. Wszystkie commity

Historyczne opisy należy czytać z uwzględnieniem opisanych powyżej rozwiązań zastąpionych podczas prac.

| Commit | Zmiana i zapisane uzasadnienie/weryfikacja |
| --- | --- |
| `648fb2a` | Add optional responsive Qt Quick competition configuration preview |
| `7041c12` | Fix invisible Qt Quick configuration dropdowns |
| `8ed248b` | Rebuild Sj.Sim desktop UI in Qt Quick for 1.3.0 beta-3 — Make the fresh alpine Qt Quick interface the default while retaining the Widgets alternative and historical previews. Add shared navigation, appearance, accessible responsive controls, bundled typography and the release badge. Rework calendars, athlete rosters, season preparation, form generation, presets and live competition presentation. Fix start-list model notifications, target lines, scoring, wind generation, live staging and related controller defects. Include Qt skills, build/run scripts, Polish and English strings, migration documentation and integration coverage. Validation: 292 responsive renders; focused full individual, KO and team competitions with start-list model contract checks; Widgets-only build. |
| `6233bdb` | Fix 01: preserve skipped places after archive classification ties — Archive standings returned 1,1,2 after a shared lead. Increment the tied block in both athlete and team ranking helpers, producing 1,1,3. Regression verified against the previously failing tie fixture. |
| `6f44d3b` | Fix 02: map statistics range choices to the displayed competition — Numeric reconstruction counted unplayed events and could choose another calendar's event. Store direct identities for displayed played events and disable season headers. Test partially played merged and filtered calendars. |
| `91a1e6e` | Fix 03: rebuild classification filters without invalidating iteration — Removing checkbox pointers from their vector during range iteration skipped widgets and risked use-after-free. Delete the old set before clearing it, guard missing sources and bound selection lookup. Test five consecutive rebuilds and all selections. |
| `55e6d16` | Fix 04: reject invalid form input and contradictory generation limits — Reject nonnumeric and non-finite table edits and min/max inversions. Keep bulk minimum and maximum consistent when either bound changes. Regression checks cover invalid text, NaN, reversed table bounds and bulk updates. |
| `07d6e30` | Fix 05: honor zero disqualification override in jump manipulation — An exact DSQ value of zero was converted to denominator one, guaranteeing disqualification. Treat zero as disabled, matching the base DSQ setting; retain exact one and inherited settings. Regression runs real jump simulations for all three cases. |
| `2993270` | Fix 06: keep all selected athletes in the form generator — The keep-selected action removed every row when all athletes were selected. Apply the selection/complement consistently, preserve empty-selection drafts and notify model resets. Test all, one-cell and empty selections. |
| `698feb4` | Fix 08: avoid statistics crashes for small fields and tied winners — Winning-margin event pointers stayed null for fewer than ten entrants or a zero-point winning gap. Show unavailable margins instead of dereferencing null; retain real tied results and identify the owning season/calendar. Test a two-athlete tie. |
| `2e4f5cc` | Fix 07: omit unavailable averages instead of producing NaN statistics — No eligible judges, wind scoring or non-DSQ jumps caused division by zero and invalid rankings. Skip undefined athlete averages and display unavailable aggregate values/percentages. Test disabled scoring components and an entirely disqualified field. |
| `38e8e31` | Fix 09: keep statistics usable without point-award presets — An empty preset database or index -1 was indexed unconditionally and crashed the ratings screen. Clear unsupported places classifications while preserving averages and competition-point classifications. Test removing all presets and both scoring modes. |
| `642a618` | Fix 10: clear obsolete statistics when filters return no results — Empty filters left previous rankings, records, facts and exports in place. Clear every result model and factual value without a blocking information dialog. Bind average tables to their own models at construction. Test a populated-to-empty filter transition. |
| `8511b0f` | Fix 11: handle empty calendars and missing statistics range endpoints — Range slicing dereferenced null endpoints and calendar switching read first() on empty calendars. Return empty slices for missing/reversed endpoints, guard selection indices and let range widgets represent empty calendars. Test switching from a populated calendar to an empty one. |
| `37d1b8f` | Fix 12: handle incomplete or detached jump details without crashing — Missing athlete/hill links and a competition without the jump's result caused null dereferences or negative round indexing. Show unavailable details, disable sharing and return an unknown rank for detached jumps. Test missing links, recovery and an orphaned result. |
| `3872878` | Fix 13: keep athlete charts safe for DNS and empty selections — Skip results without jumps, guard missing chart axes and invalid hover indices, and calculate positions only from displayed results. Empty charts show unavailable statistics instead of invalid averages. Test DNS-only data and switching to an empty calendar; include all 13 audit regressions in the default suite. |
| `56dd0ea` | Document all 13 UX and gameplay fixes with regression evidence — Preserve symptoms, reproduction scenarios, causes, changes and the individual fix commit IDs outside the chat. Record passing audit/integration checks, 292 responsive renders, Widgets compatibility and the unchanged main branch. |
| `e577e73` | UX 1: make form parameter changes explicit and scoped — Stage global parameters and presets instead of silently overwriting every athlete. Offer an explicit All/Selected scope and Apply action, preserve identities and selection, and require applying the draft before generation. Keep the Widgets workflow available. Test a real selected-only Apply click and staged presets. |
| `1611e61` | UX 2: expose table search, selection counts and empty-state recovery — Keep search visible in roomy desktop tables, support Find, show selected row counts with a clear action and offer filter reset directly in empty results. Clear selection when changing filters to avoid hidden bulk targets; preserve compact layouts and source data. Test real keyboard filtering and reset clicks. |
| `0679252` | UX 3: make saved athlete lists directly actionable — Expose list choice and separate Add, Remove and Order actions on desktop, with one accessible popover on small windows. Keep undo, eligibility and member identity; refresh saved-list names when returning from editing. Test desktop and narrow-screen membership/order actions through real clicks. |
| `191138e` | UX 4: group competition tools and expose live/weather controls — Replace the single long options stack with Progress, Conditions and Jump sections. Put live mode in the main toolbar and make wind/snow indicators open Conditions directly. Keep last jump, results and start list together. Test actual live-toggle and wind action clicks plus full competition playback. |
| `4e1c614` | UX 5: separate calendar selection from opening the editor — A click now selects without leaving the event list; use Edit, the row arrow, Enter or double-click to open details. Keep Ctrl/Shift selection in place, show selection count and provide a clear action. Test narrow-screen multi-selection and opening one batch inspector. |
| `f9a0b90` | UX 1: pin form apply controls and preserve explicit scope |
| `8708819` | UX 2: distinguish selection from cursor and expose laptop search |
| `597e1dd` | UX 3: preserve usable athlete rows in the smallest windows |
| `77811ae` | UX 5: retain keyboard focus across calendar selection refreshes |
| `d865528` | Save five-workflow UX regressions, translations and resume checkpoint |
| `5398f72` | UX 1: read complete form roster through the existing item model |
| `4a49261` | UX 4: keep KO tables inside the laptop competition viewport |
| `f1c3d87` | Document five completed UX workflows and final validation results |
| `663bd58` | Expand catalog workspaces and expose complete last-jump information — Use full-height database, season and save lists with independently scrolling editors and compact actions. Show judges, gate, landing, compensation components, totals and team positions directly; preserve live reveal stages and distinguish signed values. Cover catalog growth, small-window summaries, judge exclusions and independent sign colours in integration tests. |
| `870c929` | Fix calendar qualification sources and ordering constraints — Offer only earlier competitions of the same type; keep shared choices valid for multi-selection. Block moving a qualification source behind a dependent competition. Repair future/self links when loading calendars and clarify the field captions. Add regressions for source choices, invalid indices, ordering, undo and link repair. |
| `ac0cd90` | Keep modal actions visible and make numeric stepping immediate — Scroll long dialog titles and bodies while pinning responsive action rows. Refit transient windows after native layout/size changes and application resizing. Count each numeric button press locally and enable 250 ms hold delay / 40 ms repeat. Regressions cover rapid integer/decimal steps, bounds, hold repeat and long record notifications after forced native resizing on small screens. |
| `53fdc7f` | Compact competition cards and clarify jump ranking across rounds — Cap card widths, size them to contents, emphasize rank, add the athlete flag and leader gap, and open competition categories directly from the existing toolbar. Show previous-round positions on the gate and during live presentation until the new result is revealed, including team prefixes and ties. Snapshot rank data while native result references are valid. Validate 2K/small-screen geometry, category actions, leader gaps and live rank transitions. Document all refinements and the passing 343-render suite, focused follow-ups and Widgets build. |
| `284a89e` | Localize standard dialog actions and fix message text sizing — Project standard Yes/No, Cancel, Save and other actions through application translations while preserving native roles and IDs. Give message text the full dialog width and explicit content height below the title, keeping checkboxes and pinned actions. Add a short-message geometry and Polish-caption regression; update translations for the redesigned workflows. |
| `e021c7b` | Rebuild season calendar editing with reversible synchronized operations — Separate edited and active calendars, expose explicit activation and creation, and integrate classifications into one workspace. Add bulk competition creation, multi-selection drag/drop of complete event blocks, and shared undo/redo including catalog changes and preset replacement. Preserve displaced qualification sources through ordering and serialization, mark both ends, restore valid links automatically and block dependent launches. Protect referenced sources and played history. Synchronize overview standings and native fields immediately; retain objects safely across history and editor shutdown. Repair points-table model notifications, fractional/locale-aware editing and classification cleanup. Used standings allow renaming and copying while locking scoring. Add pointer-driven drag, serialization, playback separation, bulk edits, undo/redo, preset and model-contract regressions; enrich the next-event footer. |
| `33ff226` | Default competition gates to Auto and refresh wind recommendations — Replace the unbounded random trial loop with a deterministic bounded distance preview. Default preparation to Auto, retain manual gate selection, show the recommended gate and signed difference in the wind workspace, and apply it directly. Handle special numeric captions in Quick. Regressions cover the initial mode, repeatability, headwind/tailwind changes and the apply action. |
| `da0924b` | Clarify live competition status without revealing jump ranks early — Add the hill flag and signed wind colors to the top bar. Replace the start counter with the leader and points, compacting the smallest layout and preserving the previous leader until the live result is revealed. Keep the last-jump rank blank during flight while retaining previous-round position on the gate card. Remove the unused previous-rank snapshot and update the live regression. |
| `50e991d` | Document season workflow changes and the 350-render validation |
| `e9ce0a4` | Show the calculated starting gate as a number instead of Auto |
| `9a6641c` | Remove redundant jump card heading and live stage captions |
| `4697730` | Scroll competition options only when their content overflows — Use an explicit vertical Flickable with bounds clamped to its content. Disable interaction when every option fits; retain scrolling for smaller panels and longer categories. Validation: scripts/test-quick-ui.sh --ux-only passes, including fitted and overflowing options, direct competition actions and live presentation. The preceding label removal also passed --competitions-only. |
| `145738d` | Destroy finished competition views before releasing simulation data — Reproduced SIGSEGV in QuickBridge::refresh after closing a single competition and refreshing the surviving adapter. Scope competition and qualification windows before manager cleanup, own managers with RAII and keep hill copies scoped. Guard missing global athletes during record updates. Replace the test's direct accept() with the visible finish action and assert the competition window is destroyed on return. scripts/test-quick-ui.sh --ux-only passes. |
| `da96752` | Show the biggest gain since the previous round in the competition header — Replace duplicated leader data with the largest positive position change among current-round jumpers. Handle previous-round ties, shared gains, team totals, narrow layouts and English translations; show all tied athletes and before/after places in the tooltip. Freeze the statistic until live results are revealed. Validate empty and incomplete rounds, tied places, equal gains, third rounds and team baselines. Full individual, KO and team competition tests pass, including pointer-driven finish and view destruction; Widgets-only builds successfully. Document both the statistic and the finish crash fix. |
| `d78e4c7` | Preserve live hill assignments when serializing calendar presets — The preset's shallow calendar copy shared event pointers, so clearing hills for JSON also cleared its live data. Remove hill IDs from the serialized JSON instead. The regression fails before the fix and passes with --backend-only. |
| `291ad73` | Preserve earlier competition CSV exports when file names collide — Automatically exported competitions can share a name within the same minute. Create CSV files with NewOnly and numbered collision suffixes instead of truncating prior results. Build paths with QDir and report actual write-open errors. Regression proves the old code overwrites existing results; the fixed --backend-only run retains the original and creates distinct nonempty exports, including a directory without a trailing slash. |
| `dbacc92` | Keep random samples inside singleton, narrow and extreme bounds — Return the only valid value for singleton ranges, avoid integer overflow at INT_MAX and stop collapsing small decimal ranges through int conversion. Preserve decimal precision where possible and use a bounded fractional fallback where no precision grid point exists. The new regression fails on the old singleton behavior and passes with narrow, negative, full integer and extreme finite floating-point bounds. Backend, complete individual/KO/team competitions, season workflows and Widgets-only builds pass; document all three small but consequential fixes. |
| `44b984f` | Bind cloned preset result containers to their new calendar events — CompetitionInfo copies retain the raw CompetitionResults::competition pointer. Rebind it when building and importing a preset so new jumps do not belong to the original event or template. Regression fails on the old preset constructor and passes with --workflow-only, including the imported event and newly added result/jump ownership. |
| `df1a783` | Preserve auxiliary event settings when duplicating calendar blocks — Use the same configuration copy for main events, trainings and trial rounds. Preserve their importance, exceptional rounds, own hill and classification/qualification links while keeping fresh event identities and empty history. The regression fails before the fix on training/trial importance and passes with --workflow-only, including independent edits and undo. |
| `d157c29` | Keep duplicated qualification links within the copied calendar block — Build an original-to-copy map for selected competitions and their attached events, then remap both active and pending qualification sources. Preserve external sources and original dependencies. The regression fails before the fix and passes with --workflow-only, including reverse links, pending-source restoration, external sources and undo. |
| `4ddc8c2` | Document the complete release change inventory against main beta-2 — Consolidate the final Qt Quick UI, workflows, season and calendar changes, scoring and live repairs, crashes, persistence, presets and copying fixes. Record superseded designs, compatibility notes, validation limits and a manual release checklist. Include every one of the 50 application-history commits and all 344 changed files through d157c29, with exact baseline/runtime hashes and local links. Final full integration run passes with 350 renders; Widgets-only builds successfully. Main remains 05f2e48. |
| `ccf8b0d` | fix: preserve fractional points when reloading presets |
| `a894d4b` | fix: reset competition options scrolling when content fits |
| `d89b6f4` | wip: save hill search implementation and resume checklist |
| `54fa3f8` | docs: record prior-round gap and jump round placing requirements |
| `641c6aa` | docs: add reminder about author page |
| `da2fcd0` | feat: validate scoped hill search and handle empty matches |
| `284360f` | fix: preserve names and column boundaries in CSV results |
| `1171608` | fix: derive next competition from calendar state on load |
| `5b9f147` | fix: retain historical team points after nationality changes |
| `84e1729` | fix: stage next-season edits and apply accepted additions atomically |
| `eef3818` | fix: account for remaining starters in biggest advancement |
| `f95bcfa` | feat: show previous-round gaps and each jump round placing |
| `100f09a` | docs: track bulk and engine simulation performance work |
| `cd3cdba` | fix: clear rules editor before replacing next-season rules |
| `8090805` | perf: batch athlete and start-list presentation in automatic simulation |
| `4b8322a` | i18n: translate hill search and updated competition standings |
| `af8f47c` | perf: read simulation characteristics without copying their set |
| `380b671` | perf: compile out debug logging in release builds |
| `8d873ea` | test: cover hill search within a small laptop viewport |
| `18b681d` | docs: record completed fixes, performance measurements and validation |
| `c6c9782` | docs: preserve the release inventory generator for future updates |
| `b65723e` | docs: refresh complete release inventory after fixes and optimizations |
| `6910a70` | feat: add author page with portrait, featured Predazzo project and social links |
| `bf523d6` | fix: unload inactive race options to remove stale scrolling |
| `208fce0` | ui: remove unexplained mode range from the home screen |
| `4490b95` | fix: enable start-list sorting by default for newly added rounds |
| `04f78b6` | fix: connect author navigation explicitly without auto-slot warnings |
| `131f87e` | feat: add persistent compact interface and smaller text options |
| `a52c96f` | perf: batch single-jump progress updates and event processing |
| `4c4ba9c` | fix: show full compact author portrait on small screens — Keep the desktop portrait layout, but fit the complete photograph inside a 150 by 200 frame on narrow screens. Verify portrait bounds and responsive placement in both themes. |
| `c4b9212` | fix: initialize standalone jump importance — Use the same default importance of 5 as the configuration screen instead of reading an uninitialized floating-point value when the manager is used without explicit configuration. Add a constructor regression. |
| `c9b2682` | perf: reuse normal samples and reflect centered half-normal draws — Retain the cached second sample from the standard normal distribution. Reflect zero-centered draws for half-normal sampling instead of rejecting roughly half of them; keep truncation for shifted normals. Check signs, mean, variance and zero deviation without changing simulation coefficients. |
| `bb4f7a6` | perf: prepare single-jump wind settings and result buffers once — Reserve result and wind buffers, assign generation settings outside the batch loop, avoid detaching unchanged wind plans, and reuse fixed winds. Use a stack manipulator and initialize the competition gate. Verify cancellation with partial results, per-jump variable winds, finite scores, append semantics and immutable history. |
| `4cae942` | feat: calculate a starting gate for standalone jumps — Expose Calculate gate beside the numeric starting gate in Widgets and Quick. Use the current athlete, hill and configured base wind with deterministic previews targeting 97 percent HS. Keep manual values until requested and preserve them for incomplete inputs. Test bounded repeatable estimates, wind direction and actual Quick activation. |
| `1ee819b` | fix: let standalone jump results fill the available workspace — Replace the generic capped table with a full-height responsive results screen. Keep statistics beside results on desktop and in a separate section on narrow windows. Flatten jump details, expose all six statistics choices and retain native selection and sorting. Balance numeric column widths. Test both themes down to 360x480, larger text, and vertical expansion up to 1400px. Add a repeatable standalone simulation benchmark. |
| `f945bd2` | feat: expose three direct competition simulation actions — Replace the parent auto-simulation menu with N jumps, Round and Competition buttons. Preserve Group for team events. Close the options drawer when an operation is chosen. Verify each control opens its intended native action and cancellation preserves the competition, followed by completion of a real competition. |
| `0b81ec2` | i18n: translate compact appearance and standalone jump controls |
| `7eaf15d` | fix: place the compact author portrait beside the heading — Use a shared header row on small screens: full 84x112 or 120x160 portrait next to the greeting, followed by full-width biography and projects. Keep the desktop photo beside the entire biography. Verify portrait/header alignment, full framing, both themes and larger text. |
| `e3c62cb` | docs: record author, compact UI and standalone jump changes — Document every completed follow-up, final small-screen portrait layout, full-height results, direct simulation controls, default sorting and single-jump gate calculation. Record three optimizations with phase timings and limitations, 367-render integration validation, final author regression and Widgets build. Update the continuation checkpoint and manual release checklist. |
| `d95db6a` | docs: refresh complete release inventory through e3c62cb — Regenerate all 90 commits and 377 changed files against untouched main beta-2, including the author and standalone-jump follow-ups. This manifest-only commit is outside its explicitly recorded snapshot. |
| `0eeef38` | docs: add complete Windows development setup for contributors and Codex — Document the CI-aligned Qt 6.8.3/MSVC 2022 x64 toolchain, all required Qt modules, optional aqt installation, developer PowerShell, qmake builds, isolated runtime data, smoke tests, Widgets fallback, troubleshooting and QML-aware deployment. Verify links, build targets and tool syntax against repository configuration and primary documentation; explicitly state Windows execution was not performed here. |
| `d844f0c` | docs: include Windows setup in the release change inventory |
| `d55ed9d` | Fix Qt 6 Windows build and Quick UI shutdown |
| `db2b4ee` | Fix native Windows Quick page initialization and control style |
| `5af7916` | Record Windows UI issues for the next session |
| `028cdf2` | Fix Windows page presentation and adopt FIS country codes |
| `2e254a4` | Add per-round score resets and advanced carryover weights |
| `0dec098` | Add named data packs and jumper birth information |
| `74ea6b4` | Add per-event classification scoring, age and country filters |
| `aa0ce7d` | Add optional season dates, scheduling tools and calendar overview |
| `ece9852` | Fix stale validation after restoring a valid calendar date |
| `df6e5d1` | Keep date picker days accessible in short windows |
| `2101965` | Exclude hidden classification rows from qualification |
| `58f2517` | Preserve pack catalogs when legacy database files are missing |
| `5c99454` | Cache visible classification rows per table refresh |
| `b3db958` | Reuse QML engine caches to speed up screen navigation |
| `7efe6f0` | Make compact interface substantially denser — Reduce compact control heights, panel padding, gaps, table rows and application chrome. Keep body text at the user's selected size, adapt rows to font metrics and use smaller headings and home cards. Verify density reduction and capture desktop and narrow layouts with enlarged text. |
| `123c67c` | Avoid duplicate controls when projecting form layouts — Handle full-width form rows once instead of reading the same item through both FieldRole and SpanningRole. Hide the redundant shortcut button row in classification editors while retaining keyboard actions and explicit controls for both points tables. Add a regression reproducer and scan classification filters, rules, database catalogs and competition pages. |
| `33605a6` | Rebind jump previews after automatic simulation and round changes — Prevent Super Team previews from reading jump buffers destroyed by team member result rebuilding. Hide the preview during mutations and resolve the displayed athlete from current results afterwards, including round transitions and cancellation. Reproduce manual jumps followed by auto-round simulation and cover all automatic modes across the three-round Super Team preset. |
| `257a2dd` | ui: remove search from live start lists and KO group results — Allow table consumers to disable both search controls and the Find shortcut, and clear stale filters when search is disabled. Keep search available in regular result and editor tables. |
| `e580393` | fix: show button hints only on intentional hover or help — Stop opening tooltips merely because a button retains keyboard focus. Suppress hints throughout repeated presses until the pointer leaves, and expose explicit F1 help plus accessible descriptions. |
| `d9bab6f` | fix: use consistent tenths-based ranking for competition ties — Centralize competition score equality at the displayed tenth of a point, preserve scoring-round tiers, and use the same rule in sorting, global ranks, KO subgroup ranks and historical snapshots. Replace duplicate inconsistent comparison/ranking paths so cumulative binary floating-point noise cannot split ex aequo. |
| `c68f92b` | fix: isolate coach gate changes from jury and round baselines — Block native signals when displaying a coach-only gate, preserve the jury reference across repeated adjustments and cancellation, and clear the override on manual jury changes. Update the round baseline only before any athlete has attempted a jump. Add regressions exercising the actual coach dialog, limits, cancellation, repeated changes, and first/last starters. |
| `19d5a97` | fix: preserve numeric drafts across competition dialog refreshes — Keep an explicit local numeric draft so periodic backend metadata updates cannot overwrite typing. Commit a dirty draft before +/- steps. Add a regression typing slowly across refreshes and checking both step buttons, plus intentional tooltip help behavior. |
| `8023b76` | fix: calculate guaranteed advancement and independent contest qualification — Share rank bounds across individual, team and KO competitions; preserve boundary ties, skip ineligible qualifiers and keep KO quotas exact. Expose neutral qualification Q, muted elimination and softer next-round markers without sorting during model reads. Add exhaustive future-outcome, KO and team regressions. |
| `fa6ea6e` | fix: keep roster dragging active while scrolling long start lists — Own the pointer gesture outside recycled delegates, accelerate edge scrolling and support the wheel without losing the athlete preview. Keep selection counts and country flags visible; cover long drags, virtualization and undo. |
| `5285fd8` | fix: preserve calendar scroll and compact event rows with weekday context — Synchronize a stable item model through incremental inserts, moves and updates instead of resetting delegates. Refresh selected-event rules before notifications, retain undo behavior and add model-contract, hill/rules switching and dated qualification tests. |
| `cce62ba` | fix: rebind competition selectors and show hill flags throughout calendar tools — Keep searchable choice selection bound to its backend after activation, stabilize calendar field delegates and refresh rule projections when selection changes. Carry country icons through native choices and date views; derive the race header from effective rounds so one-round qualifications read correctly. |
| `2d2cb43` | i18n: translate qualification markers and advancement explanations |
| `8359eba` | fix: fill available calendar inspector height after event and tab changes — Anchor the field viewport below a separately sized header to prevent Qt layout negotiation from collapsing it to zero height. Verify five window sizes with and without dates, switching events and tabs; integrate competition, roster, numeric-input and calendar regressions into the smoke suite. |
| `2df4f37` | docs: record competition advancement and calendar UX fixes with validation |
| `c93f84c` | docs: refresh complete release inventory against main |
| `3295a84` | fix: show final tied podium and weighted leader gaps with KO jump previews — Suppress provisional medals and handle series cancellation as competition completion. Use rank positions for shared podium places, recompute inherited points with current weights, suppress reset gaps and resolve KO preview rows to their athletes. Remove advancement check marks and move calendar Q next to the event identity. |
| `63c7662` | feat: create simulations from three data packs in one focused setup page — Replace the initial editable-season wizard with pack choices, counts, optional dates and remembered accepted selections. Copy selected data into the save without changing active global packs; keep creation available outside scrolling content and reject empty data packs. Preserve the next-season configurator and Widgets components. Add the shared empty-state component. |
| `faeba46` | fix: make empty season workflows actionable and synchronize calendar navigation — Create and select the first calendar directly from overview even when the native tab index is unchanged. Expose rename state, simplify controls for empty calendars and explain missing calendars, classifications, results, rosters and filtered data with viewport-bound empty states. Refresh current weighted leader and KO gaps while freezing them during live flight. |
| `17bd91b` | test: cover simulation onboarding and weighted competition presentation — Verify pack isolation, remembered choices, date selection, direct calendar creation, visible empty-state actions, weighted live gaps, reset suppression, final-only tied medals and KO row preview mapping. Add an actual multi-round KO integration route. |
| `f9fef6c` | docs: record simplified season setup, empty states and final result behavior — Document user decisions, completed workflows, exact regression routes and local platform limits; add English translations for new setup and empty-state guidance. |
| `29ad6ac` | docs: refresh release manifest after simulation onboarding improvements |
| `b65a7be` | ui: align calendar qualification Q right with comfortable padding |
| `6c7ebdc` | docs: refresh release inventory after calendar badge spacing correction |
| `f1b9c33` | test: reproduce ten simulation engine defects independently — Add selectable non-UI regressions for jump state, missing context, DSQ arithmetic, flight height, judge overrides, coach compensation, hill persistence, fractional K points, wrapped wind angles and degenerate half-normal sampling. |
| `7cfef95` | fix: initialize jump snapshots and clear stale DNS and coach state — Fix engine audit case 1. The independent regression fails on the previous implementation and passes with this change. |
| `87b5792` | fix: validate simulation context and clear detached athlete skills — Fix engine audit case 2. The independent regression fails on the previous implementation and passes with this change. |
| `9d6c768` | fix: guard zero and invalid disqualification probability factors — Fix engine audit case 3. The independent regression fails on the previous implementation and passes with this change. |
| `6e50e1c` | fix: apply flight height penalty to the difference from the ideal — Fix engine audit case 4. The independent regression fails on the previous implementation and passes with this change. |
| `9b8644b` | fix: normalize partial and out-of-range exact judge overrides — Fix engine audit case 5. The independent regression fails on the previous implementation and passes with this change. |
| `6266cb8` | fix: deduct compensation for a higher coach gate below 95 percent HS — Fix engine audit case 6. The independent regression fails on the previous implementation and passes with this change. |
| `111d6d0` | fix: preserve independent automatic metre scoring in hill saves — Fix engine audit case 7. The independent regression fails on the previous implementation and passes with this change. |
| `b48fa0d` | fix: score fractional K points between 169 and 170 correctly — Fix engine audit case 8. The independent regression fails on the previous implementation and passes with this change. |
| `090bd61` | fix: normalize multi-turn wind directions before simulation — Fix engine audit case 9. The independent regression fails on the previous implementation and passes with this change. |
| `53a9960` | fix: terminate degenerate half-normal sampling and sample rare tails efficiently — Fix engine audit case 10. The independent regression fails on the previous implementation and passes with this change. |
| `a99df69` | perf: simulate large independent jump batches on isolated worker threads — Run batches of at least 4096 jumps asynchronously with at most four workers. Snapshot athlete, hill, rules and settings; use thread-local random generators, preserve output order and cancellation, and rebind detached results only on the owner thread. Cover owner destruction, settings isolation, serialization, unique IDs and serial/parallel throughput. |
| `9174fb8` | docs: record ten engine fixes and measured asynchronous batch performance |
| `b04f612` | release: set shared application version to 2.0.0-beta — Use the requested exact version string throughout the application and preserve the separate beta badge in Qt Quick. Verify the shared version assertion and engine audit regressions. |
| `0fc093d` | fix: add the first calendar competition directly from season overview — Make the overview action insert and select an event through the existing undoable calendar backend, then request the editor in Qt Quick. Verify the actual creation and editor state rather than only a tab change. |
| `a72a6b5` | fix: retranslate roster labels without reading retired athlete storage — AddressSanitizer found a heap-buffer-overflow when language changes rebuilt hidden roster rows after their source database had been replaced. Translation invalidation now notifies label bindings using cached value rows. Add a regression that retires athlete storage before invalidation and then verifies explicit data synchronization. |
| `5af8f1b` | fix: compare pre-jump gaps at the same completed-round boundary — Apply current-round weights to completed rounds only, excluding rivals' additional jumps in the current round. Keep overall and KO gaps consistent, handle team group counts and reset suppression, and cover third-round unequal weights. Save the engine audit and memory-diagnostic checkpoint for continued work. |

## Załącznik C. Pełny manifest zmienionych plików

A: dodany, M: zmieniony, D: usunięty. +/− to liczby linii tekstu; kreska oznacza zasób binarny.

### Dokumentacja etapów i migracji

| Status | Plik | + | − |
| --- | --- | ---: | ---: |
| A | [QML_COMPETITION_LIVE.md](../QML_COMPETITION_LIVE.md) | 42 | 0 |
| A | [QML_COMPETITION_PREVIEW.md](../QML_COMPETITION_PREVIEW.md) | 60 | 0 |
| A | [QUICK_UI_MIGRATION.md](../QUICK_UI_MIGRATION.md) | 314 | 0 |
| A | [docs/AUTHOR_AND_SINGLE_JUMPS_2026-10-22.md](../docs/AUTHOR_AND_SINGLE_JUMPS_2026-10-22.md) | 66 | 0 |
| A | [docs/BUGFIX_AUDIT_2026-10-21.md](../docs/BUGFIX_AUDIT_2026-10-21.md) | 140 | 0 |
| A | [docs/CALENDAR_COPY_FIXES_2026-10-22.md](../docs/CALENDAR_COPY_FIXES_2026-10-22.md) | 27 | 0 |
| A | [docs/CALENDAR_DATES.md](../docs/CALENDAR_DATES.md) | 40 | 0 |
| A | [docs/CLASSIFICATION_SCORING.md](../docs/CLASSIFICATION_SCORING.md) | 50 | 0 |
| A | [docs/COMPETITION_ADVANCEMENT_2026-10-22.md](../docs/COMPETITION_ADVANCEMENT_2026-10-22.md) | 30 | 0 |
| A | [docs/COMPETITION_AND_HILL_SEARCH_2026-10-22.md](../docs/COMPETITION_AND_HILL_SEARCH_2026-10-22.md) | 12 | 0 |
| A | [docs/COMPETITION_FINISH_CRASH_2026-10-22.md](../docs/COMPETITION_FINISH_CRASH_2026-10-22.md) | 20 | 0 |
| A | [docs/COMPETITION_REFINEMENTS_2026-10-21.md](../docs/COMPETITION_REFINEMENTS_2026-10-21.md) | 34 | 0 |
| A | [docs/COMPETITION_UX_2026-10-24.md](../docs/COMPETITION_UX_2026-10-24.md) | 62 | 0 |
| A | [docs/DATA_PACKS.md](../docs/DATA_PACKS.md) | 30 | 0 |
| A | [docs/ENGINE_AUDIT_2026-10-24.md](../docs/ENGINE_AUDIT_2026-10-24.md) | 76 | 0 |
| A | [docs/FIVE_DATA_FIXES_2026-10-22.md](../docs/FIVE_DATA_FIXES_2026-10-22.md) | 57 | 0 |
| A | [docs/NAVIGATION_PERFORMANCE.md](../docs/NAVIGATION_PERFORMANCE.md) | 30 | 0 |
| A | [docs/RELEASE_NOTES_VS_MAIN.md](../docs/RELEASE_NOTES_VS_MAIN.md) | 1343 | 0 |
| A | [docs/RESUME_2026-10-22.md](../docs/RESUME_2026-10-22.md) | 44 | 0 |
| A | [docs/RESUME_2026-10-24_COMPETITION_UX.md](../docs/RESUME_2026-10-24_COMPETITION_UX.md) | 52 | 0 |
| A | [docs/RESUME_ENGINE_AUDIT_2026-10-24.md](../docs/RESUME_ENGINE_AUDIT_2026-10-24.md) | 38 | 0 |
| A | [docs/RESUME_WINDOWS_UI_2026-10-23.md](../docs/RESUME_WINDOWS_UI_2026-10-23.md) | 44 | 0 |
| A | [docs/ROUND_POINT_WEIGHTS.md](../docs/ROUND_POINT_WEIGHTS.md) | 23 | 0 |
| A | [docs/SEASON_WORKSPACE_2026-10-22.md](../docs/SEASON_WORKSPACE_2026-10-22.md) | 60 | 0 |
| A | [docs/SIMULATION_ONBOARDING_2026-10-24.md](../docs/SIMULATION_ONBOARDING_2026-10-24.md) | 74 | 0 |
| A | [docs/SIMULATION_PERFORMANCE_2026-10-22.md](../docs/SIMULATION_PERFORMANCE_2026-10-22.md) | 57 | 0 |
| A | [docs/SPACE_AND_LAST_JUMP_2026-10-21.md](../docs/SPACE_AND_LAST_JUMP_2026-10-21.md) | 43 | 0 |
| A | [docs/SUPER_TEAM_AUTO_SIMULATION_CRASH.md](../docs/SUPER_TEAM_AUTO_SIMULATION_CRASH.md) | 17 | 0 |
| A | [docs/THREE_SMALL_FIXES_2026-10-22.md](../docs/THREE_SMALL_FIXES_2026-10-22.md) | 37 | 0 |
| A | [docs/UX_IMPROVEMENTS_2026-10-21.md](../docs/UX_IMPROVEMENTS_2026-10-21.md) | 77 | 0 |
| A | [docs/UX_REWORK_CHECKPOINT_2026-10-21.md](../docs/UX_REWORK_CHECKPOINT_2026-10-21.md) | 28 | 0 |
| A | [docs/WINDOWS_UI_AND_FIS_2026-10-23.md](../docs/WINDOWS_UI_AND_FIS_2026-10-23.md) | 33 | 0 |

### Domyślny interfejs Qt Quick i jego zasoby

| Status | Plik | + | − |
| --- | --- | ---: | ---: |
| A | [source/UI/Quick/ActionBar.qml](../source/UI/Quick/ActionBar.qml) | 29 | 0 |
| A | [source/UI/Quick/ActionMenu.qml](../source/UI/Quick/ActionMenu.qml) | 31 | 0 |
| A | [source/UI/Quick/AppFooter.qml](../source/UI/Quick/AppFooter.qml) | 15 | 0 |
| A | [source/UI/Quick/AppHeader.qml](../source/UI/Quick/AppHeader.qml) | 26 | 0 |
| A | [source/UI/Quick/AppearanceMenu.qml](../source/UI/Quick/AppearanceMenu.qml) | 32 | 0 |
| A | [source/UI/Quick/AppearancePanel.qml](../source/UI/Quick/AppearancePanel.qml) | 12 | 0 |
| A | [source/UI/Quick/AuthorPage.qml](../source/UI/Quick/AuthorPage.qml) | 96 | 0 |
| A | [source/UI/Quick/AuthorProject.qml](../source/UI/Quick/AuthorProject.qml) | 36 | 0 |
| A | [source/UI/Quick/AuthorSocial.qml](../source/UI/Quick/AuthorSocial.qml) | 25 | 0 |
| A | [source/UI/Quick/CalendarClassificationScoring.qml](../source/UI/Quick/CalendarClassificationScoring.qml) | 86 | 0 |
| A | [source/UI/Quick/CalendarClassifications.qml](../source/UI/Quick/CalendarClassifications.qml) | 61 | 0 |
| A | [source/UI/Quick/CalendarDateField.qml](../source/UI/Quick/CalendarDateField.qml) | 93 | 0 |
| A | [source/UI/Quick/CalendarDateOverview.qml](../source/UI/Quick/CalendarDateOverview.qml) | 105 | 0 |
| A | [source/UI/Quick/CalendarDatePlanner.qml](../source/UI/Quick/CalendarDatePlanner.qml) | 72 | 0 |
| A | [source/UI/Quick/CalendarEventRow.qml](../source/UI/Quick/CalendarEventRow.qml) | 116 | 0 |
| A | [source/UI/Quick/CalendarFields.qml](../source/UI/Quick/CalendarFields.qml) | 63 | 0 |
| A | [source/UI/Quick/CalendarInspector.qml](../source/UI/Quick/CalendarInspector.qml) | 75 | 0 |
| A | [source/UI/Quick/CalendarVenueButton.qml](../source/UI/Quick/CalendarVenueButton.qml) | 19 | 0 |
| A | [source/UI/Quick/CalendarWorkspace.qml](../source/UI/Quick/CalendarWorkspace.qml) | 173 | 0 |
| A | [source/UI/Quick/CatalogWorkspace.qml](../source/UI/Quick/CatalogWorkspace.qml) | 57 | 0 |
| A | [source/UI/Quick/DataPackBar.qml](../source/UI/Quick/DataPackBar.qml) | 55 | 0 |
| A | [source/UI/Quick/DataTable.qml](../source/UI/Quick/DataTable.qml) | 255 | 0 |
| A | [source/UI/Quick/EmptyState.qml](../source/UI/Quick/EmptyState.qml) | 25 | 0 |
| A | [source/UI/Quick/FormGeneratorWorkspace.qml](../source/UI/Quick/FormGeneratorWorkspace.qml) | 83 | 0 |
| A | [source/UI/Quick/Home.qml](../source/UI/Quick/Home.qml) | 53 | 0 |
| A | [source/UI/Quick/HomeCard.qml](../source/UI/Quick/HomeCard.qml) | 51 | 0 |
| A | [source/UI/Quick/HomeHero.qml](../source/UI/Quick/HomeHero.qml) | 57 | 0 |
| A | [source/UI/Quick/HomeTools.qml](../source/UI/Quick/HomeTools.qml) | 25 | 0 |
| A | [source/UI/Quick/JudgeMarks.qml](../source/UI/Quick/JudgeMarks.qml) | 27 | 0 |
| A | [source/UI/Quick/JumpMetric.qml](../source/UI/Quick/JumpMetric.qml) | 49 | 0 |
| A | [source/UI/Quick/ModalScreen.qml](../source/UI/Quick/ModalScreen.qml) | 57 | 0 |
| A | [source/UI/Quick/NewSimulationPage.qml](../source/UI/Quick/NewSimulationPage.qml) | 58 | 0 |
| A | [source/UI/Quick/Node.qml](../source/UI/Quick/Node.qml) | 214 | 0 |
| A | [source/UI/Quick/NodeGroup.qml](../source/UI/Quick/NodeGroup.qml) | 78 | 0 |
| A | [source/UI/Quick/NumberField.qml](../source/UI/Quick/NumberField.qml) | 56 | 0 |
| A | [source/UI/Quick/PlotPanel.qml](../source/UI/Quick/PlotPanel.qml) | 35 | 0 |
| A | [source/UI/Quick/ProjectedControl.qml](../source/UI/Quick/ProjectedControl.qml) | 10 | 0 |
| A | [source/UI/Quick/QuickBridge.cpp](../source/UI/Quick/QuickBridge.cpp) | 1099 | 0 |
| A | [source/UI/Quick/QuickCalendar.cpp](../source/UI/Quick/QuickCalendar.cpp) | 529 | 0 |
| A | [source/UI/Quick/QuickCalendar.h](../source/UI/Quick/QuickCalendar.h) | 176 | 0 |
| A | [source/UI/Quick/QuickCalendarDates.cpp](../source/UI/Quick/QuickCalendarDates.cpp) | 179 | 0 |
| A | [source/UI/Quick/QuickPlot.cpp](../source/UI/Quick/QuickPlot.cpp) | 116 | 0 |
| A | [source/UI/Quick/QuickPlot.h](../source/UI/Quick/QuickPlot.h) | 38 | 0 |
| A | [source/UI/Quick/QuickRaceStats.h](../source/UI/Quick/QuickRaceStats.h) | 90 | 0 |
| A | [source/UI/Quick/QuickRoster.cpp](../source/UI/Quick/QuickRoster.cpp) | 129 | 0 |
| A | [source/UI/Quick/QuickRoster.h](../source/UI/Quick/QuickRoster.h) | 90 | 0 |
| A | [source/UI/Quick/QuickSurface.cpp](../source/UI/Quick/QuickSurface.cpp) | 17 | 0 |
| A | [source/UI/Quick/QuickSurface.h](../source/UI/Quick/QuickSurface.h) | 20 | 0 |
| A | [source/UI/Quick/QuickTableModel.cpp](../source/UI/Quick/QuickTableModel.cpp) | 253 | 0 |
| A | [source/UI/Quick/QuickUi.cpp](../source/UI/Quick/QuickUi.cpp) | 297 | 0 |
| A | [source/UI/Quick/QuickUi.h](../source/UI/Quick/QuickUi.h) | 232 | 0 |
| A | [source/UI/Quick/QuickView.h](../source/UI/Quick/QuickView.h) | 7 | 0 |
| A | [source/UI/Quick/RaceAthlete.qml](../source/UI/Quick/RaceAthlete.qml) | 63 | 0 |
| A | [source/UI/Quick/RaceControls.qml](../source/UI/Quick/RaceControls.qml) | 39 | 0 |
| A | [source/UI/Quick/RaceLastJump.qml](../source/UI/Quick/RaceLastJump.qml) | 89 | 0 |
| A | [source/UI/Quick/RaceOptions.qml](../source/UI/Quick/RaceOptions.qml) | 63 | 0 |
| A | [source/UI/Quick/RaceScreen.qml](../source/UI/Quick/RaceScreen.qml) | 103 | 0 |
| A | [source/UI/Quick/RaceSimulation.qml](../source/UI/Quick/RaceSimulation.qml) | 35 | 0 |
| A | [source/UI/Quick/RaceStatus.qml](../source/UI/Quick/RaceStatus.qml) | 41 | 0 |
| A | [source/UI/Quick/RosterDragArea.qml](../source/UI/Quick/RosterDragArea.qml) | 99 | 0 |
| A | [source/UI/Quick/RosterLibrary.qml](../source/UI/Quick/RosterLibrary.qml) | 35 | 0 |
| A | [source/UI/Quick/RosterOrder.qml](../source/UI/Quick/RosterOrder.qml) | 59 | 0 |
| A | [source/UI/Quick/RosterPresets.qml](../source/UI/Quick/RosterPresets.qml) | 25 | 0 |
| A | [source/UI/Quick/RosterRow.qml](../source/UI/Quick/RosterRow.qml) | 72 | 0 |
| A | [source/UI/Quick/RosterSummary.qml](../source/UI/Quick/RosterSummary.qml) | 17 | 0 |
| A | [source/UI/Quick/RosterWorkspace.qml](../source/UI/Quick/RosterWorkspace.qml) | 69 | 0 |
| A | [source/UI/Quick/SButton.qml](../source/UI/Quick/SButton.qml) | 55 | 0 |
| A | [source/UI/Quick/SCheck.qml](../source/UI/Quick/SCheck.qml) | 27 | 0 |
| A | [source/UI/Quick/SChoice.qml](../source/UI/Quick/SChoice.qml) | 145 | 0 |
| A | [source/UI/Quick/SField.qml](../source/UI/Quick/SField.qml) | 23 | 0 |
| A | [source/UI/Quick/Screen.qml](../source/UI/Quick/Screen.qml) | 12 | 0 |
| A | [source/UI/Quick/SeasonBar.qml](../source/UI/Quick/SeasonBar.qml) | 32 | 0 |
| A | [source/UI/Quick/SeasonFooter.qml](../source/UI/Quick/SeasonFooter.qml) | 25 | 0 |
| A | [source/UI/Quick/SectionRail.qml](../source/UI/Quick/SectionRail.qml) | 27 | 0 |
| A | [source/UI/Quick/SectionTabs.qml](../source/UI/Quick/SectionTabs.qml) | 68 | 0 |
| A | [source/UI/Quick/SetupFooter.qml](../source/UI/Quick/SetupFooter.qml) | 20 | 0 |
| A | [source/UI/Quick/SingleJumpsResultsScreen.qml](../source/UI/Quick/SingleJumpsResultsScreen.qml) | 72 | 0 |
| A | [source/UI/Quick/Surface.qml](../source/UI/Quick/Surface.qml) | 9 | 0 |
| A | [source/UI/Quick/TableRow.qml](../source/UI/Quick/TableRow.qml) | 114 | 0 |
| A | [source/UI/Quick/Theme.qml](../source/UI/Quick/Theme.qml) | 48 | 0 |
| A | [source/UI/Quick/TrackMark.qml](../source/UI/Quick/TrackMark.qml) | 20 | 0 |
| A | [source/UI/Quick/VersionBadge.qml](../source/UI/Quick/VersionBadge.qml) | 21 | 0 |
| A | [source/UI/Quick/WindEditor.qml](../source/UI/Quick/WindEditor.qml) | 50 | 0 |
| A | [source/UI/Quick/WindFields.qml](../source/UI/Quick/WindFields.qml) | 56 | 0 |
| A | [source/UI/Quick/WindSectorStrip.qml](../source/UI/Quick/WindSectorStrip.qml) | 46 | 0 |
| A | [source/UI/Quick/Workspace.qml](../source/UI/Quick/Workspace.qml) | 99 | 0 |
| A | [source/UI/Quick/fonts/Barlow-Medium.ttf](../source/UI/Quick/fonts/Barlow-Medium.ttf) | - | - |
| A | [source/UI/Quick/fonts/Barlow-Regular.ttf](../source/UI/Quick/fonts/Barlow-Regular.ttf) | - | - |
| A | [source/UI/Quick/fonts/Barlow-SemiBold.ttf](../source/UI/Quick/fonts/Barlow-SemiBold.ttf) | - | - |
| A | [source/UI/Quick/fonts/BarlowCondensed-ExtraBold.ttf](../source/UI/Quick/fonts/BarlowCondensed-ExtraBold.ttf) | - | - |
| A | [source/UI/Quick/fonts/IBMPlexMono-Regular.ttf](../source/UI/Quick/fonts/IBMPlexMono-Regular.ttf) | - | - |
| A | [source/UI/Quick/fonts/OFL-Barlow.txt](../source/UI/Quick/fonts/OFL-Barlow.txt) | 93 | 0 |
| A | [source/UI/Quick/fonts/OFL-BarlowCondensed.txt](../source/UI/Quick/fonts/OFL-BarlowCondensed.txt) | 93 | 0 |
| A | [source/UI/Quick/fonts/OFL-IBMPlexMono.txt](../source/UI/Quick/fonts/OFL-IBMPlexMono.txt) | 93 | 0 |
| A | [source/UI/Quick/icons/github-dark.svg](../source/UI/Quick/icons/github-dark.svg) | 1 | 0 |
| A | [source/UI/Quick/icons/github-light.svg](../source/UI/Quick/icons/github-light.svg) | 1 | 0 |
| A | [source/UI/Quick/icons/linkedin-dark.svg](../source/UI/Quick/icons/linkedin-dark.svg) | 1 | 0 |
| A | [source/UI/Quick/icons/linkedin-light.svg](../source/UI/Quick/icons/linkedin-light.svg) | 1 | 0 |
| A | [source/UI/Quick/qmldir](../source/UI/Quick/qmldir) | 1 | 0 |
| A | [source/UI/Quick/quick-ui.qrc](../source/UI/Quick/quick-ui.qrc) | 87 | 0 |

### Instrukcje i zasoby narzędzi deweloperskich

| Status | Plik | + | − |
| --- | --- | ---: | ---: |
| A | [.agents/skills/qt-canvas2d/LICENSE.txt](../.agents/skills/qt-canvas2d/LICENSE.txt) | 32 | 0 |
| A | [.agents/skills/qt-canvas2d/SKILL.md](../.agents/skills/qt-canvas2d/SKILL.md) | 241 | 0 |
| A | [.agents/skills/qt-canvas2d/references/api-reference.md](../.agents/skills/qt-canvas2d/references/api-reference.md) | 652 | 0 |
| A | [.agents/skills/qt-canvas2d/references/porting.md](../.agents/skills/qt-canvas2d/references/porting.md) | 191 | 0 |
| A | [.agents/skills/qt-canvas2d/references/recipes/analog-clock.qml](../.agents/skills/qt-canvas2d/references/recipes/analog-clock.qml) | 159 | 0 |
| A | [.agents/skills/qt-canvas2d/references/recipes/animated-line-chart.qml](../.agents/skills/qt-canvas2d/references/recipes/animated-line-chart.qml) | 174 | 0 |
| A | [.agents/skills/qt-canvas2d/references/recipes/bar-chart.qml](../.agents/skills/qt-canvas2d/references/recipes/bar-chart.qml) | 110 | 0 |
| A | [.agents/skills/qt-canvas2d/references/recipes/color-effects-and-clipping.qml](../.agents/skills/qt-canvas2d/references/recipes/color-effects-and-clipping.qml) | 175 | 0 |
| A | [.agents/skills/qt-canvas2d/references/recipes/freehand-drawing.qml](../.agents/skills/qt-canvas2d/references/recipes/freehand-drawing.qml) | 143 | 0 |
| A | [.agents/skills/qt-canvas2d/references/recipes/images-and-patterns.qml](../.agents/skills/qt-canvas2d/references/recipes/images-and-patterns.qml) | 110 | 0 |
| A | [.agents/skills/qt-canvas2d/references/recipes/level-meter.qml](../.agents/skills/qt-canvas2d/references/recipes/level-meter.qml) | 171 | 0 |
| A | [.agents/skills/qt-canvas2d/references/recipes/node-link-diagram.qml](../.agents/skills/qt-canvas2d/references/recipes/node-link-diagram.qml) | 139 | 0 |
| A | [.agents/skills/qt-canvas2d/references/recipes/oscilloscope.qml](../.agents/skills/qt-canvas2d/references/recipes/oscilloscope.qml) | 127 | 0 |
| A | [.agents/skills/qt-canvas2d/references/recipes/pan-zoom-viewport.qml](../.agents/skills/qt-canvas2d/references/recipes/pan-zoom-viewport.qml) | 189 | 0 |
| A | [.agents/skills/qt-canvas2d/references/recipes/pie-donut-chart.qml](../.agents/skills/qt-canvas2d/references/recipes/pie-donut-chart.qml) | 184 | 0 |
| A | [.agents/skills/qt-canvas2d/references/recipes/progress-ring.qml](../.agents/skills/qt-canvas2d/references/recipes/progress-ring.qml) | 101 | 0 |
| A | [.agents/skills/qt-canvas2d/references/recipes/radial-gauge.qml](../.agents/skills/qt-canvas2d/references/recipes/radial-gauge.qml) | 138 | 0 |
| A | [.agents/skills/qt-canvas2d/references/recipes/rotary-knob.qml](../.agents/skills/qt-canvas2d/references/recipes/rotary-knob.qml) | 214 | 0 |
| A | [.agents/skills/qt-canvas2d/references/recipes/scatter-plot.qml](../.agents/skills/qt-canvas2d/references/recipes/scatter-plot.qml) | 132 | 0 |
| A | [.agents/skills/qt-canvas2d/references/recipes/shadowed-card-button.qml](../.agents/skills/qt-canvas2d/references/recipes/shadowed-card-button.qml) | 129 | 0 |
| A | [.agents/skills/qt-canvas2d/references/recipes/sparkline-delegate.qml](../.agents/skills/qt-canvas2d/references/recipes/sparkline-delegate.qml) | 148 | 0 |
| A | [.agents/skills/qt-canvas2d/references/recipes/static-shapes.qml](../.agents/skills/qt-canvas2d/references/recipes/static-shapes.qml) | 116 | 0 |
| A | [.agents/skills/qt-canvas2d/references/recipes/text-panel.qml](../.agents/skills/qt-canvas2d/references/recipes/text-panel.qml) | 147 | 0 |
| A | [.agents/skills/qt-canvas2d/references/rules.md](../.agents/skills/qt-canvas2d/references/rules.md) | 65 | 0 |
| A | [.agents/skills/qt-cmake-project/LICENSE.txt](../.agents/skills/qt-cmake-project/LICENSE.txt) | 32 | 0 |
| A | [.agents/skills/qt-cmake-project/SKILL.md](../.agents/skills/qt-cmake-project/SKILL.md) | 165 | 0 |
| A | [.agents/skills/qt-cmake-project/references/common-mistakes.md](../.agents/skills/qt-cmake-project/references/common-mistakes.md) | 271 | 0 |
| A | [.agents/skills/qt-cmake-project/references/configure.md](../.agents/skills/qt-cmake-project/references/configure.md) | 37 | 0 |
| A | [.agents/skills/qt-cmake-project/references/modular-architecture.md](../.agents/skills/qt-cmake-project/references/modular-architecture.md) | 268 | 0 |
| A | [.agents/skills/qt-cmake-project/references/qml-integration.md](../.agents/skills/qt-cmake-project/references/qml-integration.md) | 260 | 0 |
| A | [.agents/skills/qt-cmake-project/references/resources.md](../.agents/skills/qt-cmake-project/references/resources.md) | 229 | 0 |
| A | [.agents/skills/qt-cmake-project/references/simple-project.md](../.agents/skills/qt-cmake-project/references/simple-project.md) | 122 | 0 |
| A | [.agents/skills/qt-cpp-docs/LICENSE.txt](../.agents/skills/qt-cpp-docs/LICENSE.txt) | 32 | 0 |
| A | [.agents/skills/qt-cpp-docs/SKILL.md](../.agents/skills/qt-cpp-docs/SKILL.md) | 410 | 0 |
| A | [.agents/skills/qt-cpp-review/LICENSE.txt](../.agents/skills/qt-cpp-review/LICENSE.txt) | 32 | 0 |
| A | [.agents/skills/qt-cpp-review/SKILL.md](../.agents/skills/qt-cpp-review/SKILL.md) | 480 | 0 |
| A | [.agents/skills/qt-cpp-review/platforms/windsurf.md](../.agents/skills/qt-cpp-review/platforms/windsurf.md) | 34 | 0 |
| A | [.agents/skills/qt-cpp-review/references/lint-scripts/qt_review_lint.py](../.agents/skills/qt-cpp-review/references/lint-scripts/qt_review_lint.py) | 785 | 0 |
| A | [.agents/skills/qt-cpp-review/references/qt-deprecated-classes.md](../.agents/skills/qt-cpp-review/references/qt-deprecated-classes.md) | 38 | 0 |
| A | [.agents/skills/qt-cpp-review/references/qt-framework-checklist.md](../.agents/skills/qt-cpp-review/references/qt-framework-checklist.md) | 177 | 0 |
| A | [.agents/skills/qt-cpp-review/references/qt-review-checklist.md](../.agents/skills/qt-cpp-review/references/qt-review-checklist.md) | 293 | 0 |
| A | [.agents/skills/qt-figma-component-generation/SKILL.md](../.agents/skills/qt-figma-component-generation/SKILL.md) | 347 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/BadgeLabel.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/BadgeLabel.qml) | 122 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/BadgeLabelStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/BadgeLabelStyle.qml) | 117 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/BadgeNotification.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/BadgeNotification.qml) | 110 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/BadgeNotificationStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/BadgeNotificationStyle.qml) | 96 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/Button.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/Button.qml) | 177 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/ButtonStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/ButtonStyle.qml) | 215 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/Card.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/Card.qml) | 651 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/CardStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/CardStyle.qml) | 99 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/CheckBox.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/CheckBox.qml) | 121 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/CheckBoxStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/CheckBoxStyle.qml) | 143 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/ComboBox.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/ComboBox.qml) | 206 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/ComboBoxStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/ComboBoxStyle.qml) | 169 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/Dialog.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/Dialog.qml) | 136 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/DialogButtonBox.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/DialogButtonBox.qml) | 52 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/DialogButtonBoxStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/DialogButtonBoxStyle.qml) | 47 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/DialogStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/DialogStyle.qml) | 79 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/IconButton.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/IconButton.qml) | 112 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/IconButtonStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/IconButtonStyle.qml) | 159 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/Indicator.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/Indicator.qml) | 72 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/IndicatorStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/IndicatorStyle.qml) | 54 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/ItemDelegate.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/ItemDelegate.qml) | 84 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/ItemDelegateStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/ItemDelegateStyle.qml) | 84 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/Label.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/Label.qml) | 50 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/LabelStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/LabelStyle.qml) | 57 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/LinkInline.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/LinkInline.qml) | 68 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/LinkInlineStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/LinkInlineStyle.qml) | 59 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/LinkList.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/LinkList.qml) | 91 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/LinkListStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/LinkListStyle.qml) | 64 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/LinkStandalone.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/LinkStandalone.qml) | 91 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/LinkStandaloneStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/LinkStandaloneStyle.qml) | 60 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/ListItem.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/ListItem.qml) | 193 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/ListItemStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/ListItemStyle.qml) | 95 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/Menu.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/Menu.qml) | 63 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/MenuButton.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/MenuButton.qml) | 138 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/MenuButtonStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/MenuButtonStyle.qml) | 101 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/MenuItem.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/MenuItem.qml) | 77 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/MenuItemStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/MenuItemStyle.qml) | 58 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/MenuStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/MenuStyle.qml) | 49 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/PaginationDot.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/PaginationDot.qml) | 85 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/PaginationDotStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/PaginationDotStyle.qml) | 79 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/PaginationIndex.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/PaginationIndex.qml) | 77 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/PaginationIndexStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/PaginationIndexStyle.qml) | 67 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/PaginationNumber.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/PaginationNumber.qml) | 277 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/PaginationNumberStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/PaginationNumberStyle.qml) | 120 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/Popup.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/Popup.qml) | 44 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/PopupStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/PopupStyle.qml) | 41 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/ProgressBar.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/ProgressBar.qml) | 99 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/ProgressBarStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/ProgressBarStyle.qml) | 56 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/RadioButton.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/RadioButton.qml) | 95 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/RadioButtonStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/RadioButtonStyle.qml) | 91 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/RangeSlider.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/RangeSlider.qml) | 119 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/RangeSliderStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/RangeSliderStyle.qml) | 108 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/ScrollBar.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/ScrollBar.qml) | 66 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/ScrollBarStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/ScrollBarStyle.qml) | 112 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/SearchField.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/SearchField.qml) | 181 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/SearchFieldStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/SearchFieldStyle.qml) | 122 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/SearchMenu.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/SearchMenu.qml) | 390 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/SearchMenuStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/SearchMenuStyle.qml) | 90 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/Slider.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/Slider.qml) | 109 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/SliderStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/SliderStyle.qml) | 111 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/SortFilterModel.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/SortFilterModel.qml) | 60 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/SpinBox.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/SpinBox.qml) | 185 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/SpinBoxStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/SpinBoxStyle.qml) | 109 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/Switch.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/Switch.qml) | 113 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/SwitchStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/SwitchStyle.qml) | 86 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/TabBar.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/TabBar.qml) | 68 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/TabBarStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/TabBarStyle.qml) | 41 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/TabButton.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/TabButton.qml) | 139 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/TabButtonStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/TabButtonStyle.qml) | 140 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/Tag.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/Tag.qml) | 145 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/TagStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/TagStyle.qml) | 101 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/TextField.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/TextField.qml) | 220 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/TextFieldStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/TextFieldStyle.qml) | 132 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/ToolTip.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/ToolTip.qml) | 197 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/ToolTipStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/ToolTipStyle.qml) | 82 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/Toolbar.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/Toolbar.qml) | 54 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/ToolbarStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/ToolbarStyle.qml) | 45 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/TreeViewDelegate.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/TreeViewDelegate.qml) | 131 | 0 |
| A | [.agents/skills/qt-figma-component-generation/assets/qt-controls/TreeViewDelegateStyle.qml](../.agents/skills/qt-figma-component-generation/assets/qt-controls/TreeViewDelegateStyle.qml) | 85 | 0 |
| A | [.agents/skills/qt-figma-component-generation/references/Button.qml](../.agents/skills/qt-figma-component-generation/references/Button.qml) | 215 | 0 |
| A | [.agents/skills/qt-figma-component-generation/references/Checkbox.qml](../.agents/skills/qt-figma-component-generation/references/Checkbox.qml) | 150 | 0 |
| A | [.agents/skills/qt-figma-component-generation/references/Select.qml](../.agents/skills/qt-figma-component-generation/references/Select.qml) | 228 | 0 |
| A | [.agents/skills/qt-figma-component-generation/references/TextField.qml](../.agents/skills/qt-figma-component-generation/references/TextField.qml) | 150 | 0 |
| A | [.agents/skills/qt-figma-component-generation/references/Toggle.qml](../.agents/skills/qt-figma-component-generation/references/Toggle.qml) | 108 | 0 |
| A | [.agents/skills/qt-figma-token-extraction/SKILL.md](../.agents/skills/qt-figma-token-extraction/SKILL.md) | 593 | 0 |
| A | [.agents/skills/qt-figma-token-extraction/examples/FontInterface.qml](../.agents/skills/qt-figma-token-extraction/examples/FontInterface.qml) | 65 | 0 |
| A | [.agents/skills/qt-figma-token-extraction/examples/Primitives.qml](../.agents/skills/qt-figma-token-extraction/examples/Primitives.qml) | 51 | 0 |
| A | [.agents/skills/qt-figma-token-extraction/examples/Spacing.qml](../.agents/skills/qt-figma-token-extraction/examples/Spacing.qml) | 63 | 0 |
| A | [.agents/skills/qt-figma-token-extraction/examples/Theme.qml](../.agents/skills/qt-figma-token-extraction/examples/Theme.qml) | 68 | 0 |
| A | [.agents/skills/qt-figma-token-extraction/examples/Typography.qml](../.agents/skills/qt-figma-token-extraction/examples/Typography.qml) | 48 | 0 |
| A | [.agents/skills/qt-figma-token-extraction/references/token-mapping.md](../.agents/skills/qt-figma-token-extraction/references/token-mapping.md) | 133 | 0 |
| A | [.agents/skills/qt-qml-docs/LICENSE.txt](../.agents/skills/qt-qml-docs/LICENSE.txt) | 32 | 0 |
| A | [.agents/skills/qt-qml-docs/SKILL.md](../.agents/skills/qt-qml-docs/SKILL.md) | 184 | 0 |
| A | [.agents/skills/qt-qml-docs/platforms/windsurf.md](../.agents/skills/qt-qml-docs/platforms/windsurf.md) | 28 | 0 |
| A | [.agents/skills/qt-qml-profiler/LICENSE.txt](../.agents/skills/qt-qml-profiler/LICENSE.txt) | 32 | 0 |
| A | [.agents/skills/qt-qml-profiler/SKILL.md](../.agents/skills/qt-qml-profiler/SKILL.md) | 542 | 0 |
| A | [.agents/skills/qt-qml-profiler/references/qml-performance-anti-patterns.md](../.agents/skills/qt-qml-profiler/references/qml-performance-anti-patterns.md) | 150 | 0 |
| A | [.agents/skills/qt-qml-profiler/references/scripts/parse-qmlprofiler-trace.py](../.agents/skills/qt-qml-profiler/references/scripts/parse-qmlprofiler-trace.py) | 412 | 0 |
| A | [.agents/skills/qt-qml-review/LICENSE.txt](../.agents/skills/qt-qml-review/LICENSE.txt) | 32 | 0 |
| A | [.agents/skills/qt-qml-review/SKILL.md](../.agents/skills/qt-qml-review/SKILL.md) | 412 | 0 |
| A | [.agents/skills/qt-qml-review/platforms/windsurf.md](../.agents/skills/qt-qml-review/platforms/windsurf.md) | 44 | 0 |
| A | [.agents/skills/qt-qml-review/references/lint-scripts/qt_qml_lint.py](../.agents/skills/qt-qml-review/references/lint-scripts/qt_qml_lint.py) | 1486 | 0 |
| A | [.agents/skills/qt-qml-review/references/qt-qml-review-checklist.md](../.agents/skills/qt-qml-review/references/qt-qml-review-checklist.md) | 483 | 0 |
| A | [.agents/skills/qt-qml-test-run/LICENSE.txt](../.agents/skills/qt-qml-test-run/LICENSE.txt) | 32 | 0 |
| A | [.agents/skills/qt-qml-test-run/SKILL.md](../.agents/skills/qt-qml-test-run/SKILL.md) | 432 | 0 |
| A | [.agents/skills/qt-qml-test-run/references/qt-quick-test-cmake.md](../.agents/skills/qt-qml-test-run/references/qt-quick-test-cmake.md) | 491 | 0 |
| A | [.agents/skills/qt-qml-test-run/references/qt-quick-test-report-format.md](../.agents/skills/qt-qml-test-run/references/qt-quick-test-report-format.md) | 176 | 0 |
| A | [.agents/skills/qt-qml-test-run/references/scripts/parse-qmltestrunner-output.py](../.agents/skills/qt-qml-test-run/references/scripts/parse-qmltestrunner-output.py) | 216 | 0 |
| A | [.agents/skills/qt-qml-test/LICENSE.txt](../.agents/skills/qt-qml-test/LICENSE.txt) | 32 | 0 |
| A | [.agents/skills/qt-qml-test/SKILL.md](../.agents/skills/qt-qml-test/SKILL.md) | 403 | 0 |
| A | [.agents/skills/qt-qml-test/platforms/copilot.prompt.md](../.agents/skills/qt-qml-test/platforms/copilot.prompt.md) | 378 | 0 |
| A | [.agents/skills/qt-qml-test/references/qt-quick-test-controls.md](../.agents/skills/qt-qml-test/references/qt-quick-test-controls.md) | 423 | 0 |
| A | [.agents/skills/qt-qml-test/references/qt-quick-test-pitfalls.md](../.agents/skills/qt-qml-test/references/qt-quick-test-pitfalls.md) | 212 | 0 |
| A | [.agents/skills/qt-qml-test/references/qt-quick-test-pre-send-scan.md](../.agents/skills/qt-qml-test/references/qt-quick-test-pre-send-scan.md) | 35 | 0 |
| A | [.agents/skills/qt-qml-test/references/qt-quick-test-project-context.md](../.agents/skills/qt-qml-test/references/qt-quick-test-project-context.md) | 31 | 0 |
| A | [.agents/skills/qt-qml-test/references/qt-quick-test-properties.md](../.agents/skills/qt-qml-test/references/qt-quick-test-properties.md) | 206 | 0 |
| A | [.agents/skills/qt-qml-test/references/qt-quick-test-rules.md](../.agents/skills/qt-qml-test/references/qt-quick-test-rules.md) | 339 | 0 |
| A | [.agents/skills/qt-qml-test/references/qt-quick-test-source-import.md](../.agents/skills/qt-qml-test/references/qt-quick-test-source-import.md) | 71 | 0 |
| A | [.agents/skills/qt-qml-test/references/qt-quick-test-template.md](../.agents/skills/qt-qml-test/references/qt-quick-test-template.md) | 489 | 0 |
| A | [.agents/skills/qt-qml/LICENSE.txt](../.agents/skills/qt-qml/LICENSE.txt) | 32 | 0 |
| A | [.agents/skills/qt-qml/SKILL.md](../.agents/skills/qt-qml/SKILL.md) | 250 | 0 |
| A | [.agents/skills/qt-qml/platforms/windsurf.md](../.agents/skills/qt-qml/platforms/windsurf.md) | 37 | 0 |
| A | [.agents/skills/qt-qml/references/qmlpreview.md](../.agents/skills/qt-qml/references/qmlpreview.md) | 159 | 0 |
| A | [.agents/skills/qt-ui-design/LICENSE.txt](../.agents/skills/qt-ui-design/LICENSE.txt) | 32 | 0 |
| A | [.agents/skills/qt-ui-design/SKILL.md](../.agents/skills/qt-ui-design/SKILL.md) | 334 | 0 |

### Konfiguracja projektu i pozostałe pliki

| Status | Plik | + | − |
| --- | --- | ---: | ---: |
| M | [.gitignore](../.gitignore) | 5 | 0 |
| A | [WINDOWS_SETUP.MD](../WINDOWS_SETUP.MD) | 288 | 0 |
| M | [assets.qrc](../assets.qrc) | 1 | 0 |
| A | [img/author/konrad.jpg](../img/author/konrad.jpg) | - | - |
| M | [sj-sim.pro](../sj-sim.pro) | 57 | 0 |
| A | [skills-lock.json](../skills-lock.json) | 83 | 0 |

### Kontrolery, modele i logika C++

| Status | Plik | + | − |
| --- | --- | ---: | ---: |
| M | [source/UI/AppSettings/AppSettingsWindow.cpp](../source/UI/AppSettings/AppSettingsWindow.cpp) | 1 | 0 |
| M | [source/UI/AppSettings/CountriesEditorTableModel.cpp](../source/UI/AppSettings/CountriesEditorTableModel.cpp) | 1 | 1 |
| A | [source/UI/AuthorWindow.cpp](../source/UI/AuthorWindow.cpp) | 77 | 0 |
| A | [source/UI/AuthorWindow.h](../source/UI/AuthorWindow.h) | 24 | 0 |
| M | [source/UI/Competition/CompetitionConfigWindow.cpp](../source/UI/Competition/CompetitionConfigWindow.cpp) | 42 | 88 |
| M | [source/UI/Competition/CompetitionConfigWindow.h](../source/UI/Competition/CompetitionConfigWindow.h) | 5 | 2 |
| M | [source/UI/Competition/CompetitionManagerWindow.cpp](../source/UI/Competition/CompetitionManagerWindow.cpp) | 167 | 69 |
| M | [source/UI/Competition/CompetitionManagerWindow.h](../source/UI/Competition/CompetitionManagerWindow.h) | 5 | 0 |
| M | [source/UI/Competition/JumperCompetitionResultsWidget.cpp](../source/UI/Competition/JumperCompetitionResultsWidget.cpp) | 20 | 0 |
| M | [source/UI/Competition/Results/KOGroupResultsTableModel.cpp](../source/UI/Competition/Results/KOGroupResultsTableModel.cpp) | 12 | 11 |
| M | [source/UI/Competition/Results/ResultsTableModel.cpp](../source/UI/Competition/Results/ResultsTableModel.cpp) | 24 | 45 |
| M | [source/UI/Competition/Results/TeamResultsTreeModel.cpp](../source/UI/Competition/Results/TeamResultsTreeModel.cpp) | 33 | 46 |
| M | [source/UI/Competition/StartList/StartListModel.cpp](../source/UI/Competition/StartList/StartListModel.cpp) | 31 | 26 |
| M | [source/UI/Competition/StartList/StartListModel.h](../source/UI/Competition/StartList/StartListModel.h) | 5 | 2 |
| M | [source/UI/DatabaseEditor/DatabaseEditorWindow.cpp](../source/UI/DatabaseEditor/DatabaseEditorWindow.cpp) | 200 | 3 |
| M | [source/UI/DatabaseEditor/DatabaseEditorWindow.h](../source/UI/DatabaseEditor/DatabaseEditorWindow.h) | 16 | 0 |
| M | [source/UI/DatabaseEditor/DatabaseItemsListView.cpp](../source/UI/DatabaseEditor/DatabaseItemsListView.cpp) | 2 | 3 |
| M | [source/UI/DatabaseEditor/DatabaseItemsListView.h](../source/UI/DatabaseEditor/DatabaseItemsListView.h) | 1 | 0 |
| M | [source/UI/DatabaseEditor/ListModels/CalendarsListModel.cpp](../source/UI/DatabaseEditor/ListModels/CalendarsListModel.cpp) | 9 | 3 |
| M | [source/UI/DatabaseEditor/ListModels/CalendarsListModel.h](../source/UI/DatabaseEditor/ListModels/CalendarsListModel.h) | 1 | 0 |
| M | [source/UI/EditorWidgets/ClassificationEditorWidget.cpp](../source/UI/EditorWidgets/ClassificationEditorWidget.cpp) | 153 | 52 |
| M | [source/UI/EditorWidgets/ClassificationEditorWidget.h](../source/UI/EditorWidgets/ClassificationEditorWidget.h) | 22 | 0 |
| M | [source/UI/EditorWidgets/CompetitionRulesEditorWidget.cpp](../source/UI/EditorWidgets/CompetitionRulesEditorWidget.cpp) | 12 | 37 |
| M | [source/UI/EditorWidgets/JumperEditorWidget.cpp](../source/UI/EditorWidgets/JumperEditorWidget.cpp) | 77 | 2 |
| M | [source/UI/EditorWidgets/JumperEditorWidget.h](../source/UI/EditorWidgets/JumperEditorWidget.h) | 14 | 0 |
| M | [source/UI/EditorWidgets/PointsForPlacesPresetEditorWidget.cpp](../source/UI/EditorWidgets/PointsForPlacesPresetEditorWidget.cpp) | 3 | 7 |
| M | [source/UI/EditorWidgets/RoundInfoEditorWidget.cpp](../source/UI/EditorWidgets/RoundInfoEditorWidget.cpp) | 49 | 2 |
| M | [source/UI/EditorWidgets/RoundInfoEditorWidget.h](../source/UI/EditorWidgets/RoundInfoEditorWidget.h) | 11 | 0 |
| M | [source/UI/EditorWidgets/TeamsEditing/Models/PointsForPlacesTableModel.cpp](../source/UI/EditorWidgets/TeamsEditing/Models/PointsForPlacesTableModel.cpp) | 24 | 35 |
| M | [source/UI/EditorWidgets/TeamsEditing/Models/PointsForPlacesTableModel.h](../source/UI/EditorWidgets/TeamsEditing/Models/PointsForPlacesTableModel.h) | 2 | 1 |
| M | [source/UI/EditorWidgets/TeamsEditing/TeamEditorWidget.cpp](../source/UI/EditorWidgets/TeamsEditing/TeamEditorWidget.cpp) | 5 | 0 |
| M | [source/UI/EditorWidgets/WindsGeneratorSettingsEditorWidget.cpp](../source/UI/EditorWidgets/WindsGeneratorSettingsEditorWidget.cpp) | 3 | 24 |
| M | [source/UI/EditorWidgets/WindsGeneratorSettingsWidgetInputItem.cpp](../source/UI/EditorWidgets/WindsGeneratorSettingsWidgetInputItem.cpp) | 14 | 9 |
| M | [source/UI/EditorWidgets/WindsGeneratorSettingsWidgetInputItem.h](../source/UI/EditorWidgets/WindsGeneratorSettingsWidgetInputItem.h) | 1 | 0 |
| M | [source/UI/FormGenerator/JumperFormGeneratorsSettingsTableModel.cpp](../source/UI/FormGenerator/JumperFormGeneratorsSettingsTableModel.cpp) | 17 | 42 |
| M | [source/UI/FormGenerator/JumpersFormGeneratorConfigWindow.cpp](../source/UI/FormGenerator/JumpersFormGeneratorConfigWindow.cpp) | 54 | 37 |
| M | [source/UI/FormGenerator/JumpersFormGeneratorConfigWindow.h](../source/UI/FormGenerator/JumpersFormGeneratorConfigWindow.h) | 2 | 0 |
| M | [source/UI/ResponsiveWindowUtils.h](../source/UI/ResponsiveWindowUtils.h) | 1 | 1 |
| M | [source/UI/ResultsShowing/JumpDataDetailedInfoWindow.cpp](../source/UI/ResultsShowing/JumpDataDetailedInfoWindow.cpp) | 11 | 2 |
| M | [source/UI/Seasons/CalendarEditor/CalendarEditorTableModel.cpp](../source/UI/Seasons/CalendarEditor/CalendarEditorTableModel.cpp) | 4 | 0 |
| M | [source/UI/Seasons/CalendarEditor/CalendarEditorTableModel.h](../source/UI/Seasons/CalendarEditor/CalendarEditorTableModel.h) | 2 | 0 |
| M | [source/UI/Seasons/CalendarEditor/CalendarEditorWidget.cpp](../source/UI/Seasons/CalendarEditor/CalendarEditorWidget.cpp) | 49 | 54 |
| M | [source/UI/Seasons/Classifications/ClassificationResultsTableModel.cpp](../source/UI/Seasons/Classifications/ClassificationResultsTableModel.cpp) | 15 | 10 |
| M | [source/UI/Seasons/Classifications/ClassificationResultsTableModel.h](../source/UI/Seasons/Classifications/ClassificationResultsTableModel.h) | 1 | 0 |
| M | [source/UI/Seasons/CompetitionInfoEditorWidget.cpp](../source/UI/Seasons/CompetitionInfoEditorWidget.cpp) | 1 | 0 |
| M | [source/UI/Seasons/NewSeasonConfiguratorWindow.cpp](../source/UI/Seasons/NewSeasonConfiguratorWindow.cpp) | 42 | 9 |
| M | [source/UI/Seasons/NewSimulationSaveConfigurationWindow.cpp](../source/UI/Seasons/NewSimulationSaveConfigurationWindow.cpp) | 56 | 1 |
| M | [source/UI/Seasons/NewSimulationSaveConfigurationWindow.h](../source/UI/Seasons/NewSimulationSaveConfigurationWindow.h) | 6 | 0 |
| M | [source/UI/Seasons/SimulationSaveManagerWindow.cpp](../source/UI/Seasons/SimulationSaveManagerWindow.cpp) | 155 | 81 |
| M | [source/UI/Seasons/SimulationSaveManagerWindow.h](../source/UI/Seasons/SimulationSaveManagerWindow.h) | 9 | 2 |
| M | [source/UI/Seasons/SimulationSaveManagerWindow.ui](../source/UI/Seasons/SimulationSaveManagerWindow.ui) | 6 | 0 |
| M | [source/UI/Seasons/SimulationSavesWindow.cpp](../source/UI/Seasons/SimulationSavesWindow.cpp) | 5 | 17 |
| M | [source/UI/Seasons/Stats/FilteringWidgets/ClassificationsCheckBoxesWidget.cpp](../source/UI/Seasons/Stats/FilteringWidgets/ClassificationsCheckBoxesWidget.cpp) | 5 | 8 |
| M | [source/UI/Seasons/Stats/FilteringWidgets/ClassificationsCheckBoxesWidget.h](../source/UI/Seasons/Stats/FilteringWidgets/ClassificationsCheckBoxesWidget.h) | 1 | 1 |
| M | [source/UI/Seasons/Stats/FilteringWidgets/CompetitionsRangeComboBoxesWidget.cpp](../source/UI/Seasons/Stats/FilteringWidgets/CompetitionsRangeComboBoxesWidget.cpp) | 14 | 45 |
| M | [source/UI/Seasons/Stats/FilteringWidgets/CompetitionsRangeComboBoxesWidget.h](../source/UI/Seasons/Stats/FilteringWidgets/CompetitionsRangeComboBoxesWidget.h) | 3 | 2 |
| M | [source/UI/Seasons/Stats/JumperStatsWindow.cpp](../source/UI/Seasons/Stats/JumperStatsWindow.cpp) | 32 | 35 |
| M | [source/UI/Seasons/Stats/SimulationRatingsWindow.cpp](../source/UI/Seasons/Stats/SimulationRatingsWindow.cpp) | 73 | 63 |
| M | [source/UI/Seasons/Stats/SimulationRatingsWindow.h](../source/UI/Seasons/Stats/SimulationRatingsWindow.h) | 1 | 0 |
| M | [source/UI/SingleJumps/SingleJumpsConfigWindow.cpp](../source/UI/SingleJumps/SingleJumpsConfigWindow.cpp) | 46 | 3 |
| M | [source/UI/SingleJumps/SingleJumpsConfigWindow.h](../source/UI/SingleJumps/SingleJumpsConfigWindow.h) | 1 | 0 |
| M | [source/UI/mainwindow.cpp](../source/UI/mainwindow.cpp) | 32 | 1 |
| M | [source/UI/mainwindow.h](../source/UI/mainwindow.h) | 1 | 0 |
| M | [source/competitions/AbstractCompetitionManager.cpp](../source/competitions/AbstractCompetitionManager.cpp) | 213 | 89 |
| M | [source/competitions/AbstractCompetitionManager.h](../source/competitions/AbstractCompetitionManager.h) | 12 | 0 |
| A | [source/competitions/CompetitionAdvancement.h](../source/competitions/CompetitionAdvancement.h) | 51 | 0 |
| M | [source/competitions/CompetitionInfo.cpp](../source/competitions/CompetitionInfo.cpp) | 28 | 23 |
| M | [source/competitions/CompetitionInfo.h](../source/competitions/CompetitionInfo.h) | 17 | 0 |
| M | [source/competitions/CompetitionManagers/IndividualCompetitionManager.cpp](../source/competitions/CompetitionManagers/IndividualCompetitionManager.cpp) | 80 | 145 |
| M | [source/competitions/CompetitionManagers/KORoundManager.cpp](../source/competitions/CompetitionManagers/KORoundManager.cpp) | 52 | 38 |
| M | [source/competitions/CompetitionManagers/KORoundManager.h](../source/competitions/CompetitionManagers/KORoundManager.h) | 1 | 0 |
| M | [source/competitions/CompetitionManagers/TeamCompetitionManager.cpp](../source/competitions/CompetitionManagers/TeamCompetitionManager.cpp) | 46 | 110 |
| M | [source/competitions/CompetitionResults.cpp](../source/competitions/CompetitionResults.cpp) | 51 | 120 |
| M | [source/competitions/CompetitionResults.h](../source/competitions/CompetitionResults.h) | 1 | 0 |
| M | [source/competitions/CompetitionRules.cpp](../source/competitions/CompetitionRules.cpp) | 29 | 0 |
| M | [source/competitions/CompetitionRules.h](../source/competitions/CompetitionRules.h) | 2 | 0 |
| M | [source/competitions/CompetitionSingleResult.cpp](../source/competitions/CompetitionSingleResult.cpp) | 66 | 28 |
| M | [source/competitions/CompetitionSingleResult.h](../source/competitions/CompetitionSingleResult.h) | 13 | 7 |
| M | [source/competitions/RoundInfo.cpp](../source/competitions/RoundInfo.cpp) | 10 | 0 |
| M | [source/competitions/RoundInfo.h](../source/competitions/RoundInfo.h) | 7 | 0 |
| M | [source/global/Country.cpp](../source/global/Country.cpp) | 3 | 2 |
| A | [source/global/CountryCodes.cpp](../source/global/CountryCodes.cpp) | 38 | 0 |
| A | [source/global/CountryCodes.h](../source/global/CountryCodes.h) | 8 | 0 |
| M | [source/global/CountryFlagsManager.cpp](../source/global/CountryFlagsManager.cpp) | 2 | 4 |
| A | [source/global/DataPack.cpp](../source/global/DataPack.cpp) | 96 | 0 |
| A | [source/global/DataPack.h](../source/global/DataPack.h) | 25 | 0 |
| M | [source/global/GlobalDatabase.cpp](../source/global/GlobalDatabase.cpp) | 138 | 2 |
| M | [source/global/GlobalDatabase.h](../source/global/GlobalDatabase.h) | 15 | 0 |
| M | [source/global/GlobalSimulationSettings.cpp](../source/global/GlobalSimulationSettings.cpp) | 15 | 0 |
| M | [source/global/GlobalSimulationSettings.h](../source/global/GlobalSimulationSettings.h) | 18 | 0 |
| M | [source/global/MyRandom.cpp](../source/global/MyRandom.cpp) | 48 | 45 |
| M | [source/global/PointsForPlacesPreset.cpp](../source/global/PointsForPlacesPreset.cpp) | 1 | 1 |
| M | [source/global/Uuid.h](../source/global/Uuid.h) | 4 | 0 |
| M | [source/main.cpp](../source/main.cpp) | 11 | 0 |
| M | [source/seasons/Classification.cpp](../source/seasons/Classification.cpp) | 131 | 242 |
| M | [source/seasons/Classification.h](../source/seasons/Classification.h) | 8 | 0 |
| A | [source/seasons/ClassificationScoring.cpp](../source/seasons/ClassificationScoring.cpp) | 58 | 0 |
| A | [source/seasons/ClassificationScoring.h](../source/seasons/ClassificationScoring.h) | 34 | 0 |
| M | [source/seasons/ClassificationSingleResult.cpp](../source/seasons/ClassificationSingleResult.cpp) | 15 | 39 |
| M | [source/seasons/ClassificationSingleResult.h](../source/seasons/ClassificationSingleResult.h) | 2 | 0 |
| M | [source/seasons/Season.cpp](../source/seasons/Season.cpp) | 17 | 0 |
| M | [source/seasons/Season.h](../source/seasons/Season.h) | 6 | 0 |
| M | [source/seasons/SeasonCalendar.cpp](../source/seasons/SeasonCalendar.cpp) | 15 | 19 |
| M | [source/seasons/SeasonCalendarPreset.cpp](../source/seasons/SeasonCalendarPreset.cpp) | 16 | 3 |
| M | [source/seasons/SimulationSave.cpp](../source/seasons/SimulationSave.cpp) | 1 | 13 |
| M | [source/simulator/Characteristic.h](../source/simulator/Characteristic.h) | 7 | 6 |
| M | [source/simulator/Hill.cpp](../source/simulator/Hill.cpp) | 6 | 4 |
| M | [source/simulator/JumpData.cpp](../source/simulator/JumpData.cpp) | 17 | 12 |
| M | [source/simulator/JumpManipulator.cpp](../source/simulator/JumpManipulator.cpp) | 3 | 1 |
| M | [source/simulator/JumpSimulator.cpp](../source/simulator/JumpSimulator.cpp) | 49 | 54 |
| M | [source/simulator/JumpSimulator.h](../source/simulator/JumpSimulator.h) | 3 | 1 |
| M | [source/simulator/Jumper.cpp](../source/simulator/Jumper.cpp) | 8 | 2 |
| M | [source/simulator/Jumper.h](../source/simulator/Jumper.h) | 12 | 1 |
| M | [source/simulator/Team.cpp](../source/simulator/Team.cpp) | 9 | 4 |
| M | [source/simulator/Wind.cpp](../source/simulator/Wind.cpp) | 4 | 2 |
| M | [source/simulator/WindsCalculator.cpp](../source/simulator/WindsCalculator.cpp) | 14 | 2 |
| M | [source/simulator/WindsCalculator.h](../source/simulator/WindsCalculator.h) | 1 | 0 |
| M | [source/simulator/wind-generation/WindsGenerator.cpp](../source/simulator/wind-generation/WindsGenerator.cpp) | 8 | 2 |
| M | [source/simulator/wind-generation/WindsGenerator.h](../source/simulator/wind-generation/WindsGenerator.h) | 1 | 0 |
| M | [source/single-jumps/SingleJumpsManager.cpp](../source/single-jumps/SingleJumpsManager.cpp) | 83 | 11 |
| M | [source/single-jumps/SingleJumpsManager.h](../source/single-jumps/SingleJumpsManager.h) | 9 | 0 |
| M | [source/utilities/ClassWithCharacteristics.cpp](../source/utilities/ClassWithCharacteristics.cpp) | 4 | 10 |
| M | [source/utilities/ClassWithCharacteristics.h](../source/utilities/ClassWithCharacteristics.h) | 1 | 1 |

### Skrypty budowania, uruchamiania i testowania

| Status | Plik | + | − |
| --- | --- | ---: | ---: |
| A | [scripts/build-qml-preview.sh](../scripts/build-qml-preview.sh) | 21 | 0 |
| A | [scripts/build-quick-ui.sh](../scripts/build-quick-ui.sh) | 16 | 0 |
| A | [scripts/run-qml-preview.sh](../scripts/run-qml-preview.sh) | 21 | 0 |
| A | [scripts/run-quick-ui.sh](../scripts/run-quick-ui.sh) | 8 | 0 |
| A | [scripts/test-quick-ui.sh](../scripts/test-quick-ui.sh) | 17 | 0 |
| A | [scripts/update-release-inventory.py](../scripts/update-release-inventory.py) | 98 | 0 |

### Testy

| Status | Plik | + | − |
| --- | --- | ---: | ---: |
| A | [tests/calendar-dates-regressions.h](../tests/calendar-dates-regressions.h) | 91 | 0 |
| A | [tests/calendar-inspector-space-regressions.h](../tests/calendar-inspector-space-regressions.h) | 31 | 0 |
| A | [tests/calendar-workspace-ux-regressions.h](../tests/calendar-workspace-ux-regressions.h) | 35 | 0 |
| A | [tests/classification-regressions.h](../tests/classification-regressions.h) | 159 | 0 |
| A | [tests/competition-advancement-regressions.h](../tests/competition-advancement-regressions.h) | 82 | 0 |
| A | [tests/competition-quick-smoke.cpp](../tests/competition-quick-smoke.cpp) | 397 | 0 |
| A | [tests/competition-ties-gate-regressions.h](../tests/competition-ties-gate-regressions.h) | 126 | 0 |
| A | [tests/engine-audit-regressions.h](../tests/engine-audit-regressions.h) | 70 | 0 |
| A | [tests/navigation-performance.h](../tests/navigation-performance.h) | 26 | 0 |
| A | [tests/quick-ui-smoke.cpp](../tests/quick-ui-smoke.cpp) | 2288 | 0 |
| A | [tests/roster-drag-regressions.h](../tests/roster-drag-regressions.h) | 23 | 0 |
| A | [tests/roster-translation-regressions.h](../tests/roster-translation-regressions.h) | 13 | 0 |
| A | [tests/simulation-onboarding-regressions.h](../tests/simulation-onboarding-regressions.h) | 80 | 0 |
| A | [tests/ui-duplication-regressions.h](../tests/ui-duplication-regressions.h) | 79 | 0 |
| A | [tests/ux-roster-input-regressions.h](../tests/ux-roster-input-regressions.h) | 26 | 0 |

### Tłumaczenia

| Status | Plik | + | − |
| --- | --- | ---: | ---: |
| M | [translations/translation_en.ts](../translations/translation_en.ts) | 4356 | 108 |

### Wcześniejszy opcjonalny eksperyment konkursowy

| Status | Plik | + | − |
| --- | --- | ---: | ---: |
| A | [source/UI/Competition/Quick/ActionButton.qml](../source/UI/Competition/Quick/ActionButton.qml) | 28 | 0 |
| A | [source/UI/Competition/Quick/CompetitionConfig.qml](../source/UI/Competition/Quick/CompetitionConfig.qml) | 219 | 0 |
| A | [source/UI/Competition/Quick/CompetitionLive.qml](../source/UI/Competition/Quick/CompetitionLive.qml) | 77 | 0 |
| A | [source/UI/Competition/Quick/CompetitionLiveBridge.cpp](../source/UI/Competition/Quick/CompetitionLiveBridge.cpp) | 276 | 0 |
| A | [source/UI/Competition/Quick/CompetitionLiveBridge.h](../source/UI/Competition/Quick/CompetitionLiveBridge.h) | 54 | 0 |
| A | [source/UI/Competition/Quick/CompetitionQuickBridge.cpp](../source/UI/Competition/Quick/CompetitionQuickBridge.cpp) | 296 | 0 |
| A | [source/UI/Competition/Quick/CompetitionQuickBridge.h](../source/UI/Competition/Quick/CompetitionQuickBridge.h) | 59 | 0 |
| A | [source/UI/Competition/Quick/Field.qml](../source/UI/Competition/Quick/Field.qml) | 158 | 0 |
| A | [source/UI/Competition/Quick/LiveArena.qml](../source/UI/Competition/Quick/LiveArena.qml) | 170 | 0 |
| A | [source/UI/Competition/Quick/LiveButton.qml](../source/UI/Competition/Quick/LiveButton.qml) | 27 | 0 |
| A | [source/UI/Competition/Quick/LiveChoice.qml](../source/UI/Competition/Quick/LiveChoice.qml) | 25 | 0 |
| A | [source/UI/Competition/Quick/LiveDashboard.qml](../source/UI/Competition/Quick/LiveDashboard.qml) | 70 | 0 |
| A | [source/UI/Competition/Quick/LiveJumpPreview.qml](../source/UI/Competition/Quick/LiveJumpPreview.qml) | 81 | 0 |
| A | [source/UI/Competition/Quick/LiveNode.qml](../source/UI/Competition/Quick/LiveNode.qml) | 84 | 0 |
| A | [source/UI/Competition/Quick/LiveTable.qml](../source/UI/Competition/Quick/LiveTable.qml) | 109 | 0 |
| A | [source/UI/Competition/Quick/competition-quick.qrc](../source/UI/Competition/Quick/competition-quick.qrc) | 14 | 0 |
| A | [source/UI/Competition/Quick/fields.json](../source/UI/Competition/Quick/fields.json) | 190 | 0 |
