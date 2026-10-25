# Archiwum, historia wyników i DNS

## 1. Synchronizacja archiwum

Modele archiwum zgłaszają pełny reset przy zmianie zawartości, również gdy lista nadal znajduje się pod tym samym adresem. Po rozegraniu konkursu odświeżane są modele i otwarty widok klasyfikacji, a nie sam widget listy. Nie trzeba ponownie uruchamiać gry.

Lista rozegranych zawodów nie kończy się już na pierwszym nierozegranym wydarzeniu. Jawne mapowanie wiersza na rozegrany konkurs zapobiega otwieraniu niewłaściwego wydarzenia, gdy w kalendarzu są luki. Dodano sprawdzanie zakresu indeksów i pustych źródeł modeli.

Lista kalendarzy archiwum jest synchronizowana po utworzeniu widoku i zmianach kalendarza; zachowuje wybór po trwałym identyfikatorze, aktualizuje nazwy i bezpiecznie wybiera dostępny kalendarz po usunięciu dotychczasowego. Dla poprzednich sezonów nie wykorzystuje indeksu aktywnego kalendarza z bieżącego sezonu. Wskaźniki modeli i panelu klasyfikacji są inicjalizowane przed sygnałami konfiguracji.

## 2. Wyniki po wybranej serii

W konkursie pojawia się niewielki wybór **Teraz / Po N serii**. Dostępne są serie już zakończone; „Teraz” obejmuje bieżący przebieg. Dla małych ekranów wybór jest na górze, na szerszych w nagłówku wyników. Podgląd wcześniejszej serii:

- blokuje ręczne skakanie, DNS, automatyczną symulację, przechodzenie dalej i działania sterujące;
- ukrywa bieżącego zawodnika, aktualną listę startową, status rozgrywania i przyciski konkursu, aby nie mieszać ich z historycznymi danymi;
- pokazuje „Podgląd”; wybór „Teraz” odtwarza poprzednie stany dostępności kontrolek;
- wykorzystuje oddzielne kopie wyników, obcięte do wybranej serii, z jej wagami/resetami i przeliczoną kolejnością oraz miejscami ex aequo.

Ten sam mechanizm działa w konkursach indywidualnych, KO i drużynowych. W podglądzie nie pokazuje się bieżąca tabela grupy KO jako rzekomo historyczna. Wyniki drużyn zachowują hierarchię drużyna–zawodnicy i punkty serii.

Archiwum konkursu używa wspólnego widoku z wyborem **Wszystkie serie / Po N serii** oraz kartą wybranego skoku. Zachowano dostęp do webhooka. Można wybrać wcześniejszy skok w karcie, a Shift+klik w kolumnę serii otwiera tę próbę również w tabeli archiwalnej.

## 3. Klasyfikacja sezonowa po konkursie

Panel archiwum klasyfikacji ma wybór **Wszystkie konkursy / Po X. konkursie**. Pozycje listy opisują również skocznię. Podgląd uwzględnia kolejne rozegrane wydarzenia wybranego kalendarza przypisane do danej klasyfikacji.

Punktacja pochodzi z istniejącego `Classification::contributions()`, więc zachowuje metody punktowania, filtry wieku, parametry wydarzeń i dobór zawodników do punktacji krajowej. Nie modyfikuje zapisanych wyników klasyfikacji. Późniejsze konkursy nie wpływają na wcześniejszy stan. Historyczne sumy zachowują ułamkową precyzję punktacji. Wysyłanie webhooka i szczegóły odnoszące się do aktualnego rankingu nie są uruchamiane z historycznych wierszy.

## 4. Ranking indywidualny drużynówki

Wybór **Drużyny / Indywidualnie** jest dostępny podczas konkursu, po jego zakończeniu i w archiwum. Ranking indywidualny sumuje **surowe punkty oddanych skoków zawodnika**, bez wag i resetów stosowanych do klasyfikacji drużynowej. Zachowuje miejsca ex aequo. Można go ograniczyć do wybranej serii; początkowo obejmuje wszystkie dotychczasowe skoki.

W widoku „Teraz” ranking aktualizuje się po skokach i pozwala kontynuować konkurs. Podgląd wcześniejszej serii jest wstrzymany tak jak wyniki drużyn. Karta skoku i oznaczenie najnowszego wyniku pozostają przy ostatnio skaczącym zawodniku. Dane nie są ujawniane przed zakończeniem prezentacji skoku na żywo.

## 5. Pominięcie próby

W **Skok** dostępne jest **Pomiń skok · DNS**. Tworzy jedną próbę z DNS i 0 pkt bez wykonywania symulacji, bez DSQ i bez animacji lotu. Przechodzi przez istniejącą obsługę zapisu próby i przejścia do kolejnego zawodnika.

DNS dotyczy wyłącznie bieżącej próby. Nie dyskwalifikuje drużyny, nie usuwa jej pozostałych zawodników i nie tworzy trwałego zakazu startu w kolejnych seriach. Oznaczenie DNS jest widoczne w tabeli i karcie skoku; karta stosuje neutralny kolor. Próba DNS nie uruchamia sprawdzania rekordów, również przy rekordzie skoczni równym 0.

## Bezpieczeństwo danych i dodatkowy błąd

Podglądy nie zapisują kopii do sezonu ani nie zmieniają logiki symulacji. Kopie skoków zachowują identyfikatory i mają powiązania ze swoimi wynikami w kopii. Widoki i wybory używają modeli tylko do odczytu.

AddressSanitizer ujawnił istniejące założenie w funkcjach pozycji skoku: szukały skoku po adresie obiektu. Kopia nie była znajdowana; w drużynówce powstawał indeks serii -1 i odczyt poza tablicą. Wyszukiwanie oparto na trwałym ID, dodano sprawdzanie zakresu i bezpieczną obsługę brakujących skoków. Drużyna jest rozpoznawana po członkostwie zawodnika, nie wyłącznie po kraju.

## Testy

- `--archive-history-only`: sygnały modeli, rozegrane wydarzenia po luce kalendarza, ranking klasyfikacji po pierwszym i drugim konkursie, niezmienność źródłowych wyników, skopiowany i nieznany skok.
- `--history-dns-only`: rzeczywiste konkursy indywidualny, KO i drużynowy; blokada mutacji w podglądzie, powrót do sterowania, DNS, dalsze skoki drużyny, surowy ranking, widok archiwalny, wybór serii i Shift+klik. Rendery obu motywów, małych ekranów i dużego tekstu.
- `--workflow-only`: rzeczywista nawigacja i edycja sezonu po zmianie synchronizacji archiwum.
- Pełny zestaw regresji Qt Quick oraz kompilacja alternatywy Widgets.
- Kontrola pamięci AddressSanitizer: scenariusz historii/DNS po naprawie odczytu poza tablicą.

Wyniki końcowe: scenariusze historii/DNS dla wszystkich trzech typów zawodów — PASS; przepływ sezonu — PASS; pełny zestaw Qt Quick — PASS, **399 responsywnych renderów**, bez ostrzeżeń rekurencyjnych layoutów ani pętli bindingów. Kompilacja Widgets — kod 0. AddressSanitizer uruchomiony pod GDB (`detect_leaks=0`) po poprawce zakończył scenariusze historii/DNS normalnie, bez zgłoszeń odczytu poza zakresem; nie jest to kontrola wycieków Qt.

## Krótki plan ręcznego sprawdzenia

1. Rozegraj konkurs sezonu i od razu otwórz archiwum. Sprawdź nowy wiersz i aktualne punkty klasyfikacji.
2. W drugiej serii wybierz „Po 1 serii”. Sprawdź kolejność i sumy; próba rozpoczęcia symulacji powinna być niedostępna. Wróć do „Teraz” i kontynuuj.
3. W archiwum tego konkursu przełącz serie i kliknij wynik. Sprawdź wcześniejsze skoki w karcie i Shift+klik w kolumnę serii.
4. W drużynówce przełącz „Indywidualnie”, także przy wagach lub resecie. Suma ma pozostać sumą surowych punktów zawodnika. Sprawdź ten widok również po zakończeniu i w archiwum.
5. W archiwum klasyfikacji wybierz pierwszy, potem kolejny konkurs; późniejsze punkty mają pojawić się dopiero przy późniejszym stanie.
6. W „Skok” pomiń próbę. Powinno być DNS i 0 pkt; drużyna nie może otrzymać DSQ, a pozostali członkowie muszą kontynuować.
