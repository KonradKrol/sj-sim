# Sj.Sim 2.0.0 — changelog i notatki do wydania

Dokument do redakcji release notes: osobny opis dla graczy, szczegółowy wykaz funkcjonalny, informacje o wydaniu oraz kompletny indeks commitów i zmienionych plików. Zestawienie dotyczy kodu w repozytorium, nie prywatnej biblioteki autora.

## Zakres

- Porównanie: **1.3.0-beta2 z main → 2.0.0**, branch `feature/simulation-v2`.
- Data opracowania: **16 listopada 2026**.
- Baza: `05f2e48d8e92c7b9b180f18abe250b3c7e61d24c`.
- Koniec przeglądu kodu: `643d8b5af0680e8d5191d0cd22ea902ef3e1e493`.
- Liczba commitów: **698**. Manifest obejmuje całą różnicę, również wcześniejsze implementacje później zastąpione lub ukryte.
- Dokument opisuje stan końcowy. Historia w załączniku zachowuje także cofnięte pomysły i kolejne kalibracje; nie są one osobnymi aktywnymi funkcjami.
- Starszy [wykaz z 26 października](docs/RELEASE_NOTES_VS_MAIN.md) pozostaje materiałem historycznym. W razie rozbieżności obowiązuje obecny kod, następnie ten dokument.

# I. Informacje dla graczy

## Nowy interfejs całej gry

Sj.Sim otrzymał nowy interfejs Qt Quick: menu, przygotowanie zawodów, ekran konkursu, sezony, kalendarze, klasyfikacje, archiwum, bibliotekę oraz edytory zasobów. Układ jest przeznaczony dla komputerów i mniejszych laptopów. Główne zasady projektu to hierarchia, kontrast, klarowność i ograniczenie zbędnych elementów.

- Wybór jasnego lub ciemnego wyglądu oraz trzech motywów: dotychczasowego, różowego i prostego.
- Regulowana wielkość interfejsu i tekstu, bardziej kompaktowe tabele i panele, ograniczanie animacji.
- Przebudowana nawigacja, mniej wielopoziomowych dialogów, bardziej czytelne komunikaty i stany pustych ekranów.
- Nowe logo, sekcja „O autorze”, odnośniki do społeczności i czteroczęściowe powitanie ze zdjęciami.
- Przykładowy sezon uruchamiany bez ręcznego kompletowania zasobów: lato i zima, gotowe kalendarze, daty, automatyczna forma i automatyczny zapis.
- Nazwa „Pojedynczy konkurs” została zastąpiona przez „Szybki konkurs”.

## Nowi skoczkowie, skocznie i silnik

- Skoczka opisują przede wszystkim OVR, forma, klasa lądowania i opcjonalne perki. Są też dane osobowe, rok urodzenia, rekord życiowy oraz zdjęcie.
- Perki tworzą przeciwstawne pary. Mają cztery poziomy; nie można jednocześnie wybrać obu przeciwieństw w tej samej parze.
- Edytor korzysta z kompaktowych naszywek, wyraźnego zaznaczenia poziomu i podglądu zdjęcia ze względnej ścieżki w paczce.
- Skocznie mają nowe atrybuty i perki, m.in. lotna/nielotna, kompaktowa/rozległa oraz charakterystykę automatycznie losowanego wiatru.
- Punkty za punkt K i za metr są wyliczane automatycznie według zasad stosowanych w grze.
- Przebudowano obliczanie jakości skoku, wpływu specjalizacji, wiatru, geometrii zeskoku, lądowania i not sędziowskich.
- Wysoka forma nadal pomaga, lecz jej dodatni wpływ jest stopniowo ograniczany u zawodników o bardzo wysokim OVR. Słaba forma zachowuje pełny ujemny wpływ.
- W ustawieniach dostępne są uproszczone opcje kalibracji, m.in. losowości skoków, różnic w odległościach i zmienności formy.

## Wiatr

- Warunki zmieniają się w czasie i wzdłuż zeskoku; podmuch w różnych miejscach może inaczej wpłynąć na skok.
- Przed konkursem gra proponuje warunki na podstawie skoczni. Można losować ponownie, wybrać scenariusz lub dostosować kierunek, siłę i zmienność.
- Kierunek można ustawić płynnie, a warunki na zeskoku modyfikować lokalnie na podglądzie.
- Siła, zmienność w czasie i różnice na zeskoku są rozdzielone. Silny wiatr nie musi oznaczać loterii.
- Na ekranie zawodów dostępna jest prognoza wiatru i jej kompaktowy, przesuwany podgląd.
- Pomiar po skoku uwzględnia upływ czasu i warunki w trakcie skoku. Może różnić się od prognozy.
- Obecnie używany pomiar do rekompensaty odrzuca czujniki za miejscem lądowania. Nadal jest to uproszczony pomiar, nie gwarancja idealnego wyrównania wpływu wiatru.
- Opady są obecnie tymczasowo wyłączone w normalnym interfejsie; wcześniejsze prace nad nimi nie oznaczają aktywnej funkcji wydania.

## Wygodniejsze rozgrywanie zawodów

- Przebudowana karta skoku: miejsce, kraj, odległość, punkty, osobne noty sędziów, lądowanie i składniki wyniku.
- Możliwość podglądu skoków z poprzednich serii; Shift+klik w wynik pozwala wskazać konkretną serię.
- Zbędne informacje są ukrywane, jeśli odpowiednie elementy punktacji wyłączono w zasadach.
- DNS oznacza pominięty skok; DSQ jest pokazywane jawnie. Dyskwalifikacja w drużynówce obejmuje drużynę.
- Automatyczna belka może działać przed kolejnymi skokami. Dostępna jest także ręczna sugestia i obniżenie na życzenie trenera.
- Narzędzia konkursu pozwalają symulować skoki, grupę, serię lub konkurs, czekać 30 sekund, zmieniać warunki i manipulować pojedynczym skokiem.
- Osobne tryby stopniowego ujawniania skoku i automatycznego rozgrywania. Noty pojawiają się na zarezerwowanych miejscach w losowej kolejności.
- Linie do prowadzenia i awansu uwzględniają kolejne zmiany belki, w tym obniżenie przez trenera. Nierealistyczne wartości są ukrywane.
- Wyraźniejsze oznaczenia awansu i odpadnięcia, Q oraz LL w odpowiednim kontekście serii KO.
- Wyniki drużynowe mają zapamiętywane tryby rozwinięcia i dodatkową klasyfikację indywidualną.
- Podgląd wyników po wskazanej serii, także w archiwum. Podgląd wstrzymuje rozgrywanie i pozwala wrócić do aktualnego konkursu.
- W wynikach i klasyfikacjach dostępne są subtelne, wyrównane kolumny strat punktowych.

## Sezony i kalendarze

- Nowy przegląd sezonu z najbliższymi wydarzeniami, zgrupowanymi treningami i seriami próbnymi oraz podium ostatniego konkursu.
- Edycja kalendarza jest oddzielona od wyboru kalendarza aktywnego.
- Przejście do następnego sezonu sprowadza się do jednego potwierdzenia: domyślnie powtarza kalendarze i klasyfikacje bez wyników, przesuwa daty o rok i zachowuje zawodników z formą oraz zasoby. Można zamiast tego rozpocząć bez kalendarza. Archiwum poprzedniego sezonu pozostaje nienaruszone.
- Wielokrotne Cofnij/Ponów, przeciąganie konkursów i bloków wydarzeń, operacje na wielu zaznaczonych pozycjach.
- Szybkie planowanie dat i całych weekendów z kwalifikacjami, treningami i seriami próbnymi.
- Kontrola chronologii także pomiędzy kalendarzami przy włączonych datach. Wydarzenia tego samego dnia można rozgrywać w wybranej kolejności.
- Powiązania kwalifikacji nie giną po zmianie kolejności. Nieprawidłowe zależności blokują rozegranie do czasu ich poprawienia.
- Wybór źródła sortowania listy startowej: dopasowana klasyfikacja, źródło kwalifikacji, wskazany konkurs lub klasyfikacja.
- Serie próbne mogą dziedziczyć kolejność konkursu głównego, również z układu KO.
- Jawne odwołanie konkursu jest oddzielone od zwykłego wyjścia. Odwołanie kwalifikacji nie eliminuje ich uczestników z konkursu głównego.
- Przebudowana edycja klasyfikacji, różne sposoby punktowania, czytelniejsze filtry oraz podgląd wkładu zawodników w wynik kraju.
- Statystyki zawodnika z wykresem miejsc, filtrami rodzaju wydarzeń i szczegółami pod kursorem.

## Automatyczna forma

- Opcjonalny system formy działający wraz z datami sezonu; czas przesuwa się także przy odwołaniu wydarzeń.
- Siedem stanów formy, przejścia między nimi oraz niezależne wahania zawodników.
- Perki regularności formy zmieniają wahania, czas trwania i prawdopodobieństwa stanów.
- Czytelne opisy od „Głębokiego kryzysu” do „Absolutnego szczytu”.
- Przy aktywnym systemie ręczna zmiana formy jest zablokowana. Przełącznik „Pokaż formę” pozwala ukryć jej wartość; bez autoformy jest zawsze widoczna. Diagnostyka jest domyślnie ukryta i dostępna przez Ctrl+Alt+[.
- Forma dnia może dodatkowo zmienić dyspozycję w danym dniu; w szybkim konkursie można ją wyłączyć.

## Listy startowe i drużyny

- Nowy wybór uczestników z list, widok krajów i wygodniejsze przenoszenie zawodników.
- Narzędzia kopiowania i sortowania według wyników, klasyfikacji, OVR, aktualnego potencjału, alfabetu i krajów; odwracanie oraz losowanie kolejności.
- Zawodnicy bez wyniku źródłowego i remisy są losowani wewnątrz odpowiedniej części listy.
- W drużynówce można określić dostępnych zawodników, aby automatyczny dobór nie wybierał np. odpoczywających liderów.
- Ręczna zmiana kolejności w składzie przez przeciąganie zamienia zawodników miejscami.
- Kraje bez liczby dostępnych skoczków wymaganej do skompletowania drużyny nie są proponowane.
- Edytor KO z wyszukiwaniem, przeciąganiem, ręcznym doborem i dodatkowymi sposobami tworzenia grup, w tym bez rodaków i z wyrównanym potencjałem.

## Biblioteka, udostępnianie i instalacja

- Jedno przewidywalne miejsce na dane: domyślnie `Dokumenty/SjSimData`, z możliwością zmiany folderu.
- Paczki skoczków, skoczni, zasad, punktacji, kalendarze i zapisy mają wydzielone katalogi.
- Import i eksport plików `.sjsim-pack`, także wielu zasobów razem i zdjęć należących do paczki.
- Możliwość usuwania zasobów z biblioteki z potwierdzeniem i mechanizmem odzyskania przez kosz systemowy, gdy jest dostępny.
- Nowa instalacja otrzymuje zasoby startowe; aktualizacja nie zastępuje własnej biblioteki gracza.
- Windows: instalator z bibliotekami i skrótami. Linux: jeden plik AppImage.
- Rozbudowane poprawki stabilności i wydajności zapisu, wczytywania, konkursów, edytorów i archiwum.

# II. Szczegółowy wykaz zmian i uwagi techniczne

## 1. Architektura i Qt Quick

- Qt Quick jest domyślnym interfejsem. Kod Qt Widgets nie został w całości usunięty; część nadal pełni funkcje zaplecza lub alternatywnego interfejsu.
- Wspólne komponenty motywów, typografii, przycisków, pól liczbowych, tabel, pomocy i dialogów zastępują doraźne układy poszczególnych ekranów.
- Naprawiono m.in. zapętlone przeliczanie układów, ograniczanie wysokości paneli, obcinanie przycisków dialogów, niewidoczne elementy list rozwijanych i rozsynchronizowanie wyboru w edytorach.
- Poprawiono zachowanie układu po zmianie skali interfejsu, przy pierwszym wejściu, przełączaniu zakładek i zmianie rozmiaru okna. Usunięto także pętlę wyliczania wysokości panelu ustawień symulacji.
- Wprowadzono animacje nawigacji i stanów, z obsługą ograniczonego ruchu; nie należy utożsamiać liczby historycznych próśb o animacje z liczbą niezależnych efektów w kodzie.
- Ujednolicono format dat `DD.MM.YYYY`, pokazywanie odległości z jednym miejscem po przecinku i formatowanie strat. Klasyfikacje z wyłącznie całkowitymi punktami nie muszą wyświetlać końcówki „,0”.
- Usunięto z widocznego UI integracje webhooków i zbędne ustawienia. Nie oznacza to usunięcia każdego historycznego pliku lub zewnętrznej zależności z repozytorium.

## 2. Model skoczka i skoczni

- Profil skoczka: OVR i forma 1–100, pięć klas lądowania, opcjonalny rok urodzenia i rekord, względna ścieżka zdjęcia, identyfikatory źródła i własnego profilu.
- Pary perków: podejście do treningu, specjalizacja skoczni, regularność skoków oraz stabilność formy. Brak perka jest dozwolony.
- Cztery poziomy perków. Migracja dawnych trzech poziomów zachowuje znaczenie: 1→1, 2→2, 3→4. To migracja danych profilu, nie gwarancja zgodności starych sezonów.
- Dodatni efekt formy ogranicza mnożnik `1 - 0.35 * clamp((OVR-75)/25, 0, 1)`. Dotyczy tylko formy powyżej neutralnej 50; istniejący współczynnik wpływu formy pochodzi z silnika. Efektywna siła może przekraczać 100.
- Perki treningowe korzystają z rozkładów losowych zamiast wyłącznie stałych przesunięć. „Błyszczy na treningach” daje premię na treningach/próbnych i część premii w kwalifikacjach, bez dawnego stałego minusa w konkursie.
- Skocznia przechowuje K, HS i geometrię/real HS używaną przez silnik. Punktacja K/metr jest wyprowadzana automatycznie.
- Perki skoczni obejmują charakter lotny, ryzyko lądowania, charakterystykę losowanych warunków, preferowany kierunek i rozpiętość odległości.
- Końcowy zakres perka kompaktowa/rozległa wynosi obecnie maksymalnie ±39% względem odpowiedniej odchyłki od K. To nie mnożnik całej odległości skoku.

## 3. Silnik skoków, lądowanie i noty

- Nowy silnik rozdziela jakość, specjalizację, warunki i geometrię. Parametry kalibracyjne są wspólne dla gry i narzędzi porównawczych, z jawnym rozróżnieniem wariantu testowego.
- Zależność wybicie/lot jest interpolowana według K, z punktami odniesienia m.in. 50, 80, 95, 110, 125, 140, 200 i 250. Zachowane są skocznie pośrednie, zamiast sztywnego podziału na normalne/duże/mamucie.
- Lot uwzględnia tylko przebytą część zeskoku i lokalne warunki. Jest modelem przybliżonym do symulacji gry, nie pełnym solverem aerodynamiki.
- Przebudowano wypłaszczenie zeskoku, osiągalne odległości i ryzyko dalekiego lądowania. Auto-belka korzysta z real HS.
- Rozdzielono losowość jakości i skalę różnic odległości; zmieniono wpływ K, mamutów, specjalizacji oraz kompaktowości.
- Noty zależą od klasy lądowania, odległości, rodzaju lądowania i zmienności między sędziami. Zmieniono rozkłady kar za dwie nogi, podpórkę i upadek oraz rzadkie odstające noty.
- Utrudniono otrzymanie 20 i ograniczono wzrost premii odległościowej; zmniejszono różnice między najwyższymi klasami stylu.
- Bazową szansę podpórki obniżono do zera; nie jest to stwierdzenie, że wszystkie zależne od sytuacji mechanizmy ryzyka zostały usunięte.
- Obliczenia linii na podstawie historii not nie uwzględniają podpórek i upadków w średniej stosowanej do estymacji.
- Kolejne zmiany liczbowe są zachowane w historii commitów. Za aktualną kalibrację należy przyjmować wartości kodu i dostarczony profil, nie dowolny wcześniejszy wpis changelogu.

## 4. Pogoda i pomiar wiatru

- Pole wiatru ewoluuje zarówno przestrzennie, jak i w czasie. Parametry obejmują osobno siłę/kierunek, zmienność czasową, przestrzenną i charakter podmuchów/tendencji.
- Uproszczony interfejs pokazuje skalę zmienności 0–10. Wewnętrzna skala i współczynniki nie są tożsame z jednostkami m/s.
- Poprawiono sprzęganie pól: zmiana kierunku nie powinna zmieniać siły, a zmienność przestrzenna nie powinna zmieniać czasowej.
- Ustawienia ręczne i automatyczna propozycja skoczni mają odrębny zakres działania. Automatyczne warunki złagodzono o 30%, a częstotliwość bardzo skrajnych loterii dodatkowo ograniczono o 85%; nie jest to globalna redukcja wszystkich ręcznych presetów.
- Osłabiono wpływ perków skoczni na losowanie i poprawiono krzywą zmienności czasowej, aby wysokie ustawienia nie rosły nadmiernie agresywnie.
- Wcześniej zaimplementowano wiele metod pomiaru, ale wybór usunięto z UI. Bieżąca ścieżka korzysta z wariantu „Ulepszony”: pomiar przybliżony, pomijający czujniki poza osiągniętą odległością.
- Nie należy opisywać tego jako wiernego odtworzenia tajnych wag lub pełnego oficjalnego algorytmu FIS. Rekompensata nadal używa normalnych przeliczników gry.
- Korekta pomiaru po skoku i symulacja upływu czasu rozdzielają prognozę od wyniku pomiaru. Pomiędzy seriami przewidziano 15 minut dla ewolucji pola wiatru.
- Prace nad deszczem i śniegiem znajdują się w historii, lecz opady są aktualnie wyłączone i zerowane w normalnym przebiegu.

## 5. Wyniki, KO i drużyny

- Poprawiono remisy, awanse na granicy limitu, statusy pewnego awansu/odpadnięcia i odróżnienie kwalifikacji do konkursu od awansu do następnej serii.
- LL dotyczy aktualnej serii KO, również warunkowego miejsca wśród lucky loserów; pewność awansu jest oznaczana niezależnie.
- Rozbudowano metody generowania grup: losowanie koszykami bez rodaków, układy przeplatane i wyrównywanie potencjału grup.
- Umożliwiono ręczną edycję grup przed konkursem oraz przy przechodzeniu do kolejnej serii, z wyszukiwaniem i przeciąganiem.
- W wynikach przed skokiem poprawiono punktowe porównania przy wagach, resecie i różnych liczbach oddanych skoków. KO korzysta z lidera grupy, gdy jest dostępny; drużyny uwzględniają także postęp grup w pierwszej serii.
- DNS nie jest DSQ i nie wyklucza automatycznie kolejnego skoku, jeśli zawodnik lub drużyna mimo tego awansuje.
- DSQ w drużynie blokuje dalsze skoki zdyskwalifikowanej drużyny i jest komunikowane.
- Podium dotyczy końcowej klasyfikacji konkursu, także zakończonego odwołaniem kolejnej serii; uwzględnia ex aequo.
- Wyniki archiwalne i podgląd po serii obsługują indywidualne, KO i drużynowe zawody; indywidualna klasyfikacja drużynówki sumuje surowe punkty skoków.
- Dodano rozkład wkładu zawodników w punkty kraju i stan klasyfikacji po wybranym konkursie.

## 6. Sezon, kalendarze i klasyfikacje

- Zachowanie powiązań przez stabilne identyfikatory zamiast zależności wyłącznie od indeksu po przesunięciu konkursu.
- Zablokowanie rozgrywania przy nierozwiązanej chronologii i źródle kwalifikacji ustawionym za konkursem zależnym. Interfejs rozróżnia niedozwoloną akcję od zapisania błędnego kalendarza.
- Jawny wybór kalendarza aktywnego niezależny od edycji, widoczna lista kalendarzy, tworzenie kalendarza bezpośrednio z pustego przeglądu.
- Daty obejmują kalendarze całego sezonu, a nie tylko aktualny widok. Aktualna data przesuwa się do kolejnego wydarzenia.
- Planowanie pojedynczych odstępów i weekendów piątek–sobota–niedziela, wariantów kwalifikacji, treningów oraz próbnych; podgląd aktualizowany bez osobnego kroku.
- Po zmianie typu wydarzenia na kwalifikacje, trening lub próbną wybierane są odpowiednie domyślne zasady.
- Klasyfikacje: klasyczne punkty za miejsca lub nota z konkursu, różne sposoby, punktacja między typami zawodów, mnożnik i filtry zaawansowane.
- Istniejące sposoby punktacji można podglądać po rozegraniu zawodów, mimo blokady edycji chroniącej wyniki.
- Wiek do filtra punktowania bazuje na roku początku sezonu. Filtr nie blokuje startu w samych zawodach.
- W klasyfikacji drużynowej wkład TOP/LAST N kraju jest wybierany dla danego indywidualnego konkursu. Zdobyte wówczas punkty nie znikają po zmianie późniejszej kolejności zawodników.
- Przebudowano paski narzędzi, hierarchię edytorów, flagi, wyszukiwanie skoczni, etykiety konkursów z HS, dniem tygodnia i oznaczeniami Q/TEAM.

## 7. Automatyczna forma i forma dnia

- Moduł [form-engine](source/form-engine/FormEngine.h) oddziela stan zawodnika, konfigurację i losowanie. Każdy zawodnik ma własny zapisany stan losowości.
- Siedem reżimów zamiast pięciu. Robocze środki: 20; 31,33; 41,33; 50; 63,33; 76,33; 89. Neutralne udziały czasu: 2%, 10%, 23%, 30%, 23%, 10%, 2%.
- Obecna średnia bazowa długości reżimu: 104 dni. Rzeczywisty czas jest losowy i zależy także od konfiguracji oraz perka.
- Regularność formy ogranicza amplitudę o 10/25/40/55% na poziomach I–IV; chwiejność zwiększa ją analogicznie. Oddzielne współczynniki zmieniają skupienie na stanach i tempo przejść.
- System działa autonomicznie wyłącznie przy aktywnych datach. Cofnięcie przygotowania konkursu obsługuje przywracanie właściwego stanu; odwołanie wydarzenia nie zatrzymuje czasu.
- Nowa konfiguracja domyślna służy nowym zapisom. Istniejący sezon ma utrwalony własny zestaw parametrów, zamiast niejawnie zmieniać kalibrację po aktualizacji.
- Forma dnia korzysta z rozkładu Laplace’a ze skalą roboczą 2,5 i jest wspólna dla występów danego dnia; szybki konkurs traktowany jest jako jeden dzień.
- Form Engine Lab udostępnia dobór zawodników, startową formę i perki do eksperymentu, wykresy do dziesięciu zawodników, przebiegi i batch. Nie zmienia przy tym źródłowych profili paczki.
- Skrót deweloperski Ctrl+Shift+F pozwala przesunąć formę o wskazaną liczbę dni.

## 8. Balance Lab i narzędzia kalibracji

- Laboratorium jest narzędziem eksperymentalnym dostępnym z ustawień, a nie głównym trybem w menu startowym.
- Serie symulacji, rozkłady odległości i miejsc, widok pojedynczego skoczka, porównania wariantów oraz parametrów zawodnika, skoczni, warunków i silnika.
- Powtarzalne ziarna i rozróżnienie bazowych parametrów gry od kandydackiego wariantu eksperymentu.
- Obliczenia w tle z wydzieloną losowością, postępem, pozostałą pracą i możliwością anulowania; możliwość obsługi eksperymentów z narzędzi programistycznych.
- Wyszukiwanie parametrów, poprawione opisy i przykłady. Wartości robocze pozostają kalibracją gry, nie deklaracją fizycznej dokładności.
- Podgląd siedmiu konkursów liczony dopiero po włączeniu, bez kosztu ukrytego podglądu.
- Dokumentacja: [SIMULATION_PLAYGROUND.md](docs/SIMULATION_PLAYGROUND.md), [NEW_SIMULATION.md](docs/NEW_SIMULATION.md), [FORM_ENGINE_WORK.md](docs/FORM_ENGINE_WORK.md). Część tych dokumentów zachowuje wcześniejsze etapy projektu; przy liczbach należy sprawdzać aktualny kod.

## 9. Biblioteka i formaty

- Domyślna biblioteka: systemowy folder Dokumenty, podkatalog `SjSimData`. Konkretną ścieżkę wskazuje UI; można wybrać inną lokalizację lub tryb przenośny.
- Zasoby są oddzielone od kodu i ustawień aplikacji. Dawny `GlobalDatabase` nie jest docelowym miejscem nowych paczek użytkownika.
- Eksport `.sjsim-pack` zawiera nagłówek formatu, wersjonowany dokument, zasoby i kontrolowane pliki zdjęć. Obsługuje kilka zasobów w jednej paczce; to nie jest zwykłe archiwum ZIP.
- Import sprawdza strukturę, identyfikatory, odwołania, rozmiary, sumy zdjęć i bezpieczne ścieżki. Zmiany są przygotowywane etapowo, z obsługą błędów zamiast częściowego nadpisania.
- Zdjęcia należą do paczki i są przenoszone razem z nią; nie wymagają ścieżek do prywatnego katalogu autora.
- Eksport kalendarza i jego zależności różni się od zapisu rozegranego sezonu; do udostępniania należy używać eksportu biblioteki, zamiast wysyłać przypadkowe pliki robocze.
- Poprawiono usuwanie otwartych zasobów, cofanie niezapisanych zmian w DB, zachowanie innych paczek i zapobieganie przywracaniu usuniętych zasobów przez stary stan edytora.
- Aktualnie dostarczane Defaults: jedna paczka 83 skoczków ze zmienionymi nazwiskami, 128 skoczni, paczka zasad i punktacje; sześć kalendarzy: LGP 2023/2024/2026 oraz PŚ 2023/24, 2024/25, 2026/27.
- Prywatna biblioteka autora może mieć inne nazwiska i liczebność. Dzisiejsze dopisanie 20 skoczków do lokalnej paczki 83→103 nie zmienia automatycznie zasobów startowych w repozytorium ani już zbudowanego instalatora.

## 10. Stabilność i wydajność

- Naprawy crashy przy zakończeniu szybkiego konkursu, sortowaniu względem pustych klasyfikacji, rozwijaniu wyboru źródła, tworzeniu grup KO, zmianie domyślnych zasad i przełączaniu widoków.
- Naprawy reentrancy podczas zapisu pól edytora, m.in. PB zatwierdzanego Enterem, oraz resetowania wpisywanej liczby przez model w czasie edycji.
- Walidacja pustych i uszkodzonych danych, identyfikatorów i odwołań, nieprawidłowych profili i granicznych konfiguracji serii.
- Poprawiono odtwarzanie zapisów, w tym przypadki zgłaszane na „Sezoniku”; nie jest to obietnica odzyskania dowolnie uszkodzonego pliku.
- Zapisy atomowe i jawna obsługa błędów; poprawki automatycznego zapisu i ochrony przed kolizjami instancji.
- Grupowanie odświeżeń modeli podczas automatycznej symulacji zamiast pełnego przebudowywania widoku po każdym skoku.
- Ograniczenie logowania, zbędnych przeliczeń podglądu, powtarzanej walidacji i kosztów odświeżania archiwum.
- Indeksowanie zależności kalendarza zamiast wielokrotnego przeglądania całego zestawu; ograniczenie zbędnego przechodzenia po drzewach Qt Widgets przy odświeżaniu sezonu.
- Zachowanie przewijania i zaznaczeń tam, gdzie reset listy był zbędny, oraz ograniczenie ponownego budowania składów podczas przesuwania drużyn.
- Poprawki zgodności QML na Linuxie: wersjonowane importy, brak nadpisywania właściwości FINAL/read-only, usunięte cykle komponentów, spójny runtime Controls i właściwy moduł Shapes.
- Starsze raporty zawierają pomiary wydajności z wcześniejszych etapów i silników. Nie stanowią gwarancji stałego przyspieszenia o 60–90% w końcowym 2.0.0 ani na każdym komputerze.

## 11. Instalacja, CI i tłumaczenia

- Windows: Inno Setup, instalacja użytkownika, skróty, dołączone Qt i wymagany runtime Microsoft. Aktualizacja i odinstalowanie nie usuwają biblioteki gracza.
- Linux: AppImage z modułami QML, tłumaczeniami i Defaults. Możliwe uruchomienie bez FUSE przez `APPIMAGE_EXTRACT_AND_RUN=1`.
- CI buduje Windows i Linux, testuje uruchomienie faktycznej dystrybucji; Windows dodatkowo instalację/aktualizację/odinstalowanie.
- Kontrola wdrożenia obejmuje nie tylko Home, ale też Workspace, konkurs, statystyki i wykresy. Linux przechodzi kontrolę przed zastąpieniem poprzedniego artefaktu.
- Naprawiono SVG Discorda nieakceptowane przez Qt 5 i dodano kontrolę dostarczanych grafik wektorowych.
- Wprowadzono i wielokrotnie aktualizowano tłumaczenia angielskie i czeskie oraz polskie przyciski standardowych dialogów. Pełne końcowe uzupełnienie tłumaczeń zostało przerwane na prośbę właściciela; nie deklarujemy 100% pokrycia.

# III. Notatki do wydania

## Zgodność i migracja

**2.0.0 należy traktować jako nową generację symulacji.** Stare sezony 1.3.0-beta2 nie są obsługiwane jako w pełni kompatybilne rozgrywki nowego silnika. Zachować kopie przed próbą migracji/importu.

- Część dawnych profili można przeliczyć przybliżeniowo. To nie zachowuje dokładnie zachowania starego silnika.
- Mapowanie poziomów perków nie oznacza zachowania wszystkich dawnych parametrów lub wyników.
- Nowe zasoby najlepiej udostępniać przez `.sjsim-pack`. Surowy JSON starej wersji może wymagać przekształcenia; nie obiecywać bezwarunkowej zgodności każdej paczki i presetu.
- Aktualizacja nie zastępuje istniejącej biblioteki danymi startowymi. Sprawdzając czystą instalację, trzeba użyć odrębnego pustego folderu danych.
- Ukryty, niedokończony edytor rozdzielania zawodników pomiędzy listy nie jest funkcją przeznaczoną do ogłoszenia na premierę.
- Opady oraz wybór alternatywnych metod pomiaru wiatru nie są obecnie udostępnione w normalnym UI.

## Pakiet wydania

1. Zbudować świeży Release ze wskazanego commitu. Sama zmiana źródeł lub push nie aktualizuje starego lokalnego executable.
2. Sprawdzić zawartość `data/defaults`: zasoby startowe, nazwy i zdjęcia, zasady, punktacje i sześć kalendarzy. Nie dodawać prywatnych zapisów, CSV z mapowaniem nazwisk ani kopii bezpieczeństwa.
3. Uruchomić test wdrożenia oraz krótką ręczną kontrolę: powitanie → przykładowy sezon → kwalifikacje → konkurs → archiwum → zapis → ponowne wczytanie.
4. Sprawdzić osobno KO, drużynówkę, import/eksport paczki ze zdjęciem i działanie pustej biblioteki.
5. Udostępnić Windows `Setup.exe` i Linux `AppImage` oraz sumy SHA-256. Instrukcja: [DISTRIBUTION.md](docs/DISTRIBUTION.md).
6. Zwykły push tworzy artefakty CI; publikacja wydania jest uruchamiana przez tag `v…` zgodnie z workflow. Nie utożsamiać udanego buildu z zakończeniem ręcznych testów premiery.
7. Uprzedzić o możliwym ostrzeżeniu Windows o nieznanym wydawcy: certyfikat podpisywania nie został skonfigurowany.
8. Sprawdzić prawa do wszystkich publikowanych zdjęć i grafik. Zmiana dwóch liter nazwiska nie jest zapewnieniem anonimowości ani oceną prawną.

## Co sprawdzono, a czego ten dokument nie gwarantuje

- To zestawienie historii i aktualnego stanu kodu; nie pełny raport ponownego przetestowania każdej funkcji.
- Ostatnie poprawki wdrożenia obejmowały rzeczywisty start linuksowego AppImage z izolowanymi danymi, bez deweloperskich ścieżek Qt, i kontrolę głównych komponentów UI.
- Natywne testy instalatora Windows wykonuje CI, nie Linux autora.
- W tym zadaniu dokumentacyjnym nie uruchamiano ponownie całego zestawu testów silnika ani nie przeprowadzano wielosezonowej walidacji balansu.
- Pełna lista zmian poniżej jest indeksem audytowym. Zmiana pliku, nazwa commitu lub istniejący eksperyment nie oznaczają automatycznie funkcji widocznej dla gracza.

# IV. Pełna historia commitów

Chronologicznie od main do commitu kończącego przegląd. Wpisy pozostawiono w oryginalnym brzmieniu; opis stanu końcowego jest powyżej.

- 2026-10-18 [`648fb2a`](https://github.com/KonradKrol/sj-sim/commit/648fb2a61bb7a8cf61787a7980eeb3317fd4dbd0) — Add optional responsive Qt Quick competition configuration preview
- 2026-10-18 [`7041c12`](https://github.com/KonradKrol/sj-sim/commit/7041c12ad47e41990b66bdc237fb80065a856f9e) — Fix invisible Qt Quick configuration dropdowns
- 2026-10-20 [`8ed248b`](https://github.com/KonradKrol/sj-sim/commit/8ed248b1e53ddb86909380c57d778e1ba35b7d15) — Rebuild Sj.Sim desktop UI in Qt Quick for 1.3.0 beta-3
- 2026-10-21 [`6233bdb`](https://github.com/KonradKrol/sj-sim/commit/6233bdb3217cbf94c387f7cd2c662c71990e2799) — Fix 01: preserve skipped places after archive classification ties
- 2026-10-21 [`6f44d3b`](https://github.com/KonradKrol/sj-sim/commit/6f44d3b133dced41d6e8621acc2e78fa72d2c075) — Fix 02: map statistics range choices to the displayed competition
- 2026-10-21 [`91a1e6e`](https://github.com/KonradKrol/sj-sim/commit/91a1e6e3f5470c57aa698d0a07470e33e5cfa246) — Fix 03: rebuild classification filters without invalidating iteration
- 2026-10-21 [`55e6d16`](https://github.com/KonradKrol/sj-sim/commit/55e6d1663987437e7d9964ec0a85ec7f1efc2e42) — Fix 04: reject invalid form input and contradictory generation limits
- 2026-10-21 [`07d6e30`](https://github.com/KonradKrol/sj-sim/commit/07d6e306cf00a323f2cfeca840719e6b960a037f) — Fix 05: honor zero disqualification override in jump manipulation
- 2026-10-21 [`2993270`](https://github.com/KonradKrol/sj-sim/commit/2993270a44b2d1892251d82b8c31f8d245651cea) — Fix 06: keep all selected athletes in the form generator
- 2026-10-21 [`698feb4`](https://github.com/KonradKrol/sj-sim/commit/698feb4cf58ea116042ce068433dcbf1b4fb2130) — Fix 08: avoid statistics crashes for small fields and tied winners
- 2026-10-21 [`2e4f5cc`](https://github.com/KonradKrol/sj-sim/commit/2e4f5cc5d9d6443d06656df99c6954dff543a87b) — Fix 07: omit unavailable averages instead of producing NaN statistics
- 2026-10-21 [`38e8e31`](https://github.com/KonradKrol/sj-sim/commit/38e8e31dbf5c3b67957a7be61a7048d840934761) — Fix 09: keep statistics usable without point-award presets
- 2026-10-21 [`642a618`](https://github.com/KonradKrol/sj-sim/commit/642a618f2798e2c0810f0f2433ea73de622850ed) — Fix 10: clear obsolete statistics when filters return no results
- 2026-10-21 [`8511b0f`](https://github.com/KonradKrol/sj-sim/commit/8511b0fd5f66c99c5aafafec8956547f5bdd9e97) — Fix 11: handle empty calendars and missing statistics range endpoints
- 2026-10-21 [`37d1b8f`](https://github.com/KonradKrol/sj-sim/commit/37d1b8fada3cc0c8605633cc7ff70c65a13b11c5) — Fix 12: handle incomplete or detached jump details without crashing
- 2026-10-21 [`3872878`](https://github.com/KonradKrol/sj-sim/commit/3872878a3b0cb62194d89122af7adae7687498ea) — Fix 13: keep athlete charts safe for DNS and empty selections
- 2026-10-21 [`56dd0ea`](https://github.com/KonradKrol/sj-sim/commit/56dd0eac02aa15fab5de7fea843ebdd4c1e54bf4) — Document all 13 UX and gameplay fixes with regression evidence
- 2026-10-21 [`e577e73`](https://github.com/KonradKrol/sj-sim/commit/e577e73b7ea544ce34c4944d61606764bad0412e) — UX 1: make form parameter changes explicit and scoped
- 2026-10-21 [`1611e61`](https://github.com/KonradKrol/sj-sim/commit/1611e61e260f8c9b3476bfae3bc51269875289ac) — UX 2: expose table search, selection counts and empty-state recovery
- 2026-10-21 [`0679252`](https://github.com/KonradKrol/sj-sim/commit/06792527b138acda819d5091a9348c9c4caaec5c) — UX 3: make saved athlete lists directly actionable
- 2026-10-21 [`191138e`](https://github.com/KonradKrol/sj-sim/commit/191138e6267b4c4c28a602f6a5a3198bdf605ce1) — UX 4: group competition tools and expose live/weather controls
- 2026-10-21 [`4e1c614`](https://github.com/KonradKrol/sj-sim/commit/4e1c6143896d9c8ef708f0e27ac834d670f97533) — UX 5: separate calendar selection from opening the editor
- 2026-10-21 [`f9a0b90`](https://github.com/KonradKrol/sj-sim/commit/f9a0b908746d77f1684a679cbd3b690bd909c7c9) — UX 1: pin form apply controls and preserve explicit scope
- 2026-10-21 [`8708819`](https://github.com/KonradKrol/sj-sim/commit/870881941fdb1159595c45cf57dc9b4905397c23) — UX 2: distinguish selection from cursor and expose laptop search
- 2026-10-21 [`597e1dd`](https://github.com/KonradKrol/sj-sim/commit/597e1dd59f0284d4b503c3fa832e1ffa6520d9ee) — UX 3: preserve usable athlete rows in the smallest windows
- 2026-10-21 [`77811ae`](https://github.com/KonradKrol/sj-sim/commit/77811aec186f1a2e9bdccbb65fdb73a05972f331) — UX 5: retain keyboard focus across calendar selection refreshes
- 2026-10-21 [`d865528`](https://github.com/KonradKrol/sj-sim/commit/d8655289079fa73be1ca8fb72e77fd3b081897dd) — Save five-workflow UX regressions, translations and resume checkpoint
- 2026-10-21 [`5398f72`](https://github.com/KonradKrol/sj-sim/commit/5398f72a2524784f03f26ee705ae8cb0be48810e) — UX 1: read complete form roster through the existing item model
- 2026-10-21 [`4a49261`](https://github.com/KonradKrol/sj-sim/commit/4a492612c0daef2628eea9a1afcfc7d47eef7b29) — UX 4: keep KO tables inside the laptop competition viewport
- 2026-10-21 [`f1c3d87`](https://github.com/KonradKrol/sj-sim/commit/f1c3d87fcb3ae1d6eb4ea8e5c68cbed225ba6b15) — Document five completed UX workflows and final validation results
- 2026-10-21 [`663bd58`](https://github.com/KonradKrol/sj-sim/commit/663bd5817e23e1b1b83004f5fcaea14ef562170b) — Expand catalog workspaces and expose complete last-jump information
- 2026-10-21 [`870c929`](https://github.com/KonradKrol/sj-sim/commit/870c929177bf697397c46a53ddf94594eb359ed6) — Fix calendar qualification sources and ordering constraints
- 2026-10-21 [`ac0cd90`](https://github.com/KonradKrol/sj-sim/commit/ac0cd903f15a5fc46849153de1af8ba9bcc64042) — Keep modal actions visible and make numeric stepping immediate
- 2026-10-21 [`53fdc7f`](https://github.com/KonradKrol/sj-sim/commit/53fdc7f06ef2e8fce6ee54eee1a991832980c4b4) — Compact competition cards and clarify jump ranking across rounds
- 2026-10-22 [`284a89e`](https://github.com/KonradKrol/sj-sim/commit/284a89e2020c9022d370ac2dc6050f2b5973f1ea) — Localize standard dialog actions and fix message text sizing
- 2026-10-22 [`e021c7b`](https://github.com/KonradKrol/sj-sim/commit/e021c7b40d62439ebb7c7af601218aa0af6c64b9) — Rebuild season calendar editing with reversible synchronized operations
- 2026-10-22 [`33ff226`](https://github.com/KonradKrol/sj-sim/commit/33ff226427fd4dd92f660bf40e31d730e6b3bff1) — Default competition gates to Auto and refresh wind recommendations
- 2026-10-22 [`da0924b`](https://github.com/KonradKrol/sj-sim/commit/da0924ba39b8739ee157744d42c4f098e313f0fe) — Clarify live competition status without revealing jump ranks early
- 2026-10-22 [`50e991d`](https://github.com/KonradKrol/sj-sim/commit/50e991d6f148b1d957e69b7d0ee18e0ec6d91f78) — Document season workflow changes and the 350-render validation
- 2026-10-22 [`e9ce0a4`](https://github.com/KonradKrol/sj-sim/commit/e9ce0a4b30b7ccf08d5fc0ffa63f84122ef6d2ca) — Show the calculated starting gate as a number instead of Auto
- 2026-10-22 [`9a6641c`](https://github.com/KonradKrol/sj-sim/commit/9a6641cd8a4159962e18e08b39691d0310a63053) — Remove redundant jump card heading and live stage captions
- 2026-10-22 [`4697730`](https://github.com/KonradKrol/sj-sim/commit/4697730a718a592b2dac19c40ea6c6475284a2f5) — Scroll competition options only when their content overflows
- 2026-10-22 [`145738d`](https://github.com/KonradKrol/sj-sim/commit/145738dbff659c767d075e20acc4da8cf1a3ad87) — Destroy finished competition views before releasing simulation data
- 2026-10-22 [`da96752`](https://github.com/KonradKrol/sj-sim/commit/da96752d5e74028ae985b3738099e9f409fa6acf) — Show the biggest gain since the previous round in the competition header
- 2026-10-22 [`d78e4c7`](https://github.com/KonradKrol/sj-sim/commit/d78e4c70089cc62ac130e512b48b196a7200e872) — Preserve live hill assignments when serializing calendar presets
- 2026-10-22 [`291ad73`](https://github.com/KonradKrol/sj-sim/commit/291ad732df6f28228dd98354bd4787c078da31fd) — Preserve earlier competition CSV exports when file names collide
- 2026-10-22 [`dbacc92`](https://github.com/KonradKrol/sj-sim/commit/dbacc9210162eb630520bf49446a505b58294ad8) — Keep random samples inside singleton, narrow and extreme bounds
- 2026-10-22 [`44b984f`](https://github.com/KonradKrol/sj-sim/commit/44b984f9909a5e39f1cd5c0074383a4b3a107bf1) — Bind cloned preset result containers to their new calendar events
- 2026-10-22 [`df1a783`](https://github.com/KonradKrol/sj-sim/commit/df1a7830d324c0f5d37639e2094fc63a64bdeb12) — Preserve auxiliary event settings when duplicating calendar blocks
- 2026-10-22 [`d157c29`](https://github.com/KonradKrol/sj-sim/commit/d157c29d0d5dd9af49c33a21ea958a264b7b3058) — Keep duplicated qualification links within the copied calendar block
- 2026-10-22 [`4ddc8c2`](https://github.com/KonradKrol/sj-sim/commit/4ddc8c2b0ceec85d307afc926a33c234f1c8fb18) — Document the complete release change inventory against main beta-2
- 2026-10-22 [`ccf8b0d`](https://github.com/KonradKrol/sj-sim/commit/ccf8b0d7f5dd95330b15ef516ae6b77c54c6e963) — fix: preserve fractional points when reloading presets
- 2026-10-22 [`a894d4b`](https://github.com/KonradKrol/sj-sim/commit/a894d4bf1bd9c32442e190f0d358049ee1404858) — fix: reset competition options scrolling when content fits
- 2026-10-22 [`d89b6f4`](https://github.com/KonradKrol/sj-sim/commit/d89b6f4ab6ee8885daa86eafd25dd1f85621f6e6) — wip: save hill search implementation and resume checklist
- 2026-10-22 [`54fa3f8`](https://github.com/KonradKrol/sj-sim/commit/54fa3f82f27a7226149ee99fee4ca5050d2af20e) — docs: record prior-round gap and jump round placing requirements
- 2026-10-22 [`641c6aa`](https://github.com/KonradKrol/sj-sim/commit/641c6aab0399ed9ea771dd7c6eb49aaff21ba851) — docs: add reminder about author page
- 2026-10-22 [`da2fcd0`](https://github.com/KonradKrol/sj-sim/commit/da2fcd09a10e42c5e2ea48782504ace7102043b5) — feat: validate scoped hill search and handle empty matches
- 2026-10-22 [`284360f`](https://github.com/KonradKrol/sj-sim/commit/284360f402a06877c28bd7b12068c424fa63f0e8) — fix: preserve names and column boundaries in CSV results
- 2026-10-22 [`1171608`](https://github.com/KonradKrol/sj-sim/commit/1171608e23ec44467e1634f6ae195c99492ea183) — fix: derive next competition from calendar state on load
- 2026-10-22 [`5b9f147`](https://github.com/KonradKrol/sj-sim/commit/5b9f147d8a117007f3deb1af8bf10c393b84bb0a) — fix: retain historical team points after nationality changes
- 2026-10-22 [`84e1729`](https://github.com/KonradKrol/sj-sim/commit/84e17296e017a8b0dc32fb25a836a283f182faec) — fix: stage next-season edits and apply accepted additions atomically
- 2026-10-22 [`eef3818`](https://github.com/KonradKrol/sj-sim/commit/eef3818ee25299851d5cf3a79ba30b132c93e9a6) — fix: account for remaining starters in biggest advancement
- 2026-10-22 [`f95bcfa`](https://github.com/KonradKrol/sj-sim/commit/f95bcfaf50cb9474d1f8d522601ca9dc9070c2bc) — feat: show previous-round gaps and each jump round placing
- 2026-10-22 [`100f09a`](https://github.com/KonradKrol/sj-sim/commit/100f09a5cc2ba40f3231338a357d18121584b37d) — docs: track bulk and engine simulation performance work
- 2026-10-22 [`cd3cdba`](https://github.com/KonradKrol/sj-sim/commit/cd3cdba7fd514e7b359a179d493968bf9f236c55) — fix: clear rules editor before replacing next-season rules
- 2026-10-22 [`8090805`](https://github.com/KonradKrol/sj-sim/commit/8090805dbec54fec8995c832f370f54c0202e04f) — perf: batch athlete and start-list presentation in automatic simulation
- 2026-10-22 [`4b8322a`](https://github.com/KonradKrol/sj-sim/commit/4b8322a38c685bb61e7e5ac67847cd449fdb65bf) — i18n: translate hill search and updated competition standings
- 2026-10-22 [`af8f47c`](https://github.com/KonradKrol/sj-sim/commit/af8f47c5b278a26c4ca4805413b8e9ed208ce4e4) — perf: read simulation characteristics without copying their set
- 2026-10-22 [`380b671`](https://github.com/KonradKrol/sj-sim/commit/380b671c450d5897da87cd23497f72da6458dfe5) — perf: compile out debug logging in release builds
- 2026-10-22 [`8d873ea`](https://github.com/KonradKrol/sj-sim/commit/8d873eac32911eae91f63a1e66eebaf3a4eb8a9e) — test: cover hill search within a small laptop viewport
- 2026-10-22 [`18b681d`](https://github.com/KonradKrol/sj-sim/commit/18b681d04d47f439a34863346ba8f2816b7aa9df) — docs: record completed fixes, performance measurements and validation
- 2026-10-22 [`c6c9782`](https://github.com/KonradKrol/sj-sim/commit/c6c97824e9b6bb0d7f85a6621e746484d04ec345) — docs: preserve the release inventory generator for future updates
- 2026-10-22 [`b65723e`](https://github.com/KonradKrol/sj-sim/commit/b65723ed7a5b133229d9e170970decbd591cafe5) — docs: refresh complete release inventory after fixes and optimizations
- 2026-10-22 [`6910a70`](https://github.com/KonradKrol/sj-sim/commit/6910a70e70464e6b3025d164d99a8b66724f27ab) — feat: add author page with portrait, featured Predazzo project and social links
- 2026-10-22 [`bf523d6`](https://github.com/KonradKrol/sj-sim/commit/bf523d63c4bd994bf18b7ce7510f3bd4e533afcf) — fix: unload inactive race options to remove stale scrolling
- 2026-10-22 [`208fce0`](https://github.com/KonradKrol/sj-sim/commit/208fce0796f4fbfbd7694f8e7d34e877e9e0a154) — ui: remove unexplained mode range from the home screen
- 2026-10-22 [`4490b95`](https://github.com/KonradKrol/sj-sim/commit/4490b95c477b2cc07e97d6eae074558c212ae915) — fix: enable start-list sorting by default for newly added rounds
- 2026-10-22 [`04f78b6`](https://github.com/KonradKrol/sj-sim/commit/04f78b661642119b4613efef7d1c263571d3e92a) — fix: connect author navigation explicitly without auto-slot warnings
- 2026-10-22 [`131f87e`](https://github.com/KonradKrol/sj-sim/commit/131f87ed50e19d9beecddc1235d2c177afe428f0) — feat: add persistent compact interface and smaller text options
- 2026-10-22 [`a52c96f`](https://github.com/KonradKrol/sj-sim/commit/a52c96f6eee846307c4f9d48679d12e98cffc9c5) — perf: batch single-jump progress updates and event processing
- 2026-10-22 [`4c4ba9c`](https://github.com/KonradKrol/sj-sim/commit/4c4ba9cf0e3b39bf2d30bcce0ca0ea3c65c0f773) — fix: show full compact author portrait on small screens
- 2026-10-22 [`c4b9212`](https://github.com/KonradKrol/sj-sim/commit/c4b92123934e7f1c010c33b603f507ae8732eab0) — fix: initialize standalone jump importance
- 2026-10-22 [`c9b2682`](https://github.com/KonradKrol/sj-sim/commit/c9b2682bba12e8531a333581a8384023ab9307b7) — perf: reuse normal samples and reflect centered half-normal draws
- 2026-10-22 [`bb4f7a6`](https://github.com/KonradKrol/sj-sim/commit/bb4f7a69d099e5cf76adeb825090d941f0885555) — perf: prepare single-jump wind settings and result buffers once
- 2026-10-22 [`4cae942`](https://github.com/KonradKrol/sj-sim/commit/4cae942f66a2f2d4931e77933392e1372ac4850d) — feat: calculate a starting gate for standalone jumps
- 2026-10-22 [`1ee819b`](https://github.com/KonradKrol/sj-sim/commit/1ee819b40791ee56eb460976cbc1695a9c42e1a2) — fix: let standalone jump results fill the available workspace
- 2026-10-22 [`f945bd2`](https://github.com/KonradKrol/sj-sim/commit/f945bd2ef211c44c077b2dc13e02197c119e5050) — feat: expose three direct competition simulation actions
- 2026-10-22 [`0b81ec2`](https://github.com/KonradKrol/sj-sim/commit/0b81ec2b786aea263476509c9d73e6415751599c) — i18n: translate compact appearance and standalone jump controls
- 2026-10-22 [`7eaf15d`](https://github.com/KonradKrol/sj-sim/commit/7eaf15d9ceb651bda2a78388e67a43f281d2bca6) — fix: place the compact author portrait beside the heading
- 2026-10-22 [`e3c62cb`](https://github.com/KonradKrol/sj-sim/commit/e3c62cbf41a4062da30554dc4767a15251163c72) — docs: record author, compact UI and standalone jump changes
- 2026-10-22 [`d95db6a`](https://github.com/KonradKrol/sj-sim/commit/d95db6ab5d3ba85e661c1c64bc8ae30fca70c5af) — docs: refresh complete release inventory through e3c62cb
- 2026-10-22 [`0eeef38`](https://github.com/KonradKrol/sj-sim/commit/0eeef38b1810d04555b40d27a24248502d2343d9) — docs: add complete Windows development setup for contributors and Codex
- 2026-10-22 [`d844f0c`](https://github.com/KonradKrol/sj-sim/commit/d844f0c01cd770698e701a41f5b255e1892eb83a) — docs: include Windows setup in the release change inventory
- 2026-10-22 [`d55ed9d`](https://github.com/KonradKrol/sj-sim/commit/d55ed9ddadd3ca98d1b0abe4741485398cce2eb9) — Fix Qt 6 Windows build and Quick UI shutdown
- 2026-10-22 [`db2b4ee`](https://github.com/KonradKrol/sj-sim/commit/db2b4ee340ad8b43a252e06423bcbc94880a6eb8) — Fix native Windows Quick page initialization and control style
- 2026-10-22 [`5af7916`](https://github.com/KonradKrol/sj-sim/commit/5af7916180d32292cc008ee14bb81a3f0d467133) — Record Windows UI issues for the next session
- 2026-10-23 [`028cdf2`](https://github.com/KonradKrol/sj-sim/commit/028cdf2730a7fe81a58b6f2b4ab0db195d8a38bd) — Fix Windows page presentation and adopt FIS country codes
- 2026-10-23 [`2e254a4`](https://github.com/KonradKrol/sj-sim/commit/2e254a49e77483526ee57787e85002841553cd81) — Add per-round score resets and advanced carryover weights
- 2026-10-23 [`0dec098`](https://github.com/KonradKrol/sj-sim/commit/0dec09876573913a1fb24814e5f4dfbc8e53c716) — Add named data packs and jumper birth information
- 2026-10-23 [`74ea6b4`](https://github.com/KonradKrol/sj-sim/commit/74ea6b4684bff24f619f27cd2688baa93c396794) — Add per-event classification scoring, age and country filters
- 2026-10-23 [`aa0ce7d`](https://github.com/KonradKrol/sj-sim/commit/aa0ce7dafdbdd0dc4e0b88cfad3204e5ef59a9d3) — Add optional season dates, scheduling tools and calendar overview
- 2026-10-23 [`ece9852`](https://github.com/KonradKrol/sj-sim/commit/ece9852008bc91507f667e68bae4be2473df096c) — Fix stale validation after restoring a valid calendar date
- 2026-10-23 [`df6e5d1`](https://github.com/KonradKrol/sj-sim/commit/df6e5d1d78caa44c785448dd45a8f270c2c231d2) — Keep date picker days accessible in short windows
- 2026-10-23 [`2101965`](https://github.com/KonradKrol/sj-sim/commit/2101965a79fba23695b3e3c7eda1e04f73f7ddbf) — Exclude hidden classification rows from qualification
- 2026-10-23 [`58f2517`](https://github.com/KonradKrol/sj-sim/commit/58f25170547256afabf9d8c9d359f09856f814c3) — Preserve pack catalogs when legacy database files are missing
- 2026-10-23 [`5c99454`](https://github.com/KonradKrol/sj-sim/commit/5c99454019447620b969a11706e66198c5aefe6e) — Cache visible classification rows per table refresh
- 2026-10-23 [`b3db958`](https://github.com/KonradKrol/sj-sim/commit/b3db958099a923e4a3954d7444708d5c90fb42d8) — Reuse QML engine caches to speed up screen navigation
- 2026-10-23 [`7efe6f0`](https://github.com/KonradKrol/sj-sim/commit/7efe6f05553c457226bd44ac69a24edb61036424) — Make compact interface substantially denser
- 2026-10-23 [`123c67c`](https://github.com/KonradKrol/sj-sim/commit/123c67c6779e657db8da91680528117189ee4423) — Avoid duplicate controls when projecting form layouts
- 2026-10-23 [`33605a6`](https://github.com/KonradKrol/sj-sim/commit/33605a6bfdc84a69b42dfbe93583ff626ead8499) — Rebind jump previews after automatic simulation and round changes
- 2026-10-24 [`257a2dd`](https://github.com/KonradKrol/sj-sim/commit/257a2ddf330d10f9abb4df561e5335f62002b047) — ui: remove search from live start lists and KO group results
- 2026-10-24 [`e580393`](https://github.com/KonradKrol/sj-sim/commit/e5803933409472e81d80b55d567bacd084addee5) — fix: show button hints only on intentional hover or help
- 2026-10-24 [`d9bab6f`](https://github.com/KonradKrol/sj-sim/commit/d9bab6fd6d8add1008d3895515219dcc73698a78) — fix: use consistent tenths-based ranking for competition ties
- 2026-10-24 [`c68f92b`](https://github.com/KonradKrol/sj-sim/commit/c68f92b0f7ebea8c5f6988d1441b4f65327da4b3) — fix: isolate coach gate changes from jury and round baselines
- 2026-10-24 [`19d5a97`](https://github.com/KonradKrol/sj-sim/commit/19d5a977a7300f934def4c5126fec4d2b50c288f) — fix: preserve numeric drafts across competition dialog refreshes
- 2026-10-24 [`8023b76`](https://github.com/KonradKrol/sj-sim/commit/8023b7610dc356e6d0e97c18a6c6e8494737afc4) — fix: calculate guaranteed advancement and independent contest qualification
- 2026-10-24 [`fa6ea6e`](https://github.com/KonradKrol/sj-sim/commit/fa6ea6ed09a33291c7a1c1043ddc7b71344312ea) — fix: keep roster dragging active while scrolling long start lists
- 2026-10-24 [`5285fd8`](https://github.com/KonradKrol/sj-sim/commit/5285fd8b0d73986584816610fdc29467f19afcbf) — fix: preserve calendar scroll and compact event rows with weekday context
- 2026-10-24 [`cce62ba`](https://github.com/KonradKrol/sj-sim/commit/cce62bac01b57fe292e4c577bfd8306017e74872) — fix: rebind competition selectors and show hill flags throughout calendar tools
- 2026-10-24 [`2d2cb43`](https://github.com/KonradKrol/sj-sim/commit/2d2cb43cc452539d7563266eb932b218eaa702d4) — i18n: translate qualification markers and advancement explanations
- 2026-10-24 [`8359eba`](https://github.com/KonradKrol/sj-sim/commit/8359eba57a914809a13f86e5b402413f064975f0) — fix: fill available calendar inspector height after event and tab changes
- 2026-10-24 [`2df4f37`](https://github.com/KonradKrol/sj-sim/commit/2df4f376796291d61ff82f84f344f4866969a2fb) — docs: record competition advancement and calendar UX fixes with validation
- 2026-10-24 [`c93f84c`](https://github.com/KonradKrol/sj-sim/commit/c93f84c91c32d85057e815dacf262600111f09f9) — docs: refresh complete release inventory against main
- 2026-10-24 [`3295a84`](https://github.com/KonradKrol/sj-sim/commit/3295a8488a3aeed8f53a3189af5326c580e81fef) — fix: show final tied podium and weighted leader gaps with KO jump previews
- 2026-10-24 [`63c7662`](https://github.com/KonradKrol/sj-sim/commit/63c7662ec6a6d2d4102c41e22343fce78bf5152b) — feat: create simulations from three data packs in one focused setup page
- 2026-10-24 [`faeba46`](https://github.com/KonradKrol/sj-sim/commit/faeba462192562b7888ff0ea0187b81f7c0cb933) — fix: make empty season workflows actionable and synchronize calendar navigation
- 2026-10-24 [`17bd91b`](https://github.com/KonradKrol/sj-sim/commit/17bd91b98894ec75c550cc78547faabb5427a09d) — test: cover simulation onboarding and weighted competition presentation
- 2026-10-24 [`f9fef6c`](https://github.com/KonradKrol/sj-sim/commit/f9fef6c01925b62c2b701166bfbfed4399d3ab97) — docs: record simplified season setup, empty states and final result behavior
- 2026-10-24 [`29ad6ac`](https://github.com/KonradKrol/sj-sim/commit/29ad6ac1a5de1c08c4b58be3b6d0799e78e1aac2) — docs: refresh release manifest after simulation onboarding improvements
- 2026-10-24 [`b65a7be`](https://github.com/KonradKrol/sj-sim/commit/b65a7becd7d6459b44a8473dbf374a866a234930) — ui: align calendar qualification Q right with comfortable padding
- 2026-10-24 [`6c7ebdc`](https://github.com/KonradKrol/sj-sim/commit/6c7ebdcc0b59ef58f35adf95f3df1407c5cda695) — docs: refresh release inventory after calendar badge spacing correction
- 2026-10-24 [`f1b9c33`](https://github.com/KonradKrol/sj-sim/commit/f1b9c33c4525965b3a34a8d9ae92d54a2d70e15b) — test: reproduce ten simulation engine defects independently
- 2026-10-24 [`7cfef95`](https://github.com/KonradKrol/sj-sim/commit/7cfef95c7bc71242e896e2251afa1d6437db3a4c) — fix: initialize jump snapshots and clear stale DNS and coach state
- 2026-10-24 [`87b5792`](https://github.com/KonradKrol/sj-sim/commit/87b5792b4040313a682e5ddedeefab281d57a72c) — fix: validate simulation context and clear detached athlete skills
- 2026-10-24 [`9d6c768`](https://github.com/KonradKrol/sj-sim/commit/9d6c768e504f2c22f57975c4ea9d06b41fce42f0) — fix: guard zero and invalid disqualification probability factors
- 2026-10-24 [`6e50e1c`](https://github.com/KonradKrol/sj-sim/commit/6e50e1cddeed2618ec6f9dbdcbf74c8dc46a0d20) — fix: apply flight height penalty to the difference from the ideal
- 2026-10-24 [`9b8644b`](https://github.com/KonradKrol/sj-sim/commit/9b8644b53d6a3496d1c9fd240e04faca47718b92) — fix: normalize partial and out-of-range exact judge overrides
- 2026-10-24 [`6266cb8`](https://github.com/KonradKrol/sj-sim/commit/6266cb8d161c5777e46a99a6f424559d634a0672) — fix: deduct compensation for a higher coach gate below 95 percent HS
- 2026-10-24 [`111d6d0`](https://github.com/KonradKrol/sj-sim/commit/111d6d0884701c9c2bc75d137edf66289550cd06) — fix: preserve independent automatic metre scoring in hill saves
- 2026-10-24 [`b48fa0d`](https://github.com/KonradKrol/sj-sim/commit/b48fa0d5ea3717ee17f821ca6fa4a1ab5e7df662) — fix: score fractional K points between 169 and 170 correctly
- 2026-10-24 [`090bd61`](https://github.com/KonradKrol/sj-sim/commit/090bd619da0c55e59422498b8d056aa9c276ee11) — fix: normalize multi-turn wind directions before simulation
- 2026-10-24 [`53a9960`](https://github.com/KonradKrol/sj-sim/commit/53a99607d8d09ae1cc11338c9a9a407577d32a8f) — fix: terminate degenerate half-normal sampling and sample rare tails efficiently
- 2026-10-24 [`a99df69`](https://github.com/KonradKrol/sj-sim/commit/a99df69ddd3190a4df1e3600db4ce4debf099190) — perf: simulate large independent jump batches on isolated worker threads
- 2026-10-24 [`9174fb8`](https://github.com/KonradKrol/sj-sim/commit/9174fb85c24095f73b41e0663e4b442fdf07aaba) — docs: record ten engine fixes and measured asynchronous batch performance
- 2026-10-24 [`b04f612`](https://github.com/KonradKrol/sj-sim/commit/b04f612130bd6729ac308f45386f8934e4073181) — release: set shared application version to 2.0.0-beta
- 2026-10-24 [`0fc093d`](https://github.com/KonradKrol/sj-sim/commit/0fc093da61dc699dbaf613d27b45681881be7e22) — fix: add the first calendar competition directly from season overview
- 2026-10-24 [`a72a6b5`](https://github.com/KonradKrol/sj-sim/commit/a72a6b522992e92dcee8744f0bcacd50c9fec7b5) — fix: retranslate roster labels without reading retired athlete storage
- 2026-10-24 [`5af8f1b`](https://github.com/KonradKrol/sj-sim/commit/5af8f1b80c93d27c90a82c6a3fd2fe97851a30ac) — fix: compare pre-jump gaps at the same completed-round boundary
- 2026-10-24 [`39e66fd`](https://github.com/KonradKrol/sj-sim/commit/39e66fdd71e253dfe0bd985a54281add203b195b) — docs: refresh release inventory at engine audit diagnostic checkpoint
- 2026-10-24 [`3a3ded9`](https://github.com/KonradKrol/sj-sim/commit/3a3ded99075532cd721d83e5c4140f8886c75f69) — docs: confirm final engine audit and AddressSanitizer verification
- 2026-10-24 [`f735a03`](https://github.com/KonradKrol/sj-sim/commit/f735a03d3d5feff4012eb5fef74015b371769809) — docs: refresh final 2.0.0-beta release inventory
- 2026-10-24 [`a872bf0`](https://github.com/KonradKrol/sj-sim/commit/a872bf0796206346301806015053f930f405e539) — Add UI scale settings foundation and fixed event date format
- 2026-10-24 [`5ae6dc1`](https://github.com/KonradKrol/sj-sim/commit/5ae6dc1292e5d1c6a653bc8d3d7e800ed44d09d3) — Fix KO regrouping crashes and validate manual group replacements
- 2026-10-24 [`44cfcbf`](https://github.com/KonradKrol/sj-sim/commit/44cfcbf1743af7714299c1818b116c5a908e3022) — Fix percentage editing in appearance settings
- 2026-10-24 [`2d4faba`](https://github.com/KonradKrol/sj-sim/commit/2d4faba66fa27454dd24e479973c7ea4bbc44eff) — Improve team results, whole-team DSQ and KO advancement feedback
- 2026-10-24 [`9223410`](https://github.com/KonradKrol/sj-sim/commit/9223410d0ad650a756ed48ef0441c213bcc78f93) — Document team and KO changes, validation and remaining queue
- 2026-10-24 [`3690acc`](https://github.com/KonradKrol/sj-sim/commit/3690accc456e6a0e9c3dccaed3203e427051c48a) — Cover fixed event date formatting across locales
- 2026-10-24 [`f7a739b`](https://github.com/KonradKrol/sj-sim/commit/f7a739b4f01c1f4052b1d1d963fef08dfa3152c8) — Preserve latest-jump markers and confirmed advancement colors
- 2026-10-24 [`1737d5b`](https://github.com/KonradKrol/sj-sim/commit/1737d5b55d90fa297f92c03ef203678a1941d7a3) — Document latest-jump and advancement feedback fixes
- 2026-10-24 [`c6bc131`](https://github.com/KonradKrol/sj-sim/commit/c6bc1318d23d2587616ae808d3cb082a72cd8dd4) — Replace pre-competition KO setup with a direct group workspace
- 2026-10-24 [`2f1542b`](https://github.com/KonradKrol/sj-sim/commit/2f1542b866f879d78abdf383b60eb247fa19764e) — Show one weighted previous-series comparison with the current leader
- 2026-10-24 [`8ebe631`](https://github.com/KonradKrol/sj-sim/commit/8ebe631e27d80bc9887eb747747008b9c3960855) — Remove move-to-position controls from team lineup ordering
- 2026-10-24 [`980365a`](https://github.com/KonradKrol/sj-sim/commit/980365a395543b2bd6b2ff5058f991fff278ef04) — Document KO setup redesign and simplified pre-jump comparison
- 2026-10-24 [`c9cd4af`](https://github.com/KonradKrol/sj-sim/commit/c9cd4aff15426f2a688eeebc2d082addf4766a39) — Eliminate recursive layout churn in shared empty states
- 2026-10-24 [`b80a8a6`](https://github.com/KonradKrol/sj-sim/commit/b80a8a6bf65c14c71f40f037ece6631d002adf30) — Record layout diagnosis, validation and remaining jump-card work
- 2026-10-25 [`468e112`](https://github.com/KonradKrol/sj-sim/commit/468e112a2995de461a78abd314e07ddf4b841bc6) — Add historical jump previews and weighted score details
- 2026-10-25 [`bfd9767`](https://github.com/KonradKrol/sj-sim/commit/bfd9767db868d706a9d3c267d581f2d0ae161131) — Document jump history, weights and team table changes
- 2026-10-25 [`1244494`](https://github.com/KonradKrol/sj-sim/commit/12444941a90ad50c6c0feee3d5c52a95f1b0d36c) — Keep KO workspace stable across periodic bridge refreshes
- 2026-10-25 [`d21a99e`](https://github.com/KonradKrol/sj-sim/commit/d21a99ecae2ee1362bfe8bf9f1dd69e5c6ce66ca) — Record KO refresh fix in release inventory
- 2026-10-25 [`cdf73d0`](https://github.com/KonradKrol/sj-sim/commit/cdf73d0aa616c3803239eef4d4e9ab9429b1a6dd) — Record final jump history and KO validation results
- 2026-10-25 [`dce5736`](https://github.com/KonradKrol/sj-sim/commit/dce57362917a4f4a74dce1b5d0e41df725719992) — Avoid rasterizing KO flags on every bridge refresh
- 2026-10-25 [`c11be0b`](https://github.com/KonradKrol/sj-sim/commit/c11be0b8b89a01a13df18ad193fe33deb46e2890) — Add searchable KO athlete picker and drag-to-swap
- 2026-10-25 [`b0d0a3d`](https://github.com/KonradKrol/sj-sim/commit/b0d0a3d9c6e787856ccf789c8d5545caa0ca7f62) — Document KO performance measurements and editing workflow
- 2026-10-25 [`0bd7577`](https://github.com/KonradKrol/sj-sim/commit/0bd75771608538a5f481bbfd86ae143930e50647) — Clarify existing table fixture warnings in KO validation report
- 2026-10-25 [`93661fd`](https://github.com/KonradKrol/sj-sim/commit/93661fded446730aaa9d1babd944754081bcaf6a) — Simplify jumper catalog actions and preserve result scroll
- 2026-10-25 [`8e7d7d9`](https://github.com/KonradKrol/sj-sim/commit/8e7d7d9a4c544e6d1c3a46bc0c1e86a0e4db48fe) — Document jumper catalog and result scroll fixes with validation
- 2026-10-25 [`52fcd85`](https://github.com/KonradKrol/sj-sim/commit/52fcd8574799b2b0b30aaaa5caf8a0a764dd7ae9) — Keep lucky-loser badges scoped to the current KO round
- 2026-10-25 [`885648d`](https://github.com/KonradKrol/sj-sim/commit/885648d1565eb64ab43e4bf79709c84aaffa9628) — Reuse searchable KO editor between rounds and simplify reset scores
- 2026-10-25 [`d2c72ae`](https://github.com/KonradKrol/sj-sim/commit/d2c72aea719fb21a07a20eeca4e26ee7f105c762) — Record SuperKO regression, between-round editor and reset presentation
- 2026-10-25 [`c6ee51d`](https://github.com/KonradKrol/sj-sim/commit/c6ee51dad47d2a48ad3551fd3c742d7dea0b208b) — Add isolated round snapshots and raw individual team ranking
- 2026-10-25 [`df9ace3`](https://github.com/KonradKrol/sj-sim/commit/df9ace3d32e5487025b570d100b7d5030b0d1ea1) — Synchronize season archives and add classification checkpoints
- 2026-10-25 [`eb5a15c`](https://github.com/KonradKrol/sj-sim/commit/eb5a15cd7f6c3db07c75b976df76e4a491880cfa) — Expose paused result history, archive previews and one-attempt DNS
- 2026-10-25 [`e640f45`](https://github.com/KonradKrol/sj-sim/commit/e640f457509191776cbf3ff2b05f6935805f317c) — Keep historical viewing separate from live controls and records
- 2026-10-25 [`d574f3d`](https://github.com/KonradKrol/sj-sim/commit/d574f3d74087e55c047d19afc79a2c0d455b9237) — Document archive synchronization, result timelines, team ranking and DNS
- 2026-10-25 [`8247748`](https://github.com/KonradKrol/sj-sim/commit/8247748621404f4d986e7592db8687eae020625a) — Refresh release manifest for archive and result history changes
- 2026-10-25 [`90bbd79`](https://github.com/KonradKrol/sj-sim/commit/90bbd796d81bbb41e2c5ac7b4878fdef2c92a53d) — Fix historical results rebuild storms and redundant round choices
- 2026-10-25 [`55b5563`](https://github.com/KonradKrol/sj-sim/commit/55b55633ddeadc40b3ae2c9f8b815ceda4f86d08) — Compact result controls and team preparation; preserve historical layout
- 2026-10-25 [`2b1f99d`](https://github.com/KonradKrol/sj-sim/commit/2b1f99dc5121c39ce8d1bc6b2b73a04828238500) — Document result view, team setup and measured history performance fixes
- 2026-10-25 [`729c632`](https://github.com/KonradKrol/sj-sim/commit/729c6323c2b6c74e06caeae1f7a0821c84178f8a) — Refresh complete release inventory against main
- 2026-10-25 [`8a5cc69`](https://github.com/KonradKrol/sj-sim/commit/8a5cc692eb882e241df6b676996fd213538da342) — Group series, team view and ranking choices on the shared results bar
- 2026-10-25 [`bc77232`](https://github.com/KonradKrol/sj-sim/commit/bc7723299b153970b272baef843efac9c38fa6f7) — Give calendar event and classification editors full column height
- 2026-10-25 [`6b7b7bb`](https://github.com/KonradKrol/sj-sim/commit/6b7b7bb59f9801f6358779c12b0a3f464ef7cea5) — Update release inventory for calendar editor layout fixes
- 2026-10-25 [`4db7d0d`](https://github.com/KonradKrol/sj-sim/commit/4db7d0d02621d6f2ef45e6675d660b0756255e39) — Rebuild classification editing with clear hierarchy and progressive disclosure
- 2026-10-25 [`16637c5`](https://github.com/KonradKrol/sj-sim/commit/16637c5e0a1e4b363a21c1cfa557cca41a7ae803) — Make scoring methods visible and compact points editing without scroll jumps
- 2026-10-25 [`7394f7a`](https://github.com/KonradKrol/sj-sim/commit/7394f7abe73a0f158adb583f701a8281975945e8) — Use plain result dropdowns and pause individual previews in team events
- 2026-10-25 [`cab65b2`](https://github.com/KonradKrol/sj-sim/commit/cab65b2ef6c49d06a4c19fc752e047662fc57514) — Record UX verification and pending classification policy decisions
- 2026-10-25 [`4954d04`](https://github.com/KonradKrol/sj-sim/commit/4954d040157f04d33063a6d97b778fd4cc95826f) — Refresh release notes inventory after classification and result UX updates
- 2026-10-25 [`91e0858`](https://github.com/KonradKrol/sj-sim/commit/91e0858ba7aca089684cff6c234e9a1ee40cf23b) — Size result dropdown backgrounds from their full static content
- 2026-10-25 [`8073676`](https://github.com/KonradKrol/sj-sim/commit/807367661440644ca5b90207a1222935300082f0) — Preserve scoring grid scroll with incremental point rows
- 2026-10-25 [`d3ad04c`](https://github.com/KonradKrol/sj-sim/commit/d3ad04c6761df456216458d6ffeb069f09d46dc0) — Bind classification ages to seasons and expose per-country contribution details
- 2026-10-25 [`617006a`](https://github.com/KonradKrol/sj-sim/commit/617006aaab31865d7c74a5b9802ff63d532ca04c) — Document resolved classification rules and complete validation
- 2026-10-25 [`985316d`](https://github.com/KonradKrol/sj-sim/commit/985316d3f5c7b93250cd4312237e2e46a0297009) — Refresh release inventory for season scoring and contribution views
- 2026-10-25 [`e7a7554`](https://github.com/KonradKrol/sj-sim/commit/e7a75548710673833d3028119bab56e06a111bc3) — Save calendar rules automatically and simplify scheduling and standings
- 2026-10-25 [`df0ea86`](https://github.com/KonradKrol/sj-sim/commit/df0ea86ab526f5fc71c5488d08ab66296ffad135) — Document weekend planning, automatic rule saves and standings verification
- 2026-10-25 [`878beec`](https://github.com/KonradKrol/sj-sim/commit/878beec232cbb5beb2554fec738536a81edeb7fc) — Refresh release inventory after calendar planning improvements
- 2026-10-25 [`c5b0b96`](https://github.com/KonradKrol/sj-sim/commit/c5b0b96704042640155447766ccccfac3f32b11d) — Move start-list teams without rebuilding squads or resetting the list
- 2026-10-25 [`f2aeef0`](https://github.com/KonradKrol/sj-sim/commit/f2aeef0bdd9c00f3f72fe06d3156a12071db2c3b) — Cap advancement baselines at the next-round field limit for KO qualifiers
- 2026-10-25 [`848ef94`](https://github.com/KonradKrol/sj-sim/commit/848ef94a8f38df7a2c8f57c7768adc7a67be2d71) — Allow browsing locked classification methods while keeping scoring read-only
- 2026-10-25 [`615eef0`](https://github.com/KonradKrol/sj-sim/commit/615eef0604f317d74da80e7564249ec53dc9b44a) — Cover fast team moves, KO gain caps and read-only method previews
- 2026-10-25 [`4c3aabf`](https://github.com/KonradKrol/sj-sim/commit/4c3aabf76c49bf5cf27e33e5ff3587a59684a199) — Refresh release inventory for team ordering and scoring previews
- 2026-10-25 [`c62e87c`](https://github.com/KonradKrol/sj-sim/commit/c62e87c25ff780f4abd658afc1d4bafe82ddbb09) — Remember simulation list order across additions, deletions and restarts
- 2026-10-25 [`d000b48`](https://github.com/KonradKrol/sj-sim/commit/d000b4865a52dcda8a4535ef1ca3b5bec1111cfb) — Reduce simulation JSON whitespace without changing saved data
- 2026-10-25 [`a27c5c4`](https://github.com/KonradKrol/sj-sim/commit/a27c5c4ef72475ea3db6bbe8fdb2e71b0404611c) — Verify persistent save ordering, smaller JSON and legacy compatibility
- 2026-10-25 [`1fcdad3`](https://github.com/KonradKrol/sj-sim/commit/1fcdad366c82a7dae173e4f50c0073fbd5ad583f) — Refresh release inventory for save ordering and storage improvements
- 2026-10-25 [`1ddc3d2`](https://github.com/KonradKrol/sj-sim/commit/1ddc3d233330613c78bdc47ee00cc7d991143fba) — Separate date planning from weekend creation and preview weekends automatically
- 2026-10-25 [`3180890`](https://github.com/KonradKrol/sj-sim/commit/31808901743f84fc95f6ff6a292ca8c2e649510f) — Refresh release inventory after automatic weekend previews
- 2026-10-25 [`6c7aaef`](https://github.com/KonradKrol/sj-sim/commit/6c7aaef7ee14e9d83a05a7e6ab5473774964f6d0) — Add isolated repeatable random streams for simulation experiments
- 2026-10-25 [`e72570a`](https://github.com/KonradKrol/sj-sim/commit/e72570ab9c37b4dd7a738c4765a32aaaef5aac05) — Introduce multithread simulation playground with paired experiments, analysis UI and headless CLI
- 2026-10-25 [`3795f1f`](https://github.com/KonradKrol/sj-sim/commit/3795f1f0b3ab49bdc0d9be0d2a4f78cb639d83c6) — Add experiment-scoped takeoff flight and wind physics coefficients
- 2026-10-25 [`2305df8`](https://github.com/KonradKrol/sj-sim/commit/2305df89f489021c02ab5294777c72d23787da59) — Harden playground replay determinism paired statistics and cancellation
- 2026-10-25 [`b84cd12`](https://github.com/KonradKrol/sj-sim/commit/b84cd12e54395fb22f0e3bca28651e58cdc8a5fd) — Document simulation playground workflows experiments and measured validation
- 2026-10-25 [`c684454`](https://github.com/KonradKrol/sj-sim/commit/c684454b58ef6a9e42002bd3738734bc95c02401) — Refresh release inventory with complete simulation playground
- 2026-10-26 [`335a7d0`](https://github.com/KonradKrol/sj-sim/commit/335a7d02cf83b6a760b629ce285a8271a1e847f7) — Add isolated athlete model and three expressive Qt Quick editor views
- 2026-10-26 [`1d84c07`](https://github.com/KonradKrol/sj-sim/commit/1d84c07d60cf5d0cb14791f334dce803cb8f6ff2) — Document new athlete editor isolation migration and package workflow
- 2026-10-26 [`392841c`](https://github.com/KonradKrol/sj-sim/commit/392841cfaf30f175feda0337537ce0b799943c9f) — Refresh release inventory for independent athlete editor
- 2026-10-26 [`43c140f`](https://github.com/KonradKrol/sj-sim/commit/43c140f7da52957b1cfdf0923870dcd4494752ff) — Replace athlete previews with five distinct game-inspired concepts and compact perk badges
- 2026-10-26 [`2b6f3b3`](https://github.com/KonradKrol/sj-sim/commit/2b6f3b3bc6b0e8a49ff36697758c4047211f863b) — Document five athlete concepts equipped badges and tooltip-only perk help
- 2026-10-26 [`7ce0273`](https://github.com/KonradKrol/sj-sim/commit/7ce0273a352f8792c4b2396481751137c3642b82) — Refresh release inventory after athlete UI redesign
- 2026-10-26 [`45ee63c`](https://github.com/KonradKrol/sj-sim/commit/45ee63c1d38309b67a065ff88d903a1be15cedac) — Refine patch perk workflow, pack-relative portraits and compact rating steppers
- 2026-10-26 [`4007be1`](https://github.com/KonradKrol/sj-sim/commit/4007be174a452ffd7077ee0da1a24c7e5caab1d5) — Document compact optional perks and pack-relative image workflow
- 2026-10-26 [`715bdd1`](https://github.com/KonradKrol/sj-sim/commit/715bdd1082acde7303abd9d64a67436a8f59f192) — Refresh release inventory after patch editor usability changes
- 2026-10-26 [`360b18e`](https://github.com/KonradKrol/sj-sim/commit/360b18eec29502fdeb643bad7c88c8dd040a6421) — Handle each perk button release during rapid double clicks
- 2026-10-26 [`bdbfaf3`](https://github.com/KonradKrol/sj-sim/commit/bdbfaf3ae3aa0bc50b84b2f94a354c763dd039a4) — Make strengthened perks unmistakable with full gold treatment
- 2026-10-26 [`d6e435c`](https://github.com/KonradKrol/sj-sim/commit/d6e435ce872d57001c3c76125ec738b56f101c3a) — Add AI-generated athlete perk pictograms across badge views
- 2026-10-26 [`a25a584`](https://github.com/KonradKrol/sj-sim/commit/a25a584c4f13bf3d35cf6d72cf7681e9f7ce54eb) — Refresh release inventory through AI perk icon integration
- 2026-10-26 [`a729e4f`](https://github.com/KonradKrol/sj-sim/commit/a729e4f18b9201fbbc05534912b22aa720bb200b) — Give athlete landing choices a compact panel and clear selection
- 2026-10-26 [`a6d38fa`](https://github.com/KonradKrol/sj-sim/commit/a6d38fac7805cf655b4a167f2ae34136ec12641e) — Distinguish flight and takeoff perks and simplify consistency icon
- 2026-10-26 [`217dbbb`](https://github.com/KonradKrol/sj-sim/commit/217dbbb654f941d2152044057a00c3f7f331f0e9) — Refresh release inventory after landing and perk icon revisions
- 2026-10-26 [`1067887`](https://github.com/KonradKrol/sj-sim/commit/106788742475e956293cef9faa822be36afab185) — Show landing quality as five ordered levels
- 2026-10-26 [`dbac449`](https://github.com/KonradKrol/sj-sim/commit/dbac4499b5b24c6be3db6946c6a52e1b33dc1653) — Record ordered landing levels in release inventory
- 2026-10-26 [`8a4b07e`](https://github.com/KonradKrol/sj-sim/commit/8a4b07e0c7e4a6519a763bf99ca1e1e4fdfb1c0a) — Compact landing quality into a single five-level row
- 2026-10-26 [`4c9177b`](https://github.com/KonradKrol/sj-sim/commit/4c9177b57a36970313a29bec468e299340c5e4bb) — Remove athlete concept chooser and open patches directly
- 2026-10-26 [`cfd8d63`](https://github.com/KonradKrol/sj-sim/commit/cfd8d636f9685cd0b2b01733a671c771ddfad353) — Refresh release inventory after athlete editor compaction
- 2026-10-27 [`ceb36ba`](https://github.com/KonradKrol/sj-sim/commit/ceb36ba9e91a26459df8b7d1774dda5326894361) — Restore left athlete list beside the new patches editor
- 2026-10-27 [`80bfbeb`](https://github.com/KonradKrol/sj-sim/commit/80bfbebb6112ab6a9f9e109b664e47acb16c8110) — Refresh release inventory with athlete side list changes
- 2026-10-27 [`7029935`](https://github.com/KonradKrol/sj-sim/commit/7029935a6aca749d87cf230393572db7c0015d8a) — Complete localization and modern editors with ten regression fixes
- 2026-10-27 [`dea79d8`](https://github.com/KonradKrol/sj-sim/commit/dea79d8a7b3681cc1d4456a4af2036863ba0e06b) — Polish menus and editors, optimize perk selection and add UI motion
- 2026-10-27 [`e66d3ea`](https://github.com/KonradKrol/sj-sim/commit/e66d3ea5f4c7de1bb30167fca19c21c8c69e01bd) — Restore Qt 5 compatibility for Quick startup and editors
- 2026-10-27 [`a2fe448`](https://github.com/KonradKrol/sj-sim/commit/a2fe4484dfe2b7ae0479da747b4eb321be5976f1) — Remove assigned perk height binding loop on Qt 5
- 2026-10-27 [`4587bc8`](https://github.com/KonradKrol/sj-sim/commit/4587bc8dda32da0f3d9806146a377e9ea3a54307) — Update release inventory for Linux Qt 5 compatibility
- 2026-10-27 [`6486a85`](https://github.com/KonradKrol/sj-sim/commit/6486a85cf8f99b70334e5a3121771c0c184f1825) — Document continuous wind model for simulation v2
- 2026-10-27 [`55feaa7`](https://github.com/KonradKrol/sj-sim/commit/55feaa7eb2298438f408fd4e0f4dc2a432923055) — Specify wind corridor history and FIS-style compensation
- 2026-10-27 [`6095d31`](https://github.com/KonradKrol/sj-sim/commit/6095d31088dff769483a01a99920f8e0ac05df6d) — Clarify FIS wind measurement averaging requirement
- 2026-10-27 [`8729b50`](https://github.com/KonradKrol/sj-sim/commit/8729b5024203598751c206be3b475c36861a9cc5) — Record simulation v2 scope, research and implementation checklist
- 2026-10-27 [`b60bbd4`](https://github.com/KonradKrol/sj-sim/commit/b60bbd488d6db2117434f002d39357b6eb9cb641) — Add injectable seeded randomness for simulation v2
- 2026-10-27 [`05ae84f`](https://github.com/KonradKrol/sj-sim/commit/05ae84f14c25f5e937a6b5502931952498dfb5cc) — Specify wind setup UX and hill direction preference perks
- 2026-10-27 [`73293e6`](https://github.com/KonradKrol/sj-sim/commit/73293e65bbfc0b2f5207700a644ba2fb34edf528) — Record approved simulation v2 migration and calibration decisions
- 2026-10-27 [`8969679`](https://github.com/KonradKrol/sj-sim/commit/896967984ad0d5234849f505664a48daeba10446) — feat(simulation-v2): add tunable quality model and monotone hill skill curve
- 2026-10-27 [`b99be3e`](https://github.com/KonradKrol/sj-sim/commit/b99be3efa5c813059848b24dd2e64c898532b86b) — feat(simulation-v2): implement continuous seeded weather and automatic proposals
- 2026-10-27 [`13954d0`](https://github.com/KonradKrol/sj-sim/commit/13954d0b90f1370bd5233ed4d5870dca9b8a8175) — feat(simulation-v2): add calibrated flight, landing and scoring engine
- 2026-10-27 [`eedff18`](https://github.com/KonradKrol/sj-sim/commit/eedff183a6067e6ee126c3ca54698b747d963cd8) — feat(hills): add prevailing wind direction perks with compatible pack loading
- 2026-10-27 [`24b5224`](https://github.com/KonradKrol/sj-sim/commit/24b5224ac433ceab96ba71895a79d15fdfb3bc49) — feat(simulation-v2): add explicit jump overrides with consistent wind exposure
- 2026-10-27 [`274c68d`](https://github.com/KonradKrol/sj-sim/commit/274c68df0aa3c3c32eaae51832e453dd743c1e02) — fix(simulation-v2): keep neutral athletes neutral and reject degenerate curves
- 2026-10-27 [`b99afd4`](https://github.com/KonradKrol/sj-sim/commit/b99afd44ac8467c42970826e2371ff2dd70dc120) — fix(profiles): preserve custom-hill records and clarify approximate migration
- 2026-10-27 [`00e0fb2`](https://github.com/KonradKrol/sj-sim/commit/00e0fb2ea4b38d39462526fc1671ec268676f192) — feat(simulation-v2): bridge new profiles into game results and migrate packs safely
- 2026-10-27 [`04ec0ff`](https://github.com/KonradKrol/sj-sim/commit/04ec0ff8dc244294080839c4167a78008f2dc455) — docs(simulation-v2): record complete engine handoff, tests and remaining integration
- 2026-10-27 [`d2b4654`](https://github.com/KonradKrol/sj-sim/commit/d2b4654e9fd6d3bf4ffe783c0be05ff25039cde4) — feat(ui): always open the athlete and hill studios without an editor switch
- 2026-10-27 [`a689c3a`](https://github.com/KonradKrol/sj-sim/commit/a689c3ad858db3f179a225ab77e4dd32d4b3286e) — feat(simulation-v2): unify canonical profile storage and catalog snapshots
- 2026-10-28 [`d27eb28`](https://github.com/KonradKrol/sj-sim/commit/d27eb28eead7de84af9d74473f045a2485c4a967) — feat(simulation-v2): run standalone jumps from canonical profiles and local drafts
- 2026-10-28 [`8a87132`](https://github.com/KonradKrol/sj-sim/commit/8a87132897d6807fc6810e840f541f1721ae3b59) — feat(simulation-v2): model persistent wind tendencies and changing instability
- 2026-10-28 [`e11125e`](https://github.com/KonradKrol/sj-sim/commit/e11125ec8bad33888299fbc35114d7bb63d5fca1) — feat(weather): add sampled scenarios, compact setup and continuous batch conditions
- 2026-10-28 [`ceea5b7`](https://github.com/KonradKrol/sj-sim/commit/ceea5b79397b6bf6e2751e10c5317957c501b046) — fix(simulation-v2): validate contest profiles and bound quality-based gate selection
- 2026-10-28 [`7c762b0`](https://github.com/KonradKrol/sj-sim/commit/7c762b02592eb01269d4b6847b0e918485a3d4b7) — fix(simulation-v2): apply exact wind overrides to injected weather and preview
- 2026-10-28 [`724f12e`](https://github.com/KonradKrol/sj-sim/commit/724f12e950b6fc395e4adbb1f2ba668397983ed2) — refactor(simulation-v2): share contest weather clock across all jump paths
- 2026-10-28 [`eb373c7`](https://github.com/KonradKrol/sj-sim/commit/eb373c7d998134ba0862c0737ff782e9d304afd0) — fix(ui): unify perk badges and expose high-contrast named strength choices
- 2026-10-28 [`32c894a`](https://github.com/KonradKrol/sj-sim/commit/32c894aee18d4a483f65463ae6943c82b45fdd8a) — feat(weather): simplify hill-based setup and separate rain from snowfall effects
- 2026-10-28 [`0e21023`](https://github.com/KonradKrol/sj-sim/commit/0e210231b9baf0113bf369761b114e4e9a8f38de) — feat(simulation-v2): run standalone contests from canonical packs and shared weather
- 2026-10-28 [`48053a3`](https://github.com/KonradKrol/sj-sim/commit/48053a31730788dd20f4b19efc84b5bfefb57b74) — feat(ui): replace wind scenario dropdown with compact directional tiles
- 2026-10-28 [`7918455`](https://github.com/KonradKrol/sj-sim/commit/7918455091d4c32d429453cb0afa001344583c6e) — fix(simulation-v2): update active profile form without losing athlete parameters
- 2026-10-28 [`4911d31`](https://github.com/KonradKrol/sj-sim/commit/4911d3183531bce1e1ee909f9464475a8b6da2dd) — feat(weather): add full-angle direction and continuous local wind editing
- 2026-10-28 [`c345288`](https://github.com/KonradKrol/sj-sim/commit/c3452882405d27403098cb5b15a913ba4c3cabd9) — feat(simulation-v2): validate season engine format and restore portable photo context
- 2026-10-28 [`f3749d7`](https://github.com/KonradKrol/sj-sim/commit/f3749d7c3a1e9bc22223fc8c1d0aa6291eff574f) — fix(simulation-v2): model damaging heavy rain with a tunable continuous curve
- 2026-10-28 [`a97a27d`](https://github.com/KonradKrol/sj-sim/commit/a97a27d3c38af871845fad79d8be9e793ad20bc0) — fix(ui): edit wind directly on slope arrows with clear skier-relative directions
- 2026-10-28 [`586eebc`](https://github.com/KonradKrol/sj-sim/commit/586eebc112e7bc8223b83babc88caee5737615ce) — feat(simulation-v2): create and run seasons with canonical profiles and continuous weather
- 2026-10-28 [`6abc196`](https://github.com/KonradKrol/sj-sim/commit/6abc1963529ddd50c568d823467b55f1ab959d92) — feat(weather): inspect live field and recorded jump wind phases in contests and archives
- 2026-10-28 [`cfc28eb`](https://github.com/KonradKrol/sj-sim/commit/cfc28ebd1d2b922be72a8363aea59e533e4523c2) — fix(i18n): refresh competition action captions and remove footer sizing feedback
- 2026-10-28 [`9d321b9`](https://github.com/KonradKrol/sj-sim/commit/9d321b98618178ca8e9a6a84ba401e02d3d7f595) — fix(i18n): update captions without rebinding competition navigation actions
- 2026-10-28 [`eafc4dc`](https://github.com/KonradKrol/sj-sim/commit/eafc4dc8c9bff305cc6b72fd4c9811eb70337187) — feat(weather): add six independent wind measurement methods with explicit FIS approximation
- 2026-10-28 [`63862c7`](https://github.com/KonradKrol/sj-sim/commit/63862c7c88632747dc35f4c68689e40d298ab050) — feat(simulation-v2): rebuild one-jump manipulation with validated drafts and local continuous wind
- 2026-10-28 [`58460c6`](https://github.com/KonradKrol/sj-sim/commit/58460c6495c106bf0c56154df2a67869dd104bd4) — fix(weather): decouple controls, clarify local wind editing and document cross-machine handoff
- 2026-10-28 [`69756f8`](https://github.com/KonradKrol/sj-sim/commit/69756f8493269232b4898b4c4a6b80f81869a96e) — Add v2 simulation laboratory and complete Windows integration audit
- 2026-10-29 [`289bcc6`](https://github.com/KonradKrol/sj-sim/commit/289bcc60140d88b9381a85937ab18124bba2ea18) — Prepare balancing with tunable wind response, K-based form and lab design
- 2026-10-29 [`21c6f27`](https://github.com/KonradKrol/sj-sim/commit/21c6f2749b4bf67dba1275a59755506a61335125) — Add comprehensive balance laboratory with frozen game profiles
- 2026-10-29 [`f12dde9`](https://github.com/KonradKrol/sj-sim/commit/f12dde9ec134bec64a8ba8d2a84295bcc4ac445b) — Speed up lab contests and restore complete results
- 2026-10-29 [`61846d4`](https://github.com/KonradKrol/sj-sim/commit/61846d40696e771c068baf8060e917d900ec809d) — Clarify balance laboratory UX and preserve full results alongside faster previews
- 2026-10-29 [`3c7f9c4`](https://github.com/KonradKrol/sj-sim/commit/3c7f9c410698ee0431e9613b1ed575bffcf35571) — fix(roster): preserve multi-selection when deleting from a focused row
- 2026-10-29 [`cabd0f0`](https://github.com/KonradKrol/sj-sim/commit/cabd0f079063c8d2a071bd2e727b601aa418ed87) — fix(lab): show all control sections without horizontal scrolling and hide home entry
- 2026-10-29 [`140b304`](https://github.com/KonradKrol/sj-sim/commit/140b3047fc30866fe2f7d43f40526b71122fd93b) — feat(author): animate page entrance with reduced-motion support
- 2026-10-29 [`fbe542e`](https://github.com/KonradKrol/sj-sim/commit/fbe542e4f6e93bf21475dc1cb670a8bbfaee47fc) — fix(packs): clarify converted pack labels and keep migration notices out of competition setup
- 2026-10-29 [`d30b183`](https://github.com/KonradKrol/sj-sim/commit/d30b183648379f8252b66a6e3195af56ab21d633) — fix(competition): compare carried KO points with the current group leader
- 2026-10-29 [`873d16c`](https://github.com/KonradKrol/sj-sim/commit/873d16c3be65d8cb0efb77e797119de218b34efb) — fix(competition): accept nonempty rosters containing valid unnamed athlete profiles
- 2026-10-29 [`47a80e6`](https://github.com/KonradKrol/sj-sim/commit/47a80e6b94b83f71a63a7220a463a3dd0dbc2f72) — fix(weather): constrain simple manual controls to half-point steps
- 2026-10-29 [`1dfbd31`](https://github.com/KonradKrol/sj-sim/commit/1dfbd31f80e97b9fc937c64404aa2d6f5a55c2a0) — feat(setup): select hill packs directly within advanced hill editing
- 2026-10-29 [`58ccdb0`](https://github.com/KonradKrol/sj-sim/commit/58ccdb044f1b08b83ab1ece8a7b15f46955114d3) — docs: record UI principles, date decisions and the full active UX queue
- 2026-10-29 [`f299752`](https://github.com/KonradKrol/sj-sim/commit/f2997523b8ca7aef6322e3a5db8d6fdc402ede15) — fix(calendar): reject conflicting dates and ordering and show the next event date
- 2026-10-29 [`3c52b38`](https://github.com/KonradKrol/sj-sim/commit/3c52b38204edf91957a1b026ebc736c9296f16ef) — feat(calendar): clarify scheduling modes and add Friday-to-Sunday distribution
- 2026-10-29 [`903478c`](https://github.com/KonradKrol/sj-sim/commit/903478c72a0b61922ab1bdf85a740ac4c71425e7) — feat(weather): explain precipitation effects and illustrate intensity and changes
- 2026-10-29 [`a350ca2`](https://github.com/KonradKrol/sj-sim/commit/a350ca2f29b88590c49002514089f7c57e195921) — refactor(copy): use direct action labels for 22 checkbox options
- 2026-10-29 [`e0c879f`](https://github.com/KonradKrol/sj-sim/commit/e0c879f56bb369e4fa1487815f4fbd56ca2ae266) — fix(database): confirm country sorting before changing pack order
- 2026-10-29 [`88ed939`](https://github.com/KonradKrol/sj-sim/commit/88ed93934da36729692448fcdaeefb94dd947230) — feat(competition): make run conditions and jump settings clearly actionable
- 2026-10-29 [`af6d09d`](https://github.com/KonradKrol/sj-sim/commit/af6d09da35db99b09f5d4daf4f66c755a82134d6) — docs: record balance lab input, parameter copy and search follow-up tasks
- 2026-10-29 [`01d381e`](https://github.com/KonradKrol/sj-sim/commit/01d381e5d46fe4c36729171407c6ed2bb360df52) — fix(balance-lab): compute seven-contest previews only when explicitly shown
- 2026-10-29 [`1fcd4ff`](https://github.com/KonradKrol/sj-sim/commit/1fcd4ff57b9abc6a42c2a911e7022abc20b43e6b) — fix(balance-lab): preserve numeric drafts and search all simulation parameters
- 2026-10-29 [`0aa179e`](https://github.com/KonradKrol/sj-sim/commit/0aa179ec869f3ff54fd65b54b6f66e174f778623) — docs(balance-lab): explain concrete parameter effects instead of generic group text
- 2026-10-29 [`fd06925`](https://github.com/KonradKrol/sj-sim/commit/fd0692589aa438dbed1d9faffbbb68be74f31592) — feat(data): add editable 98-athlete men's 2026 pack with sources and default skills
- 2026-10-29 [`12162da`](https://github.com/KonradKrol/sj-sim/commit/12162dad932d8bdfb5bfd3b9e72d31e25b438ca3) — feat(balance-lab): show stage progress and estimated remaining experiment time
- 2026-10-29 [`d4b8471`](https://github.com/KonradKrol/sj-sim/commit/d4b8471982c2bf4787b80c295a054bfcc79eaa92) — fix(seasons): require the new simulation model when opening season editors
- 2026-10-29 [`b47e7e5`](https://github.com/KonradKrol/sj-sim/commit/b47e7e594fcbdef8c60f93070296dc4f75da16ee) — fix(ux): complete recent control translations and stabilize roster regression focus
- 2026-10-29 [`670d383`](https://github.com/KonradKrol/sj-sim/commit/670d3838d039134f72f3474f983b0d931d18df89) — docs(data): confirm seven athletes correctly have no personal-best record
- 2026-10-29 [`19a6973`](https://github.com/KonradKrol/sj-sim/commit/19a6973e67b8b76a7de42ba8c6f696d6974b58cc) — fix(athletes): expose deletion beside the list with Delete and one-click undo
- 2026-10-29 [`a500c9f`](https://github.com/KonradKrol/sj-sim/commit/a500c9fdb3e8b3332e047243d10035eabd38c7f2) — fix(athletes): keep slider previews live after numeric edits without extra undo steps
- 2026-10-29 [`eb98941`](https://github.com/KonradKrol/sj-sim/commit/eb989413f93904eb8fcbc7ca4ac9d60f957a56cf) — fix(athletes): reserve whole-editor fading for changing the selected athlete
- 2026-10-29 [`81631ee`](https://github.com/KonradKrol/sj-sim/commit/81631ee141936ca1f7cb388c4a5678229744154a) — feat(seasons): show five upcoming events with grouped training and clear next action
- 2026-10-29 [`1fc6533`](https://github.com/KonradKrol/sj-sim/commit/1fc6533c7bf846088132f403ca5428fe5bf03722) — feat(simulation): rescale perks to four strengths with explicit one-time data conversion
- 2026-10-29 [`6e4670e`](https://github.com/KonradKrol/sj-sim/commit/6e4670ed9038fad53547b5fc1b385442a679e416) — fix(migration): reject invalid old perk data and preserve already converted coefficient scales
- 2026-10-29 [`e2251ce`](https://github.com/KonradKrol/sj-sim/commit/e2251ce7204866d77bf8860a781d7dcdc4880dcb) — fix(migration): handle old frozen scenarios nested inside four-level experiments
- 2026-10-29 [`b6e8d1c`](https://github.com/KonradKrol/sj-sim/commit/b6e8d1c44fae4df8a45f0591ae940d8488b4951c) — feat(onboarding): launch an editable winter example season directly from welcome and home
- 2026-10-29 [`125ec44`](https://github.com/KonradKrol/sj-sim/commit/125ec44eb43b4d096474e383b335aff8465a5287) — fix(i18n): translate four-level perks, season overview and first-launch actions
- 2026-10-29 [`09323c5`](https://github.com/KonradKrol/sj-sim/commit/09323c5feb013b5183f6dcff0d5095b0025164a3) — fix(calendar): defer scoring changes until dropdown activation completes
- 2026-10-29 [`7f6fbe9`](https://github.com/KonradKrol/sj-sim/commit/7f6fbe9c3b6dfef52e956c17840d12c75f2e374d) — feat(ui): animate thirty navigation surfaces with reduced-motion support
- 2026-10-29 [`14e0d61`](https://github.com/KonradKrol/sj-sim/commit/14e0d61f5ffbf5c78dcca830c335d059cae94f5a) — docs: record completed perk rescaling, onboarding, lab fixes and animation verification
- 2026-10-29 [`1b1d090`](https://github.com/KonradKrol/sj-sim/commit/1b1d090fda0e7d787129525d742b1b6adf1a0e21) — fix(packs): upgrade three-level perks automatically without hiding edited packs
- 2026-10-30 [`870932a`](https://github.com/KonradKrol/sj-sim/commit/870932ad7065c2fa8d318d2c62a9961ebd02cdcd) — fix(balance-lab): give every parameter a concise explanation and concrete example
- 2026-10-30 [`1c6ac08`](https://github.com/KonradKrol/sj-sim/commit/1c6ac087d71f40d91be70db51fe32f794a4f402e) — fix(i18n): translate every laboratory explanation and detect missing source keys
- 2026-10-30 [`118907b`](https://github.com/KonradKrol/sj-sim/commit/118907b78af72c287ababed947593fb7039fa5f3) — fix(perks): fit strength controls to card width and distinguish middle tiers
- 2026-10-30 [`3e7d0a0`](https://github.com/KonradKrol/sj-sim/commit/3e7d0a0a49cbf18dfd4926b0739004d6b1aa1801) — fix(editors): save fields before deferring QML refresh beyond input callbacks
- 2026-10-30 [`dd60295`](https://github.com/KonradKrol/sj-sim/commit/dd602959337c2d7b0b20322848689e7a26ddeefa) — fix(qml): settle editor content heights without recursive binding evaluation
- 2026-10-30 [`94d349d`](https://github.com/KonradKrol/sj-sim/commit/94d349d8f292f570a54b258ace68b21330592f5d) — docs: identify completed layout fix and clarify UI-thread scheduling
- 2026-10-30 [`327cff5`](https://github.com/KonradKrol/sj-sim/commit/327cff5ece834205a3ff546698dc27ac8462ca9d) — docs: leave temporary October 30 regression and crash investigation queue
- 2026-10-30 [`b08a29e`](https://github.com/KonradKrol/sj-sim/commit/b08a29e9d03642eef9e413d7358cb4206dba4b14) — data: snapshot current athlete and hill packs for transfer
- 2026-10-30 [`a395c1a`](https://github.com/KonradKrol/sj-sim/commit/a395c1ae6a3c0f88347cb4b46d2293b06a1b8be5) — Retain studio controllers while database categories change
- 2026-10-30 [`b019b31`](https://github.com/KonradKrol/sj-sim/commit/b019b3129fac22ae1b8bac28a7cb98018f6ea31a) — Unify pack settings across database studios
- 2026-10-30 [`043998d`](https://github.com/KonradKrol/sj-sim/commit/043998d5237d0617d7b08b6810f226d76450325b) — Support persistent athlete reordering by drag and keyboard
- 2026-10-30 [`18e6476`](https://github.com/KonradKrol/sj-sim/commit/18e64764fbf246fadb96b1fc7c43271c4b06e3f7) — Add confirmed country sorting to athlete and hill libraries
- 2026-10-30 [`6161927`](https://github.com/KonradKrol/sj-sim/commit/61619275ffed6bc94c51b6ecd70934ec23da3aff) — Calculate tailwind compensation automatically
- 2026-10-30 [`745e1eb`](https://github.com/KonradKrol/sj-sim/commit/745e1ebec50be7bed5f5be046c17115ed9d97456) — Clarify calendar scheduling and warnings
- 2026-10-30 [`1d97fc8`](https://github.com/KonradKrol/sj-sim/commit/1d97fc83c7ae2bfaa56c5c590b301356db45e741) — Make season header compact and emphasize saving
- 2026-10-30 [`a4e0174`](https://github.com/KonradKrol/sj-sim/commit/a4e01747426ab4111730042703cc39ea7a0c2889) — Add per-save automatic season saving
- 2026-10-30 [`f167b71`](https://github.com/KonradKrol/sj-sim/commit/f167b71475010bf81d5a111a636c01bf98ecf6e5) — Add advisory country limits and grouped roster view
- 2026-10-31 [`87b7e4e`](https://github.com/KonradKrol/sj-sim/commit/87b7e4e75e53d17ca208edc53fe3f7997f28063e) — Checkpoint disabled active roster groundwork
- 2026-10-31 [`5ddacd2`](https://github.com/KonradKrol/sj-sim/commit/5ddacd2a0dea722818afa2c2b2d74c14852a5856) — Checkpoint Windows tests and October 31 handoff
- 2026-10-31 [`246335c`](https://github.com/KonradKrol/sj-sim/commit/246335c276ddd100dc9dba7bbd3fca7b839dc365) — docs: record roster redesign decisions
- 2026-10-31 [`016a82b`](https://github.com/KonradKrol/sj-sim/commit/016a82baeed3f614fa328e3ea3ce5c945315c07b) — docs: clarify roster selection behavior
- 2026-10-31 [`341c1d6`](https://github.com/KonradKrol/sj-sim/commit/341c1d670a8b22d57ec9e7b44909a1b7843aa485) — fix: use all athletes when active roster is empty
- 2026-10-31 [`bc06ed7`](https://github.com/KonradKrol/sj-sim/commit/bc06ed7a9b764bf5cb1ddee71ffde33ac2e244d0) — fix: remove obsolete importance controls and theme autosave label
- 2026-10-31 [`de330cb`](https://github.com/KonradKrol/sj-sim/commit/de330cbc90a6c1c684bbc09021b020662741ead7) — refactor: route competition entrants through guarded active roster
- 2026-10-31 [`5b8f938`](https://github.com/KonradKrol/sj-sim/commit/5b8f938dae69d96a943a32ba5a1a9cc4196034fc) — feat: introduce dedicated country-grouped roster workspace
- 2026-10-31 [`4f2bad1`](https://github.com/KonradKrol/sj-sim/commit/4f2bad1465955720e73dc280d12fcb9c2538fed2) — fix: give classification names full editor width
- 2026-10-31 [`a1a1227`](https://github.com/KonradKrol/sj-sim/commit/a1a1227a1bf8b175bbc6f5771f2d3ccafd33cf9a) — docs: record roster workspace progress and remaining integration
- 2026-10-31 [`9302b34`](https://github.com/KonradKrol/sj-sim/commit/9302b348e9302aa7d75ac55b075dba5e84d8e941) — feat: replace saved roster rows with batch selection and cross-panel drag
- 2026-10-31 [`394fa4a`](https://github.com/KonradKrol/sj-sim/commit/394fa4a381bbb627ea6eece1cfb523c9aa0dc2bb) — docs: track empty Classical KO field after qualifications
- 2026-10-31 [`792a044`](https://github.com/KonradKrol/sj-sim/commit/792a044aeabfd4a0eec02b745c709dd8e0f7c2ba) — fix: name race screens by training trial or qualification event
- 2026-10-31 [`b5350fd`](https://github.com/KonradKrol/sj-sim/commit/b5350fd34223f1528cb7915fefa2f63c522d961a) — feat: activate season rosters and persist per-calendar selection
- 2026-10-31 [`2bad9f0`](https://github.com/KonradKrol/sj-sim/commit/2bad9f0d22f6ce31464e17b28f51fb76c376e12e) — feat: offer ready season in a focused first-run welcome dialog
- 2026-10-31 [`00898fe`](https://github.com/KonradKrol/sj-sim/commit/00898fe1f1c65c74daffd8c78f8beffb981ec272) — docs: queue classification precision and remaining importance controls
- 2026-10-31 [`e6f0f9a`](https://github.com/KonradKrol/sj-sim/commit/e6f0f9a01a457958b9c87772f04222d2ce4d5ca8) — docs: preserve full 34-item owner queue in repository
- 2026-10-31 [`13a3bf6`](https://github.com/KonradKrol/sj-sim/commit/13a3bf60a610ee7ce3fd772abfd3935a1093b7a7) — fix: simplify season date and trial-round wording
- 2026-10-31 [`9e19ed3`](https://github.com/KonradKrol/sj-sim/commit/9e19ed3fa739fa4a0862e46a072893bcd515ac67) — fix: omit decimal places for integral classification columns
- 2026-10-31 [`075e259`](https://github.com/KonradKrol/sj-sim/commit/075e2592ab55ee2d804ca1afcf64f7a6f7f9e601) — ui: remove help action from main menu header
- 2026-10-31 [`60faf0d`](https://github.com/KonradKrol/sj-sim/commit/60faf0deb0813f1e3406f52f74cd0246bc5cd90e) — ui: simplify season rule label and hide technical save-size setting
- 2026-10-31 [`69bfe45`](https://github.com/KonradKrol/sj-sim/commit/69bfe45414d6c0e06547c4ee6a76d5971b4c1f15) — ui: merge contest options into preparation and remove importance inputs
- 2026-10-31 [`1471d97`](https://github.com/KonradKrol/sj-sim/commit/1471d97fb7e758949ccd65904597c70e77c5b272) — fix: defer roster membership updates until drag release completes
- 2026-10-31 [`9419fe2`](https://github.com/KonradKrol/sj-sim/commit/9419fe2a9d4a5e3321208487017d591ff6899a3c) — feat: automatically prepare squads with per-save gate and team preferences
- 2026-10-31 [`fdae948`](https://github.com/KonradKrol/sj-sim/commit/fdae948f4490fa750815d96f47825af4f5fd6004) — feat: reuse database athlete and hill libraries in season editing
- 2026-10-31 [`3d2f775`](https://github.com/KonradKrol/sj-sim/commit/3d2f775e89fd6cb3fb75bd171c359e6120b47b95) — feat: expose starter ordering and OVR sorting in one roster workspace
- 2026-10-31 [`dc100ea`](https://github.com/KonradKrol/sj-sim/commit/dc100ea3702b0d4a26d89a1cd6c56f54d7925ec3) — fix: resolve qualification athletes by identity and index result lookups
- 2026-10-31 [`9129007`](https://github.com/KonradKrol/sj-sim/commit/9129007f3f44bd1796d693b093b06f61aa65fd09) — perf: group team members in one pass while preserving starter order
- 2026-10-31 [`732bd0e`](https://github.com/KonradKrol/sj-sim/commit/732bd0e0a01fad40fae969123c10e9307c9599cf) — test: benchmark roster preparation against frozen original implementations
- 2026-10-31 [`5301838`](https://github.com/KonradKrol/sj-sim/commit/53018388862e287f6a6129132b0e947d92e628b3) — fix: preserve calendar list height on the smallest viewport
- 2026-10-31 [`fe664cd`](https://github.com/KonradKrol/sj-sim/commit/fe664cdec6ba5ac0ecbff91a6477f96a44ab3253) — fix: clarify empty rosters and retain active-list removal notice
- 2026-10-31 [`1bcb7fc`](https://github.com/KonradKrol/sj-sim/commit/1bcb7fc0495a5dee96cdcb9d906a472d849fef7f) — fix: retain active roster when a calendar assignment no longer exists
- 2026-10-31 [`dd786dd`](https://github.com/KonradKrol/sj-sim/commit/dd786ddcfdaa2c1647a0dd0dea5a89c344d44b47) — fix: retain profile values during squad ranking and support Qt6 container sizes
- 2026-10-31 [`e876687`](https://github.com/KonradKrol/sj-sim/commit/e8766876a58e7473f1f9d3191c6cd29909619697) — docs: complete 34-item queue and document validation and remaining diagnostic limits
- 2026-10-31 [`50aad3c`](https://github.com/KonradKrol/sj-sim/commit/50aad3cdfbc2bfae0937be710d3f127359fa78a7) — fix: bind legacy wind field delegates to explicit model data
- 2026-10-31 [`095549e`](https://github.com/KonradKrol/sj-sim/commit/095549e56c3563e0b6f090e086564d8a9dc1403f) — fix: render dropdown options with stable popup sizing and model-aware labels
- 2026-10-31 [`9d598e3`](https://github.com/KonradKrol/sj-sim/commit/9d598e332e052bd3ef82a9f3241a030232749c31) — fix: isolate roster country views to prevent Quick section crashes
- 2026-10-31 [`6974812`](https://github.com/KonradKrol/sj-sim/commit/6974812bbdf12d6581b712f733d9626be7a72378) — fix: publish roster insertion and removal only after data mutation
- 2026-10-31 [`b5a8b28`](https://github.com/KonradKrol/sj-sim/commit/b5a8b280c373ccba521fd2032da48aa49013f49d) — test: cover roster publication and repeated country-view switching
- 2026-10-31 [`39463ea`](https://github.com/KonradKrol/sj-sim/commit/39463ea09f22bcef4314827369a1fec27a483a80) — wip: preserve inline roster redesign and detailed continuation handoff
- 2026-10-31 [`08ecdbd`](https://github.com/KonradKrol/sj-sim/commit/08ecdbdecfea64bf9b6d7f5f17b74c15cea9c456) — docs: preserve and rename owner work queue
- 2026-10-31 [`4685016`](https://github.com/KonradKrol/sj-sim/commit/4685016de8175d3dbd4c13587f2aab2045a14a5e) — test: cover inline roster persistence and translate roster controls
- 2026-10-31 [`b878f1f`](https://github.com/KonradKrol/sj-sim/commit/b878f1ff2d3b51727c998b80b45160adb0ebe726) — fix: ignore transient non-hill entries when switching libraries
- 2026-10-31 [`e463b51`](https://github.com/KonradKrol/sj-sim/commit/e463b51a51d57cdbeefbd806a425e325d18f56d2) — fix: validate only starters when checking team completeness
- 2026-10-31 [`e791da3`](https://github.com/KonradKrol/sj-sim/commit/e791da3c99f6c2fc06112d86b59294498812636b) — Give new athletes localized default names
- 2026-10-31 [`140180c`](https://github.com/KonradKrol/sj-sim/commit/140180ce3853a1b6e2e0bfe30a36ed037bd0aef9) — Avoid size binding loops in database dialogs
- 2026-10-31 [`2a225d9`](https://github.com/KonradKrol/sj-sim/commit/2a225d9fc7aaf3b24818f90174d09f925e48aedd) — Make season roster editing usable on small screens
- 2026-10-31 [`d7c8b83`](https://github.com/KonradKrol/sj-sim/commit/d7c8b835c95ceedad29e347b9c7e9d46a9577039) — Handle sorting by an unplayed classification safely
- 2026-10-31 [`f75f533`](https://github.com/KonradKrol/sj-sim/commit/f75f5335ca41203155996549608348a8de85bd06) — Keep pack migration notices out of competition setup
- 2026-10-31 [`71a3d41`](https://github.com/KonradKrol/sj-sim/commit/71a3d41430abf68e6d16621ea52acf75edf76891) — Keep wind field preview stable when editing a station
- 2026-10-31 [`067e2b4`](https://github.com/KonradKrol/sj-sim/commit/067e2b44da35486b4564e87fac93c231c0227ff9) — Simplify team preparation controls and expand the table
- 2026-10-31 [`7140092`](https://github.com/KonradKrol/sj-sim/commit/7140092b88a16e3f174ed40b60ed8f7b31e0c13c) — Keep light perks visually subtle
- 2026-10-31 [`1fda89d`](https://github.com/KonradKrol/sj-sim/commit/1fda89d1a34bba18118cea4e9c80a0d6761b1553) — Hide the legacy manual form generator from season navigation
- 2026-10-31 [`a401ebe`](https://github.com/KonradKrol/sj-sim/commit/a401ebe7463414809918966e8bef8f4f71aa2b98) — Distinguish country sorting from grouping on start lists
- 2026-10-31 [`cd94942`](https://github.com/KonradKrol/sj-sim/commit/cd94942dad107e8dec202ffb90712bc67eb4d4fc) — Make flight specialists respond differently to wind
- 2026-10-31 [`75755fd`](https://github.com/KonradKrol/sj-sim/commit/75755fd9a4be70a038473d03b162311cfbfbf5f8) — Move competition data packs into a secondary section
- 2026-10-31 [`8434438`](https://github.com/KonradKrol/sj-sim/commit/8434438acab4217dc71f22193f8e99b171ae6fc5) — Exclude unnamed or duplicate athletes from team lineups
- 2026-10-31 [`af834dc`](https://github.com/KonradKrol/sj-sim/commit/af834dc313fa7260e4e83b2a60678c1609cac5b6) — Exercise inline roster persistence and small screen layouts
- 2026-10-31 [`d54980c`](https://github.com/KonradKrol/sj-sim/commit/d54980c3e76d510e790b0baf74dd46f08098c75f) — Stress inline roster grouping with a large athlete library
- 2026-10-31 [`e955acd`](https://github.com/KonradKrol/sj-sim/commit/e955acd8624f86d7d43f9e49f542947b60341afb) — Test dragging between inline athlete roster panes
- 2026-10-31 [`40e7e90`](https://github.com/KonradKrol/sj-sim/commit/40e7e90df46a237670ea8b3a023c119a65fd702c) — Wait for welcome dismissal in author navigation test
- 2026-11-01 [`936f59b`](https://github.com/KonradKrol/sj-sim/commit/936f59b0d59ba040978188d3aa676731df854668) — Avoid roster layout loops at larger interface scales
- 2026-11-01 [`03de742`](https://github.com/KonradKrol/sj-sim/commit/03de742a0768a0bbfeaa83a4af67577221542de5) — Cover roster renaming while switching lists
- 2026-11-01 [`600816d`](https://github.com/KonradKrol/sj-sim/commit/600816d9697feba9e86386a96c8799215cc2fd30) — Cover roster undo with calendar reassignment
- 2026-11-01 [`2767678`](https://github.com/KonradKrol/sj-sim/commit/2767678e73ce6ba72fcfaf8f2c8d9b9d3560f1a0) — Cover filtered roster selection during regrouping and resize
- 2026-11-01 [`f24f369`](https://github.com/KonradKrol/sj-sim/commit/f24f369b17cdeb88ea7283edd47af84370ac3538) — Update current task progress and validation notes
- 2026-11-01 [`8b65a94`](https://github.com/KonradKrol/sj-sim/commit/8b65a94577a60f882482ddfeb38ab81408e371db) — Guard empty characteristic choices and remove diagnostic spam
- 2026-11-02 [`b23ce6a`](https://github.com/KonradKrol/sj-sim/commit/b23ce6acf2ff6ca701d7607beb2d5db4b55b63c0) — Stabilize season tools regression after native dialog cancellation
- 2026-11-02 [`5d836ea`](https://github.com/KonradKrol/sj-sim/commit/5d836ea63257db33a099e7aaade983e761ad8ecb) — Respect active language in author regression assertions
- 2026-11-02 [`319c72b`](https://github.com/KonradKrol/sj-sim/commit/319c72b3969a8840b83bb1f257c059856fbd23e1) — Record successful full Quick UI regression run
- 2026-11-02 [`5a7d15d`](https://github.com/KonradKrol/sj-sim/commit/5a7d15d4bd55dd3ea83a0a9cfad7242ec409a87d) — Scale large-hill distance spread smoothly with tunable controls
- 2026-11-02 [`8cc1d9e`](https://github.com/KonradKrol/sj-sim/commit/8cc1d9ecdc69f84e3d4a965e809fe7055f5aaf06) — Keep key competition results visible
- 2026-11-02 [`22d62c3`](https://github.com/KonradKrol/sj-sim/commit/22d62c30d2e70d3ff7b4d527a70984a167c22143) — Refresh competition target lines after adjustments
- 2026-11-02 [`644b34c`](https://github.com/KonradKrol/sj-sim/commit/644b34c08bed14b8ddf4841bc776fdb3b4bf7a9b) — Simplify wind variability controls
- 2026-11-02 [`da3d897`](https://github.com/KonradKrol/sj-sim/commit/da3d897960f97f104b8b53f6f4e137eb4324377e) — Tune gate, judging and weather calibration
- 2026-11-02 [`4753884`](https://github.com/KonradKrol/sj-sim/commit/475388453b52a3f1d520fb19b7a6b42b4ef60ecd) — Support portable beta balance profiles
- 2026-11-02 [`aa5ca91`](https://github.com/KonradKrol/sj-sim/commit/aa5ca9111dc47929c538a0341da0c55c87948638) — Translate new balance laboratory controls
- 2026-11-02 [`bbebd2f`](https://github.com/KonradKrol/sj-sim/commit/bbebd2fcd3ecb61c10c0ca51380f199c0cabd997) — Scale wind differences across hill positions
- 2026-11-02 [`504db69`](https://github.com/KonradKrol/sj-sim/commit/504db6949c8416dcb355a626baa46e7a8a56c046) — Explain new balance controls with examples
- 2026-11-02 [`f879c54`](https://github.com/KonradKrol/sj-sim/commit/f879c544e85e461c97846b1aa09919b6a9066753) — Make sharing studio packs clearer
- 2026-11-02 [`9affc5e`](https://github.com/KonradKrol/sj-sim/commit/9affc5e5295ae94e7ace20ba31f9777a7bc26768) — Offer clear disqualification frequency choices
- 2026-11-02 [`1b4e1da`](https://github.com/KonradKrol/sj-sim/commit/1b4e1da8ec3acc3edb31496419785dd8a78ed344) — Explain jumper rating and form
- 2026-11-02 [`84fca8f`](https://github.com/KonradKrol/sj-sim/commit/84fca8fb99cd785a17826ddc2809494ad3cb8d48) — Choose replacements for missing preset hills
- 2026-11-02 [`2c49d68`](https://github.com/KonradKrol/sj-sim/commit/2c49d68ed28b4bdce20eed7e825d19986cb03678) — Polish disqualification frequency selector
- 2026-11-02 [`63c193b`](https://github.com/KonradKrol/sj-sim/commit/63c193bdf398682c2058dcb56d7addc0d2952138) — Clarify wind settings hierarchy
- 2026-11-02 [`d3b57f7`](https://github.com/KonradKrol/sj-sim/commit/d3b57f7c40850bb5c4fd0f415455b1ba9fb0423b) — Show frequency level in DSQ selector
- 2026-11-02 [`267e412`](https://github.com/KonradKrol/sj-sim/commit/267e412c1afeac8c496c82fdf0d46e97a3add818) — Clarify wind measurement method descriptions
- 2026-11-02 [`7f60664`](https://github.com/KonradKrol/sj-sim/commit/7f6066414143ff0d32afebcde210cd5c1b6d792d) — Bundle current athlete and hill packs in beta releases
- 2026-11-02 [`8906319`](https://github.com/KonradKrol/sj-sim/commit/89063194ab41e65386a62eb9ce8217f771924c31) — Hide obsolete legacy settings in Quick UI
- 2026-11-02 [`07beb76`](https://github.com/KonradKrol/sj-sim/commit/07beb76a044cd9692d7c1159432010f41763e293) — Remove webhook settings from Quick interface
- 2026-11-02 [`052f26a`](https://github.com/KonradKrol/sj-sim/commit/052f26a4f2d120810c6284298db21e18c5daca01) — Color landing types in jump card
- 2026-11-02 [`1c806d6`](https://github.com/KonradKrol/sj-sim/commit/1c806d609d013bbef54a6f4b746a14833e6a7d49) — Use Main balance profile as built-in defaults
- 2026-11-02 [`58c3441`](https://github.com/KonradKrol/sj-sim/commit/58c34419319670c5009f4da19eb3ac2f81f2c790) — Show jump distance before points
- 2026-11-02 [`c93a4a7`](https://github.com/KonradKrol/sj-sim/commit/c93a4a7faad291116f0327263f5158924dc78833) — Add pack rule defaults and bulk qualification planning
- 2026-11-02 [`acabaff`](https://github.com/KonradKrol/sj-sim/commit/acabaffd90077709c815da7f80c6a64d86e5e9bb) — Show team previews for individual trial rounds
- 2026-11-02 [`b05f595`](https://github.com/KonradKrol/sj-sim/commit/b05f595c4e72ffb28a28e98fd44115b49dd34349) — Expose pack import and export in database studios
- 2026-11-02 [`a7ad6b0`](https://github.com/KonradKrol/sj-sim/commit/a7ad6b010792c469592f88e5a75f4d96873bfcf3) — Document ten production stability and compatibility risks
- 2026-11-02 [`d3b7a73`](https://github.com/KonradKrol/sj-sim/commit/d3b7a73959f9fdc862e56c5278b44682bfb7187f) — Keep competition total points after all round results
- 2026-11-02 [`35a02c6`](https://github.com/KonradKrol/sj-sim/commit/35a02c69bb44f2e669bf5c075ec430eb9b465092) — Initialize application settings safely before loading
- 2026-11-02 [`eed4010`](https://github.com/KonradKrol/sj-sim/commit/eed40105cc44913d3e93062d4739acbbc3113725) — Reject duplicate and conflicting UUIDs before loading saves
- 2026-11-02 [`839c089`](https://github.com/KonradKrol/sj-sim/commit/839c08922c0f429fa18e15562aa256580afe22b8) — Write settings and database JSON atomically with checked errors
- 2026-11-02 [`58595b2`](https://github.com/KonradKrol/sj-sim/commit/58595b23e7598f28c69739a64aace462dce36bc3) — Continue independent database writes after individual failures
- 2026-11-02 [`c43b1b4`](https://github.com/KonradKrol/sj-sim/commit/c43b1b40cde098ebf012700c2e7c036e229e5e36) — Validate calendar presets while preserving legacy compatibility
- 2026-11-04 [`e78475e`](https://github.com/KonradKrol/sj-sim/commit/e78475e282de3eb1ae07e51f7572288c8afc915b) — Complete season planning, squad locks and database editing workflows
- 2026-11-04 [`e207c93`](https://github.com/KonradKrol/sj-sim/commit/e207c93ffb76991da8f168714eef61f412a01dcb) — Reject damaged save references and balance snapshots; lock GUI data writes
- 2026-11-04 [`dc0911f`](https://github.com/KonradKrol/sj-sim/commit/dc0911f024e8e2ed533cb043648f49ec522ed242) — Replace README with concise Polish guide and current UI screenshots
- 2026-11-04 [`f648321`](https://github.com/KonradKrol/sj-sim/commit/f648321bfc14aead70a8fe69a839197c2cd919ac) — Protect shared authoring profiles across separate application copies
- 2026-11-04 [`23dfed9`](https://github.com/KonradKrol/sj-sim/commit/23dfed90d806a7ef1545b6690b7f98ec6194546b) — Prepare locally verified Windows and Linux deployment workflow
- 2026-11-04 [`63cf3b1`](https://github.com/KonradKrol/sj-sim/commit/63cf3b105786ed568b7d7bc3c1035fe886c712f5) — Document ten outstanding production risks with isolated reproductions
- 2026-11-04 [`df430e4`](https://github.com/KonradKrol/sj-sim/commit/df430e489e6d4324f588d8b9257e6a0033214133) — Fix versioned QML imports for Qt 5 Linux runtime
- 2026-11-04 [`9d9cf5e`](https://github.com/KonradKrol/sj-sim/commit/9d9cf5e68ea088d4fa705e8c8f3a33ead06e7c59) — Strengthen tailwind penalties and adverse wind reactions
- 2026-11-04 [`9732c07`](https://github.com/KonradKrol/sj-sim/commit/9732c07efbe18315a388d627773a6436e36d1185) — Derive hill scoring automatically and clarify hill editor actions
- 2026-11-04 [`b5b8964`](https://github.com/KonradKrol/sj-sim/commit/b5b8964d2c52c4d5ba9fa07f0fca1f66e86fb534) — Keep visible hill action backgrounds with copy and red trash icons
- 2026-11-04 [`f48d7d2`](https://github.com/KonradKrol/sj-sim/commit/f48d7d2a35f02900f53ebb69e6b3d393ff3115da) — Support drag reordering in hills rules and points catalogs
- 2026-11-04 [`da63b54`](https://github.com/KonradKrol/sj-sim/commit/da63b54e991146cac9717aa64b964e5f24c79591) — Keep lab game baseline synchronized with active runtime balance
- 2026-11-04 [`cacffa2`](https://github.com/KonradKrol/sj-sim/commit/cacffa2d6f0fc30e78e877fe6a4697544844acd1) — Use real HS as automatic gate target
- 2026-11-04 [`faae535`](https://github.com/KonradKrol/sj-sim/commit/faae535e387cf3f8786ccfaaf4b3b039765580e6) — Calibrate hill-dependent wind strength and soften landing geometry
- 2026-11-04 [`f82a109`](https://github.com/KonradKrol/sj-sim/commit/f82a1094893ce5f5cbb40d64352efbdd09ddcd26) — Calibrate landing styles and large hill spread
- 2026-11-04 [`a9a25c7`](https://github.com/KonradKrol/sj-sim/commit/a9a25c7277506c4a1b0a42c98375d52bf13c1eb4) — Set specialist wind sensitivity to 0.275 safely
- 2026-11-04 [`d6e173c`](https://github.com/KonradKrol/sj-sim/commit/d6e173c648e43de27af0455ba335d42312de9abc) — Correct specialist wind coefficient to 0.0275
- 2026-11-04 [`c0e96b1`](https://github.com/KonradKrol/sj-sim/commit/c0e96b113878995dfc9ae8ddf050c4a4c4e5064e) — Soften preset and automatic wind generation
- 2026-11-05 [`a73a7b7`](https://github.com/KonradKrol/sj-sim/commit/a73a7b74b84d363965b86233d3f1d6765604e3d1) — Add attributed transparent athlete portraits to pack transfer
- 2026-11-05 [`2d2fcc5`](https://github.com/KonradKrol/sj-sim/commit/2d2fcc5af9bd07c0ba34fb42b2d9110afc21c9ba) — Show latest podium and clear preparation status in season overview
- 2026-11-05 [`e3240e7`](https://github.com/KonradKrol/sj-sim/commit/e3240e7823fe8754b072a61998f321504d61fb54) — Explain season and database controls with contextual help
- 2026-11-05 [`6e95158`](https://github.com/KonradKrol/sj-sim/commit/6e95158de0223d90189173a2329916665c2fb0e9) — Reject unsafe preset file names and record roster board requirements
- 2026-11-05 [`e943b73`](https://github.com/KonradKrol/sj-sim/commit/e943b73c9f04d92d9283ae5b243de581df12695c) — Add shared roster assignment board with explicit transactional apply
- 2026-11-05 [`d5cccd4`](https://github.com/KonradKrol/sj-sim/commit/d5cccd4aa5bb35959a5e2b8f1d89b23cf25a55a8) — Reject malformed save collections before graph deserialization
- 2026-11-05 [`e2f1f9e`](https://github.com/KonradKrol/sj-sim/commit/e2f1f9e2a9946a50ab7d2dcf955002fda3e122f0) — Make season athlete and hill row removals obey Qt model contracts
- 2026-11-05 [`ab750b2`](https://github.com/KonradKrol/sj-sim/commit/ab750b2f9052a6cb5b02ba8ddbaa87389fc13929) — Validate season calendar ownership and calendar-local dependencies
- 2026-11-05 [`7315a8f`](https://github.com/KonradKrol/sj-sim/commit/7315a8f7f7c32dc1a2ac0a54d35656df62a4fd72) — Protect unreadable balance workspaces until explicit backed-up recovery
- 2026-11-05 [`95c12c6`](https://github.com/KonradKrol/sj-sim/commit/95c12c618954a4af76e245aef1ff943f05f8f346) — Retain dirty season revisions and retry failed autosaves
- 2026-11-05 [`1ce38bb`](https://github.com/KonradKrol/sj-sim/commit/1ce38bb9fbcee3e9ac5cdcbb01da48abc422744e) — Report corrupt active balance profiles and expose explicit recovery
- 2026-11-05 [`dafe4fb`](https://github.com/KonradKrol/sj-sim/commit/dafe4fbcbcb0c33d13eb1d5f2ba602847e8dfd3f) — Explain rejected preset names and add path traversal regressions
- 2026-11-05 [`0664eda`](https://github.com/KonradKrol/sj-sim/commit/0664edaeda38f4a3a83402110a708c4ddf53e756) — Index save files lazily and bound full season reads
- 2026-11-05 [`c8ca34f`](https://github.com/KonradKrol/sj-sim/commit/c8ca34f2a3c150ebdcb85047c7799915eb381f5b) — Guard ongoing competitions with an explicit simulation algorithm revision
- 2026-11-05 [`4a122a8`](https://github.com/KonradKrol/sj-sim/commit/4a122a870b229a05340577f12cd04ca248800593) — Own classification heap rows safely across snapshots and release retired calendars
- 2026-11-05 [`ab54589`](https://github.com/KonradKrol/sj-sim/commit/ab54589f3f7a7ba15432ab528fb7a9542902c544) — Simplify calendar navigation and defer default-rule edits until popup closes
- 2026-11-05 [`3ba20f5`](https://github.com/KonradKrol/sj-sim/commit/3ba20f5a7221df3dc24dcfe4dc8237739efddc4d) — Keep roster country destinations consistent and document production fixes
- 2026-11-05 [`b791d79`](https://github.com/KonradKrol/sj-sim/commit/b791d7984134f50b7f813f8a6459144418d23b0c) — Apply calendar rule defaults whenever event type changes
- 2026-11-05 [`2e0a5ad`](https://github.com/KonradKrol/sj-sim/commit/2e0a5adc6ccae53448c987ad7caa9d2814c447a9) — Transfer roster athletes directly by tile click and fill the selected pane
- 2026-11-05 [`d56b185`](https://github.com/KonradKrol/sj-sim/commit/d56b1853b9e52657bac9e119947f0c4718d7d42f) — Keep calendar events visible beside planning and move defaults to settings
- 2026-11-05 [`98c0ed6`](https://github.com/KonradKrol/sj-sim/commit/98c0ed6ed837dc16c3eed5f18bf4d18a09a18b9a) — Use factual empty-day text in calendar overview
- 2026-11-05 [`d7a5991`](https://github.com/KonradKrol/sj-sim/commit/d7a599187a45b910b3147313ef9598ce04725103) — Size the season footer to its content and align its action consistently
- 2026-11-05 [`7918824`](https://github.com/KonradKrol/sj-sim/commit/7918824fc75d19da0b430a46e7c8a72ed8a98f53) — Place calendar planning beside event tabs as a separate action
- 2026-11-05 [`8ab196d`](https://github.com/KonradKrol/sj-sim/commit/8ab196d4c03db374ea3deea2f58087ff6155ab65) — Reduce generated wind by twenty percent relative to the previous baseline
- 2026-11-05 [`cc2874f`](https://github.com/KonradKrol/sj-sim/commit/cc2874f93d80eed3246aa27e7804c0a0893c88a7) — Increase generated wind strength five percent above original baseline
- 2026-11-05 [`c975a8a`](https://github.com/KonradKrol/sj-sim/commit/c975a8acc1d1cc708a5d14a54b08cbbf1daaf8f2) — Set generated slope wind differences to ninety-five percent of baseline
- 2026-11-05 [`f5ae2eb`](https://github.com/KonradKrol/sj-sim/commit/f5ae2eb44dc0d71eb94e54cbb73e2e744ed480f7) — Fix race controls final property collision and weather layout height loop
- 2026-11-05 [`9e019a2`](https://github.com/KonradKrol/sj-sim/commit/9e019a2647527bd1f8b1cd06d2117a9fc909f46a) — Reduce large hill distance spread gain by thirty percent
- 2026-11-05 [`9c9bb5b`](https://github.com/KonradKrol/sj-sim/commit/9c9bb5bdedda15c89602d8f33eb038c590781a57) — Fix preset filename validation rejecting ordinary names
- 2026-11-05 [`dcb0fba`](https://github.com/KonradKrol/sj-sim/commit/dcb0fba3f3d7cc7e542db0dd15a224304139ee22) — Tune headwind variability and increase overall weight
- 2026-11-09 [`cca4d0e`](https://github.com/KonradKrol/sj-sim/commit/cca4d0e3de2ba40b97e507b336ba400040c96750) — Make conserving energy penalty mostly intermittent
- 2026-11-09 [`147be42`](https://github.com/KonradKrol/sj-sim/commit/147be42086450bafd9a5c22dbc8aa0095e8f4cca) — Use continuous distribution for conserving effort penalty
- 2026-11-09 [`413c2df`](https://github.com/KonradKrol/sj-sim/commit/413c2df792626deefc712bf95be8ea8a7562d245) — Randomize training specialist bonus and remove competition penalty
- 2026-11-09 [`0d61688`](https://github.com/KonradKrol/sj-sim/commit/0d616888b77caf65441b21083625dce7649f8c93) — Keep next athlete card compact between rounds
- 2026-11-09 [`3aacce3`](https://github.com/KonradKrol/sj-sim/commit/3aacce3ea39eda668f3d661413ff4712068e855a) — Support event cancellation and recorded wind display
- 2026-11-09 [`0ec29d3`](https://github.com/KonradKrol/sj-sim/commit/0ec29d337f1ba226104b68dc963e9a0936fcad9f) — Record completion of November competition work
- 2026-11-09 [`b55e41e`](https://github.com/KonradKrol/sj-sim/commit/b55e41e718a073727dacee36f47c88e90c75ae8b) — Restore authoring pack files when database changes are discarded
- 2026-11-09 [`ffac8fe`](https://github.com/KonradKrol/sj-sim/commit/ffac8fe6a9c82804525ac5d626332c726d113edb) — Refine judging variation and landing penalty distributions
- 2026-11-09 [`117c3f4`](https://github.com/KonradKrol/sj-sim/commit/117c3f4051e2ced1d8215cd3b3c0c296b1d64028) — Allow precise wind strength entry with half-unit buttons
- 2026-11-09 [`c8b70f9`](https://github.com/KonradKrol/sj-sim/commit/c8b70f9f824dde77197f657c8dd6b17c0d2914b8) — Soften high temporal wind variability settings
- 2026-11-09 [`20dc561`](https://github.com/KonradKrol/sj-sim/commit/20dc56110c47f6c512e76cb741052832a90d2e29) — Expose wind forecast alongside recorded jump sectors
- 2026-11-09 [`5f48b3a`](https://github.com/KonradKrol/sj-sim/commit/5f48b3a80b32094d4e393a09f6bf673742354d39) — Simplify wind measurement to three explained methods
- 2026-11-09 [`40aa5d7`](https://github.com/KonradKrol/sj-sim/commit/40aa5d70d1ce756e29824be0741314af115b0482) — Increase baseline tailwind effect by ten percent
- 2026-11-09 [`ac6ee17`](https://github.com/KonradKrol/sj-sim/commit/ac6ee1753aad154b9221bae728c5ababe3ba8975) — Further soften upper temporal variability amplitudes
- 2026-11-09 [`f6b0231`](https://github.com/KonradKrol/sj-sim/commit/f6b02311c652943359554cadb63c5fb73a1f0fd1) — Guard start-list ordering against empty or missing ranking data
- 2026-11-09 [`ec520f6`](https://github.com/KonradKrol/sj-sim/commit/ec520f666b109560203190eafd122dce5bde5c7d) — Add reversible gate suggestions and safer selection dialogs
- 2026-11-09 [`a19ce85`](https://github.com/KonradKrol/sj-sim/commit/a19ce85610c9ce89cc21a256d25cc6bed166cd75) — Set tailwind increase to five percent above previous baseline
- 2026-11-09 [`6b0c650`](https://github.com/KonradKrol/sj-sim/commit/6b0c650faccec18d048fc8da2fd17c371fe6d81d) — Replace wind popups with a compact before-and-after chart
- 2026-11-09 [`af19264`](https://github.com/KonradKrol/sj-sim/commit/af192645312d5d4251a2b79cf66e5a9c6426a823) — Reduce hill weather perk influence by thirty percent
- 2026-11-09 [`6e8efa8`](https://github.com/KonradKrol/sj-sim/commit/6e8efa84810be63f26c57a257d68601062798780) — Protect numeric drafts from refreshes and defer validation until commit
- 2026-11-09 [`a72d1c9`](https://github.com/KonradKrol/sj-sim/commit/a72d1c9caf7717169ab517398067b6779a819ef8) — Use the competition setup wind slope view for before-and-after previews
- 2026-11-09 [`5af2823`](https://github.com/KonradKrol/sj-sim/commit/5af28234ff1b25353c9dffff38c30261eb09f661) — Unify athlete and hill editor action bars
- 2026-11-09 [`acc9dbc`](https://github.com/KonradKrol/sj-sim/commit/acc9dbccf92a9d35b7ced3fbfff4f360317c3aab) — Make wind forecast a movable tool panel and separate recorded wind access
- 2026-11-09 [`5abb16d`](https://github.com/KonradKrol/sj-sim/commit/5abb16d0eb16a9d5a29393d4765e631e2945e3e0) — Compact wind forecast and consolidate competition actions into top menu
- 2026-11-09 [`cc49549`](https://github.com/KonradKrol/sj-sim/commit/cc4954937f3eaee4be99166086d7eec04159209b) — Replace conditions text button with a weather settings icon
- 2026-11-09 [`e189655`](https://github.com/KonradKrol/sj-sim/commit/e18965585052bc69c28ebfb9d8e252fd538b6348) — Replace jump options with compact competition toolbar actions
- 2026-11-09 [`046d1e8`](https://github.com/KonradKrol/sj-sim/commit/046d1e8bd0b5b542a1dab116397616476a522e8d) — Keep season header height tied to compact content
- 2026-11-09 [`4de6a89`](https://github.com/KonradKrol/sj-sim/commit/4de6a896ce29fdffa48cf94adb2fbbcdc0c0db39) — Fix race screen syntax and weather workspace sizing loop
- 2026-11-09 [`69d1144`](https://github.com/KonradKrol/sj-sim/commit/69d114441b305214e9dd415f71727a37b0428f1b) — Compact competition gate controls and refresh season layout after scaling
- 2026-11-09 [`c400456`](https://github.com/KonradKrol/sj-sim/commit/c400456bd5a486c908c88690b53b66c71d1e16e0) — Stabilize competition controls and simplify automatic gate adjustment
- 2026-11-09 [`0d5269b`](https://github.com/KonradKrol/sj-sim/commit/0d5269ba85bc4d5c85f6f4e281848040a7ccfe46) — Fix QML layout errors and restore clear toolbar button boundaries
- 2026-11-09 [`049063e`](https://github.com/KonradKrol/sj-sim/commit/049063e8556dc18ac7abadf32624de71c82a4cd1) — Avoid duplicate DNS label in jump card footer
- 2026-11-09 [`46f7524`](https://github.com/KonradKrol/sj-sim/commit/46f75249c837bf0fff3d41fe5411f7241f5e044f) — Add five simplified simulation preferences with responsive settings UI
- 2026-11-09 [`0b21513`](https://github.com/KonradKrol/sj-sim/commit/0b21513b322bb664544ce4d222aae91a72e083cc) — Support bulk simulation save deletion and simplify save browser
- 2026-11-09 [`feba23b`](https://github.com/KonradKrol/sj-sim/commit/feba23b9f295314ce16238d1a566ac96bc579fff) — Add thirty contextual help points and explain balance laboratory
- 2026-11-09 [`45472f6`](https://github.com/KonradKrol/sj-sim/commit/45472f6c976eca9875cff1c56c0ba863a710e63b) — Add pink and clean colour themes with light and dark variants
- 2026-11-09 [`5b6d14a`](https://github.com/KonradKrol/sj-sim/commit/5b6d14a2f21262e091c05f9935003f8dca89903a) — Give themes distinct shapes typography and interaction treatments
- 2026-11-10 [`cefcf7a`](https://github.com/KonradKrol/sj-sim/commit/cefcf7a8f3d6d3fb8ffb122034912276f62eb4c8) — Simplify upcoming events and improve calendar planning controls
- 2026-11-10 [`f15f151`](https://github.com/KonradKrol/sj-sim/commit/f15f15103e0b9511ac603157b43dd71bd72fd421) — Respect jump information preferences and add configurable live presentation and autoplay
- 2026-11-10 [`e3a91df`](https://github.com/KonradKrol/sj-sim/commit/e3a91df0d3625aa02a61f6155069c0deadd53517) — Restore planning navigation and simplify only qualification day selection
- 2026-11-10 [`d4069d9`](https://github.com/KonradKrol/sj-sim/commit/d4069d97a7196a8fa8d9c6850f56fbbf040c98e0) — Refine live timing season overview calendar states and remove webhook UI
- 2026-11-10 [`912b220`](https://github.com/KonradKrol/sj-sim/commit/912b2201a086a21cc5b9e9555ad53346a0f037c8) — Fix season header and tool button sizing across UI scales
- 2026-11-10 [`9834437`](https://github.com/KonradKrol/sj-sim/commit/983443730ca3587c2e89578bc5862d7cb331932c) — Connect season overview cancellation and cover real overview actions
- 2026-11-10 [`e4c97cb`](https://github.com/KonradKrol/sj-sim/commit/e4c97cb2660473e3fedbec1fc3e0da861b4b75fe) — Pause automatic jumps before the live settings opening animation
- 2026-11-10 [`2e94ea0`](https://github.com/KonradKrol/sj-sim/commit/2e94ea0c4da5be6d68881a7a7407b5e4a22ea064) — Avoid theme selector height feedback at large text sizes
- 2026-11-10 [`e23fabe`](https://github.com/KonradKrol/sj-sim/commit/e23fabe315c65900753c450d6e7bf3a0a063610a) — Add deterministic daily form engine with Markov regimes and persisted state
- 2026-11-10 [`adf0d20`](https://github.com/KonradKrol/sj-sim/commit/adf0d203dc4a222ab8b32412f48deffc797c2153) — Refresh wind forecast label when advancing competition rounds
- 2026-11-10 [`eaf0d98`](https://github.com/KonradKrol/sj-sim/commit/eaf0d98ce33abac0478ec28b585822243e7923a1) — Remove duplicated language live reveal and DSQ controls from general settings
- 2026-11-10 [`6194344`](https://github.com/KonradKrol/sj-sim/commit/6194344fb9716ae51c4de431e9a8a4b783252ddb) — Add form calibration lab and transactional date-driven season integration
- 2026-11-10 [`8c0f24c`](https://github.com/KonradKrol/sj-sim/commit/8c0f24ce7dd2a1a3076cbeba3122cb5a5a9a37cf) — Add selectable athletes and temporary per-athlete form edits to the form lab
- 2026-11-10 [`5f8cbe3`](https://github.com/KonradKrol/sj-sim/commit/5f8cbe3005cdfb3519ad621b6718e043f993c005) — Pair compact autosave and automatic form switches in the season overview
- 2026-11-10 [`2a358b0`](https://github.com/KonradKrol/sj-sim/commit/2a358b0de1ede06aa781a188e393d1e6d88ad8f1) — Allow selecting any ten athletes for form lab charts without rerunning experiments
- 2026-11-10 [`633e162`](https://github.com/KonradKrol/sj-sim/commit/633e1620015b1f61344270102a83bfc09fde5e91) — Remember automatic race gate and recommend from every participant before jumps and rounds
- 2026-11-10 [`d84e218`](https://github.com/KonradKrol/sj-sim/commit/d84e2185021eec193a310f8130db8a8a5bcb8eb0) — Finish compact form status, detailed diagnostics and progression on cancelled events
- 2026-11-10 [`18a8032`](https://github.com/KonradKrol/sj-sim/commit/18a80321578f03b0268beaa3251041f9573bb975) — Set built-in form generator defaults to the owner's current local calibration
- 2026-11-10 [`3da8760`](https://github.com/KonradKrol/sj-sim/commit/3da8760d9eebfb0f55a5dcb1bdbeba191b512a51) — Apply configurable diminishing returns to positive form bonuses above OVR 75
- 2026-11-10 [`20d8250`](https://github.com/KonradKrol/sj-sim/commit/20d8250a1d0b2487abbe9674d8accd16a2e224c5) — Ship owner's current 83 athletes, 128 hills and rules as the beta defaults; refresh form calibration
- 2026-11-10 [`51c3903`](https://github.com/KonradKrol/sj-sim/commit/51c39039afe0ba115e5297dc6e18fb50777af827) — Simplify low and high form labels
- 2026-11-10 [`e3155d1`](https://github.com/KonradKrol/sj-sim/commit/e3155d18e591533bb2715f1bedad690de3c892e1) — Store beta packs alongside each installation and seed the current default roster
- 2026-11-10 [`c12aca6`](https://github.com/KonradKrol/sj-sim/commit/c12aca6978018d3b7b1207dccff7b8a887d05ee3) — Capture the latest saved packs from the running development instance
- 2026-11-10 [`dd76f98`](https://github.com/KonradKrol/sj-sim/commit/dd76f982a5019681d7dd2284b42d175de6b45527) — Bundle current World Cup 2026/27 and official Summer Grand Prix 2026 calendar presets
- 2026-11-10 [`74fd390`](https://github.com/KonradKrol/sj-sim/commit/74fd390c133608d5e598c5326bc97f6344d6f5a6) — Fix World Cup calendar preset name
- 2026-11-10 [`dfdf531`](https://github.com/KonradKrol/sj-sim/commit/dfdf531d303fcf2f36c8be0d491590682fa7aa74) — Keep valid saved balance profiles readable across generator and perk changes
- 2026-11-10 [`8da8546`](https://github.com/KonradKrol/sj-sim/commit/8da85465cbfe1b0e7cb9700f2c5e1a5d538be6aa) — Hide target distances below seventy percent of the K point
- 2026-11-10 [`83f6206`](https://github.com/KonradKrol/sj-sim/commit/83f62060270e5b36daa58a456bd953b9ea879d97) — Respect disabled coach 95 percent rule when calculating target lines and legacy scores
- 2026-11-10 [`68b79e8`](https://github.com/KonradKrol/sj-sim/commit/68b79e8ff23e4d928b7e240004b8ee727e20ebfc) — Add compact and spacious hill perks with configurable final distance spread
- 2026-11-10 [`b21f020`](https://github.com/KonradKrol/sj-sim/commit/b21f020ba2d7ddb8c45f4fcc102e247b84a19277) — Double compact hill distance compression while preserving spacious hill strength
- 2026-11-10 [`a5e279c`](https://github.com/KonradKrol/sj-sim/commit/a5e279c6c938f0a027b49e2f987618f41e829b37) — Scale compact hill compression from five to twenty percent
- 2026-11-10 [`893fa3b`](https://github.com/KonradKrol/sj-sim/commit/893fa3b08525d770dd9384c91cd8ed6dd96a07d8) — Double compact hill compression to ten through forty percent
- 2026-11-10 [`5d38fc7`](https://github.com/KonradKrol/sj-sim/commit/5d38fc7c36d6f97e0779462730d2fdbb615fd03f) — Restore compact hill compression to five through twenty percent
- 2026-11-10 [`f8af500`](https://github.com/KonradKrol/sj-sim/commit/f8af500fa3a68a32024fdf68d0e2fc8751c3c90f) — Increase compact hill compression by thirty percent
- 2026-11-10 [`bcec6e2`](https://github.com/KonradKrol/sj-sim/commit/bcec6e219aaff76bc0896fa26ed1565a0c8677ce) — Scale compact hill compression to a maximum of eighteen percent
- 2026-11-10 [`28c5774`](https://github.com/KonradKrol/sj-sim/commit/28c5774356a10f33cd11c05a073dfe3e8c8240c7) — Scale compact hill compression to twenty-five percent maximum
- 2026-11-10 [`0bec7e0`](https://github.com/KonradKrol/sj-sim/commit/0bec7e02540fa224093c1c189cae061cd91a8e09) — Scale compact hill compression to thirty-two percent maximum
- 2026-11-11 [`f5889e8`](https://github.com/KonradKrol/sj-sim/commit/f5889e8471476ae9538530cbebcc720368afad2d) — Recalibrate distance spread judging weather variability and symmetric hill perks
- 2026-11-11 [`09f2a70`](https://github.com/KonradKrol/sj-sim/commit/09f2a70244c3034b1fed65a03e3baa4b15e52352) — Separate jump randomness from distance spread and record judging calibration
- 2026-11-11 [`8d3afdf`](https://github.com/KonradKrol/sj-sim/commit/8d3afdfb37e067aef5c921963280c8dcec98caec) — Halve style mark gaps above the correct landing class
- 2026-11-11 [`b7f7e89`](https://github.com/KonradKrol/sj-sim/commit/b7f7e8925b5a34d3b0b97f31f66fd92f3fd80ff1) — Add five seasonal form variability levels with frozen per-save configuration
- 2026-11-11 [`4c77762`](https://github.com/KonradKrol/sj-sim/commit/4c77762f594e20fcbb3f807264697ddc66512161) — Reduce specialist wind and hill effects and quality distance spread
- 2026-11-11 [`6558858`](https://github.com/KonradKrol/sj-sim/commit/6558858c37f4c034b324edba065a289af77f34a2) — Restore compact hill maximum to forty-five percent
- 2026-11-11 [`6bfc7a7`](https://github.com/KonradKrol/sj-sim/commit/6bfc7a788335a6947da54f80f97d1c25754374af) — Undo only the latest general quality-distance reduction and retain hill perk tuning
- 2026-11-11 [`9d73076`](https://github.com/KonradKrol/sj-sim/commit/9d73076070858c178aeeb9f344d0f588e0aab28e) — Reduce quality-based distance spread by a further five percent
- 2026-11-11 [`b8f1ea8`](https://github.com/KonradKrol/sj-sim/commit/b8f1ea87fa580b515da1b16ba549f23a260c0f5c) — Keep classification dropdown alive while the competition setup refreshes
- 2026-11-11 [`3a01b4f`](https://github.com/KonradKrol/sj-sim/commit/3a01b4f33a64888c461a993a5e083f1c9bc3d146) — Add deterministic daily form for dated seasons and standalone competitions
- 2026-11-11 [`e8da6f4`](https://github.com/KonradKrol/sj-sim/commit/e8da6f42505b6336ef5b128a60fcbdd53e4539c9) — Silently preserve start order when a classification has no results
- 2026-11-11 [`6c5ac9b`](https://github.com/KonradKrol/sj-sim/commit/6c5ac9b8ffd54be28e3ae84003a9aa10b30efba5) — Offer next group as the primary action between team jumping groups
- 2026-11-11 [`3cd40ac`](https://github.com/KonradKrol/sj-sim/commit/3cd40ac633389ec63c5861705be636190fa2582a) — Run upcoming season events directly with automatic gate and archived results
- 2026-11-11 [`e921f39`](https://github.com/KonradKrol/sj-sim/commit/e921f39cfc215b21636c9836e8ff12f49c3e83d9) — Offer optional signed daily form bonus in jump details, disabled by default
- 2026-11-11 [`591f785`](https://github.com/KonradKrol/sj-sim/commit/591f7851602306e5cda1bce34d0badb9582eaf06) — Order season start list by qualification source
- 2026-11-11 [`07a4ec6`](https://github.com/KonradKrol/sj-sim/commit/07a4ec652fd2e97ed3f3557d93e29c2dc3bd99ea) — Respect first round start list sorting setting
- 2026-11-11 [`21b6f46`](https://github.com/KonradKrol/sj-sim/commit/21b6f46c4e985efcf6dc3d6a2ad11340251e788c) — Configure qualification source ordering in calendar links
- 2026-11-11 [`81b474e`](https://github.com/KonradKrol/sj-sim/commit/81b474ee832f37f58cb4a0669f76f63c044259fe) — Choose season start list sorting source in calendar
- 2026-11-11 [`ec0b9f1`](https://github.com/KonradKrol/sj-sim/commit/ec0b9f1fa52bd7c56a6b0abdc5c91756b5a70d14) — Separate calendar start order controls and clarify choices
- 2026-11-11 [`dedf154`](https://github.com/KonradKrol/sj-sim/commit/dedf154e0fd722a5357ac3d70ac8903c0280e342) — Slow ordinary precipitation changes while preserving blizzard variability
- 2026-11-11 [`0f2cc6f`](https://github.com/KonradKrol/sj-sim/commit/0f2cc6f4c5980c99b6a7add793f4aee95eee40ee) — Remove increasing precipitation choice and automatic generation
- 2026-11-11 [`513539f`](https://github.com/KonradKrol/sj-sim/commit/513539f50ee74aa6a953b6103e673e7acd6fb309) — Show team points gap from second group of first round
- 2026-11-11 [`db9bc4d`](https://github.com/KonradKrol/sj-sim/commit/db9bc4d68a79b4ac532f70aa77199abdc237e466) — Confirm coach gate reduction with rounded 95 percent HS threshold
- 2026-11-11 [`2751da5`](https://github.com/KonradKrol/sj-sim/commit/2751da59eec2008d9d58e04d0ffec044763f8876) — Inherit parent start order and automatic KO ordering in trial rounds
- 2026-11-11 [`79cd11f`](https://github.com/KonradKrol/sj-sim/commit/79cd11f2409f8eaec83737a0d77e447feb5ed4da) — Expand form engine to seven regimes and shorten holding times by 35 percent
- 2026-11-11 [`a2745fc`](https://github.com/KonradKrol/sj-sim/commit/a2745fc26426ec8a3886fbdaec3b3ea474a0fada) — Accept saved balance snapshots containing retired precipitation parameter
- 2026-11-11 [`fd4451a`](https://github.com/KonradKrol/sj-sim/commit/fd4451a9fcbac4d02899c5482a831d24d349d89b) — Allow qualification source sorting for multiple calendar events
- 2026-11-11 [`747dd8f`](https://github.com/KonradKrol/sj-sim/commit/747dd8f4f5fbe119e17d6280e17ae4e25d592a3b) — Expand competition sorting picker and default to preceding event
- 2026-11-11 [`e3ceb1a`](https://github.com/KonradKrol/sj-sim/commit/e3ceb1aa58ec351580113a99166328d402ade6ba) — Show hill flags HS event type and weekday in calendar event pickers
- 2026-11-11 [`e69c73f`](https://github.com/KonradKrol/sj-sim/commit/e69c73f254e6157b6edcc45d4c56161a9889dd5f) — Refresh season hill editor on selection without draft writeback during loading
- 2026-11-11 [`76681d8`](https://github.com/KonradKrol/sj-sim/commit/76681d8cdbbecf3e7b4c9b186dbe9b493b445a4b) — Confirm disabling automatic form and lock dates after season starts
- 2026-11-11 [`5a32406`](https://github.com/KonradKrol/sj-sim/commit/5a324064907f2f4b7435c94de7dd6443012427b0) — Allow calendar date drafts with visible blockers before confirmation and competition
- 2026-11-11 [`1deef44`](https://github.com/KonradKrol/sj-sim/commit/1deef4488b1112ebcdae066d9e1ebef9e5c25776) — Reduce metres per quality point by five percent
- 2026-11-11 [`3887ca9`](https://github.com/KonradKrol/sj-sim/commit/3887ca9746a75dc0e3178a92616565ba751cd0c9) — Use improved wind measurement and remove measurement choice from UI
- 2026-11-11 [`ed7ded2`](https://github.com/KonradKrol/sj-sim/commit/ed7ded24af5ef036ad985d0279f29ba1e4d18fb3) — Temporarily disable precipitation and hide competition precipitation controls
- 2026-11-11 [`bac38ae`](https://github.com/KonradKrol/sj-sim/commit/bac38ae9c21700879d499ab9800a58ac28f6edd4) — Improve calendar empty states roster visibility and action menu
- 2026-11-11 [`3cd3a33`](https://github.com/KonradKrol/sj-sim/commit/3cd3a33b71efcc18ac275f2b02e135ea1a57d4e7) — Hide remaining precipitation controls in settings manipulation and playground
- 2026-11-11 [`7ab24dd`](https://github.com/KonradKrol/sj-sim/commit/7ab24dd317f10c8937fa2b8342dc0cd5e59b93dc) — Limit lucky loser badges to their current KO round
- 2026-11-11 [`1c7ea12`](https://github.com/KonradKrol/sj-sim/commit/1c7ea128e931a1a4d8db28de789bc6e8076a7373) — Bake current owner form calibration into engine defaults
- 2026-11-11 [`762c669`](https://github.com/KonradKrol/sj-sim/commit/762c669c6789239e93033ea9eb8ee7749932198f) — Apply target form regime occupancy and symmetric stability perks
- 2026-11-11 [`4680bf8`](https://github.com/KonradKrol/sj-sim/commit/4680bf8601f7a4d26b33d7c1d881d6ccf0ac5f09) — Add developer shortcut to advance every athlete form by specified days
- 2026-11-11 [`eba558b`](https://github.com/KonradKrol/sj-sim/commit/eba558bfbe2c2d71be1213d5bf331a1287a13e61) — Rework start list participant selection and grouped ordering tools
- 2026-11-11 [`f15513f`](https://github.com/KonradKrol/sj-sim/commit/f15513f1fb6e112d8e8995ac23add17730ca92bb) — Randomize unmatched entrants even when sorting source has no results
- 2026-11-11 [`7e7c585`](https://github.com/KonradKrol/sj-sim/commit/7e7c58562d7bfa4cede59af8030b7375d9615183) — Handle invalid competition settings safely in start ordering tools
- 2026-11-11 [`8c0e203`](https://github.com/KonradKrol/sj-sim/commit/8c0e203d8bd299b3251f50371266a714d26bc38a) — Identify current lucky loser round by number instead of rules address
- 2026-11-11 [`a74511e`](https://github.com/KonradKrol/sj-sim/commit/a74511eeebfe24ec64029464653bd3df6e88f8ae) — Remove read-only Flow implicit height assignment from roster workspace
- 2026-11-11 [`0266a82`](https://github.com/KonradKrol/sj-sim/commit/0266a82c5b95bfe1243147e394d27fbb6a6d9ac7) — Split large KO group allocation into striped and interleaved methods
- 2026-11-11 [`ccc6138`](https://github.com/KonradKrol/sj-sim/commit/ccc6138eda33b0182a71e1dd944b774b0f4f37dc) — Add country-safe KO baskets and balance group mean and spread using shared potential
- 2026-11-11 [`852cb9d`](https://github.com/KonradKrol/sj-sim/commit/852cb9d81f94ebf030fca17e8823ffbe94b04fe1) — Refine start tools hierarchy and add compact source and country ordering dialogs
- 2026-11-11 [`648aa1d`](https://github.com/KonradKrol/sj-sim/commit/648aa1dac19eb1cc418c7b66da2641c1538b07d8) — Calculate KO advancement from current round rules and live jump completion
- 2026-11-11 [`3ecdd0c`](https://github.com/KonradKrol/sj-sim/commit/3ecdd0c0a35755f2e426b4e4c7b546f49a981f77) — Initialize season competition participants from the active athlete list
- 2026-11-11 [`ebaf5be`](https://github.com/KonradKrol/sj-sim/commit/ebaf5be92fe6e7383e0344127131063477663540) — Make start list tool icons larger and reduce color noise
- 2026-11-11 [`f4824e3`](https://github.com/KonradKrol/sj-sim/commit/f4824e3cfd2122620e0ee5e82fa23d60a232e244) — Place compact season selector in start order source dialog header
- 2026-11-11 [`2d88d8c`](https://github.com/KonradKrol/sj-sim/commit/2d88d8c84f54adda24d5a19b909df679ec09c5bc) — Make blocking calendar issues prominent and include pending qualification conflicts
- 2026-11-11 [`fedcb58`](https://github.com/KonradKrol/sj-sim/commit/fedcb58e8707edc64130c6532d19643b47cde589) — Increase visual prominence of blocking calendar alerts
- 2026-11-11 [`048f28e`](https://github.com/KonradKrol/sj-sim/commit/048f28e16b6286201e3b674eb550a191b6e7486d) — Enforce chronological season progression across all calendars
- 2026-11-11 [`7ba36a4`](https://github.com/KonradKrol/sj-sim/commit/7ba36a4d92a8626e904c54ca1d7c7b4fa3fd0a6c) — Restore athlete statistics access with focused charts and working CSV export
- 2026-11-11 [`bb49fde`](https://github.com/KonradKrol/sj-sim/commit/bb49fde6118af0f1eec6e9eab7983d4d44557ad4) — Break QML component cycle in athlete statistics filter loader
- 2026-11-11 [`8779dcf`](https://github.com/KonradKrol/sj-sim/commit/8779dcff4c850821e68d1072d489f1a978c9f176) — Distinguish rejected calendar actions from invalid calendar dates
- 2026-11-11 [`fb8d8bc`](https://github.com/KonradKrol/sj-sim/commit/fb8d8bc9d0c255fe5807050f6de790b57cfa0928) — Show aligned leader gaps beside competition and classification totals
- 2026-11-11 [`0fdaced`](https://github.com/KonradKrol/sj-sim/commit/0fdacedfbebc7cb703bcf4318f300d7024b8885f) — Use subtle green backgrounds for completed calendar events
- 2026-11-11 [`4de02ed`](https://github.com/KonradKrol/sj-sim/commit/4de02ed7cbc35eae50efe2b2fdd8786e378d23d3) — Expand overview standings to the season viewport height
- 2026-11-11 [`0daa48b`](https://github.com/KonradKrol/sj-sim/commit/0daa48b6caca82936508df40dd4c68420843d711) — Rebuild athlete statistics around a position chart and direct event filters
- 2026-11-11 [`4590810`](https://github.com/KonradKrol/sj-sim/commit/4590810430d93a39e0d203cd2bf16e014d2fafec) — Replace application branding and Windows icons with the new Sj.Sim logo
- 2026-11-11 [`0c9a904`](https://github.com/KonradKrol/sj-sim/commit/0c9a9045777fc6df0646d1e14d0531c9f7a9b671) — Validate locked team membership independently of team starting order and IDs
- 2026-11-11 [`4290197`](https://github.com/KonradKrol/sj-sim/commit/4290197aae5fc72a36f8cd21055e8869fe12ded8) — Make statistics filter clicks toggle the authoritative event selection
- 2026-11-11 [`dd03333`](https://github.com/KonradKrol/sj-sim/commit/dd03333ce044a489f061a8244f836cd117b0a748) — Reserve a clear minimum gap between total points and leader deficits
- 2026-11-11 [`7ba538d`](https://github.com/KonradKrol/sj-sim/commit/7ba538dd0338409180cb70bfdad1f8d64642f455) — Show leader deficits in archived competition and classification results
- 2026-11-11 [`fb7db70`](https://github.com/KonradKrol/sj-sim/commit/fb7db7010d1c12ffb80ce6193c077dd053487ec3) — Remove baseline support landings and reserve distance risk for jumps beyond real HS
- 2026-11-11 [`4946159`](https://github.com/KonradKrol/sj-sim/commit/494615950849263b90b201a012921c0768901f62) — Refresh beta starter packs and ship six curated official and historical calendars
- 2026-11-11 [`210f057`](https://github.com/KonradKrol/sj-sim/commit/210f05750fd1cd8613dc246b5ea35abd58d3ab36) — Resolve the developer form advance action from the season window
- 2026-11-12 [`be2df89`](https://github.com/KonradKrol/sj-sim/commit/be2df8950d1486bdef5a1f98279344507852a9e0) — Calibrate regularity perk tails to the requested weak, severe and excellent jump rates
- 2026-11-12 [`4f287fb`](https://github.com/KonradKrol/sj-sim/commit/4f287fb82d81abbfa2708225f27131cca6973a6b) — Cache successful immutable balance validation to speed up save loading
- 2026-11-12 [`1e4e299`](https://github.com/KonradKrol/sj-sim/commit/1e4e29943fd01596eb389be0af6dd0abd5aba22c) — Reuse event contributions while refreshing classification totals
- 2026-11-12 [`d7ea224`](https://github.com/KonradKrol/sj-sim/commit/d7ea22437ad5fd6b55865c11cf378d665382336a) — Speed up automatic competitions with grouped gate previews and deferred display lines
- 2026-11-12 [`b02eb4f`](https://github.com/KonradKrol/sj-sim/commit/b02eb4f877f45e6da67ec95c1e5499379ffc4510) — Index archive rows and document reproducible season performance measurements
- 2026-11-12 [`ea8ee67`](https://github.com/KonradKrol/sj-sim/commit/ea8ee6748428c4cdf8706eb822be3a1777cfedf2) — Exclude falls and support landings from judges averages used by target lines
- 2026-11-12 [`b0991a2`](https://github.com/KonradKrol/sj-sim/commit/b0991a22cef077a09d957b748b6b113e0e1ae87a) — Rename single competition mode to Quick competition across UI and translations
- 2026-11-12 [`f09712c`](https://github.com/KonradKrol/sj-sim/commit/f09712c44cbd9e638c122baf5eeae573ab1dad5d) — Introduce four-step welcome and summer/winter example simulation
- 2026-11-12 [`42d68d0`](https://github.com/KonradKrol/sj-sim/commit/42d68d08775efd66325b18549bd162267d025d15) — Introduce a shared resource library with bundled pack imports and starter defaults
- 2026-11-12 [`01cc6b5`](https://github.com/KonradKrol/sj-sim/commit/01cc6b5a5ccba7054ffee0d0bd407129f4c09793) — Test library bootstrap, resource roundtrips, safe imports and Quick navigation
- 2026-11-12 [`16a53b0`](https://github.com/KonradKrol/sj-sim/commit/16a53b070cc425f0d0ccedc331d69320c2c6e1db) — Increase athlete hill-specialisation bonuses and penalties by 75 percent
- 2026-11-12 [`f8cf480`](https://github.com/KonradKrol/sj-sim/commit/f8cf480e80f24d8c7cf51b486a4f46c904d2af5c) — Keep season athlete selection synchronized without overwriting profiles
- 2026-11-12 [`6c98f51`](https://github.com/KonradKrol/sj-sim/commit/6c98f51335bdd4e6f417c2eefa2a269cd23c5e30) — Fix simulation preference panel height and prevent row stretching
- 2026-11-12 [`5299fe6`](https://github.com/KonradKrol/sj-sim/commit/5299fe6bade047ec2dc5abb0ed369cd3a8e8197f) — Show Sj.Sim 2.0.0 consistently without beta badge
- 2026-11-12 [`8f38df4`](https://github.com/KonradKrol/sj-sim/commit/8f38df4f52281e2d60572b544b09d51ed8a5bb3d) — Add team eligibility workspace and restrict squad selection to available athletes
- 2026-11-12 [`a8a4619`](https://github.com/KonradKrol/sj-sim/commit/a8a4619ff8fc9836e98c360e86ed64ee31e6eba5) — Make welcome dialog compact with cropped imagery and clear final actions
- 2026-11-12 [`924e7aa`](https://github.com/KonradKrol/sj-sim/commit/924e7aa339cd7b612c3970efce7277f40ea6d7b6) — Temporarily hide the season athlete and hill removal warning
- 2026-11-12 [`2284b84`](https://github.com/KonradKrol/sj-sim/commit/2284b845f50a5e79c5b2efca6845f0a9390716d4) — Shrink welcome dialog by 20 percent and widen the image frame by 30 percent
- 2026-11-12 [`12b720e`](https://github.com/KonradKrol/sj-sim/commit/12b720e0f38ccaabb883120c74c9d7263952564d) — Reduce training jump quality deviation by 12 percent and trial deviation by 6 percent
- 2026-11-12 [`77aa557`](https://github.com/KonradKrol/sj-sim/commit/77aa5575c20fbd9bcde3926cb492222b9951302a) — Reduce baseline jump fall risk by 70 percent
- 2026-11-12 [`80f5126`](https://github.com/KonradKrol/sj-sim/commit/80f512631f15195d6e8d5a0a802a6aad045b9f23) — Add per-calendar competition results table to athlete statistics
- 2026-11-12 [`148cd1d`](https://github.com/KonradKrol/sj-sim/commit/148cd1d24d40023f2d2ed995e4339f2813bff7b1) — Exclude team competitions from athlete results table
- 2026-11-12 [`939ef41`](https://github.com/KonradKrol/sj-sim/commit/939ef41d17e6b726a090b9867e2fe71f7d884d66) — Load welcome screenshots at native resolution for cleaner downsampling
- 2026-11-12 [`022d4d1`](https://github.com/KonradKrol/sj-sim/commit/022d4d19461bfd6490b1cd40ed129b05b5ffbce4) — Fix season profile selection and preserve library scroll across editing and navigation
- 2026-11-12 [`da80168`](https://github.com/KonradKrol/sj-sim/commit/da801688494406bad69251d440113ac5a4d3a01f) — Take results table total from first individual classification
- 2026-11-12 [`22f4a23`](https://github.com/KonradKrol/sj-sim/commit/22f4a236c5e2d47258450e25faf3dd5c0c98a6b3) — Show q for athletes eliminated in qualifications
- 2026-11-13 [`3336ada`](https://github.com/KonradKrol/sj-sim/commit/3336ada12fb5500df82e5ae35f19cd59683d7505) — Package Windows setup installer and self-contained Linux AppImage
- 2026-11-13 [`3e12e19`](https://github.com/KonradKrol/sj-sim/commit/3e12e19bf9e664c1e41f350666628da49622dc38) — Include AppImage desktop entry in packaging sources
- 2026-11-13 [`79c53b8`](https://github.com/KonradKrol/sj-sim/commit/79c53b89ae6c9951f2e22106f39a30d09a30bcae) — Ship independent starter athlete pack with lightly altered surnames
- 2026-11-13 [`75cd74c`](https://github.com/KonradKrol/sj-sim/commit/75cd74c5a985b845ca088282afeb71550b77b6f6) — Hide national teams without enough eligible athletes immediately
- 2026-11-13 [`3036f99`](https://github.com/KonradKrol/sj-sim/commit/3036f99a7c09c2fb09218a34d3a4ce2cc342942a) — Start dated example seasons with autoform and show a dismissible inline introduction
- 2026-11-13 [`225ec6f`](https://github.com/KonradKrol/sj-sim/commit/225ec6fff44235cc8349b74e54a6657de3975acb) — Request elevation only when Microsoft runtime installation is needed
- 2026-11-13 [`4ee18dd`](https://github.com/KonradKrol/sj-sim/commit/4ee18dddbe2dc58652e503031795b9bd8a973bc9) — Reduce compact and spacious hill perk effects by twenty-five percent
- 2026-11-13 [`4c0af19`](https://github.com/KonradKrol/sj-sim/commit/4c0af1960bcf4babe2c5763dac43b83183f4fe56) — Stabilize team squad pane widths on initial entry and resizing
- 2026-11-13 [`98dab30`](https://github.com/KonradKrol/sj-sim/commit/98dab301deb06831d24c916332ca09828a31b7ab) — Soften automatic hill weather and cut extreme lottery frequency by eighty-five percent
- 2026-11-13 [`987d8be`](https://github.com/KonradKrol/sj-sim/commit/987d8be73dd2a9f65219948e8de02b84bf226299) — Allow exact custom temporal and spatial wind variability values
- 2026-11-13 [`39ed367`](https://github.com/KonradKrol/sj-sim/commit/39ed36706e768ce32e5e26ba933ff6aa52f273f7) — Display custom wind variability on a simpler zero-to-ten scale
- 2026-11-13 [`f2005d1`](https://github.com/KonradKrol/sj-sim/commit/f2005d19102a4b10aafd6675a6ed8bd45284bee9) — Keep simulation preference sections at their natural content height
- 2026-11-13 [`9ed4156`](https://github.com/KonradKrol/sj-sim/commit/9ed4156fa3dbe555a9f911eca68efba767f8cdcd) — Alter exactly two letters in every starter athlete surname
- 2026-11-13 [`e778a32`](https://github.com/KonradKrol/sj-sim/commit/e778a3223befc620ee8eef5746ec3a686faa2ec6) — Avoid redundant season widget traversal and index calendar dependencies once
- 2026-11-13 [`a5a7b8a`](https://github.com/KonradKrol/sj-sim/commit/a5a7b8afc8651468088253a2a2e295853352638f) — Allow confirmed recoverable removal of selected library resources
- 2026-11-13 [`a72d139`](https://github.com/KonradKrol/sj-sim/commit/a72d13950fbbce20f78f21de43b7085b73d1a906) — Allow library deletion inside the database editor without resurrecting removed packs
- 2026-11-15 [`d0a9786`](https://github.com/KonradKrol/sj-sim/commit/d0a978659a218bbb0248a9a75499d588378b5b4a) — Select a consistent native Qt runtime before using the legacy development fallback
- 2026-11-16 [`6e78ba7`](https://github.com/KonradKrol/sj-sim/commit/6e78ba7b6c61d9e996239ba247b9e6e6673aff9d) — Hide unfinished roster distribution and remove duplicate inspector preset action
- 2026-11-16 [`636260f`](https://github.com/KonradKrol/sj-sim/commit/636260ff9c6ba190bf7aad9eac3927b3abeb248a) — Add Discord community join button to the main menu
- 2026-11-16 [`6b4b73c`](https://github.com/KonradKrol/sj-sim/commit/6b4b73cf2b0041a7ca6dd4b36c503c0a00035e52) — Use a quiet Discord logo button on the right side of the home footer
- 2026-11-16 [`f9c8795`](https://github.com/KonradKrol/sj-sim/commit/f9c87950e9241a4411cf2d4b281999a159ceec4e) — Use the matching fallback import scanner when the system Qt SDK lacks tools
- 2026-11-16 [`39526d2`](https://github.com/KonradKrol/sj-sim/commit/39526d2c930ed2bd4bff11ee975211d5166ef62b) — Normalize the Discord SVG path for Qt 5 and check all shipped vector assets
- 2026-11-16 [`28ff00c`](https://github.com/KonradKrol/sj-sim/commit/28ff00c6083d61ae0dc868a89c21f314ef943245) — Supply only the matching missing Shapes module and validate complete deployed UI before publishing
- 2026-11-16 [`d33c895`](https://github.com/KonradKrol/sj-sim/commit/d33c895f645cd5c300d4b0b8df4605c1ceb913c8) — Document complete 2.0.0 player changes, release notes and full branch history
- 2026-11-16 [`6d3552b`](https://github.com/KonradKrol/sj-sim/commit/6d3552b809df29cea51f6ed5742569a0df6ec8fe) — Update community Discord invite in the menu and localized welcome
- 2026-11-16 [`a90e306`](https://github.com/KonradKrol/sj-sim/commit/a90e3063b9d1cc99aaa0dff734a4ac90bb2d1fd0) — Allow hiding automatic form and reveal diagnostics only with a developer shortcut
- 2026-11-16 [`643d8b5`](https://github.com/KonradKrol/sj-sim/commit/643d8b5af0680e8d5191d0cd22ea902ef3e1e493) — Defer simulation preference height updates to avoid recursive layout

# V. Pełny manifest zmienionych plików

Różnica `main → 643d8b5`, z wykrywaniem przeniesień przez Git. `A` — dodany, `M` — zmieniony, `D` — usunięty, `R` — przeniesiony. Manifest nie obejmuje niezacommitowanych lokalnych plików użytkownika.

```text
A	.agents/skills/qt-canvas2d/LICENSE.txt
A	.agents/skills/qt-canvas2d/SKILL.md
A	.agents/skills/qt-canvas2d/references/api-reference.md
A	.agents/skills/qt-canvas2d/references/porting.md
A	.agents/skills/qt-canvas2d/references/recipes/analog-clock.qml
A	.agents/skills/qt-canvas2d/references/recipes/animated-line-chart.qml
A	.agents/skills/qt-canvas2d/references/recipes/bar-chart.qml
A	.agents/skills/qt-canvas2d/references/recipes/color-effects-and-clipping.qml
A	.agents/skills/qt-canvas2d/references/recipes/freehand-drawing.qml
A	.agents/skills/qt-canvas2d/references/recipes/images-and-patterns.qml
A	.agents/skills/qt-canvas2d/references/recipes/level-meter.qml
A	.agents/skills/qt-canvas2d/references/recipes/node-link-diagram.qml
A	.agents/skills/qt-canvas2d/references/recipes/oscilloscope.qml
A	.agents/skills/qt-canvas2d/references/recipes/pan-zoom-viewport.qml
A	.agents/skills/qt-canvas2d/references/recipes/pie-donut-chart.qml
A	.agents/skills/qt-canvas2d/references/recipes/progress-ring.qml
A	.agents/skills/qt-canvas2d/references/recipes/radial-gauge.qml
A	.agents/skills/qt-canvas2d/references/recipes/rotary-knob.qml
A	.agents/skills/qt-canvas2d/references/recipes/scatter-plot.qml
A	.agents/skills/qt-canvas2d/references/recipes/shadowed-card-button.qml
A	.agents/skills/qt-canvas2d/references/recipes/sparkline-delegate.qml
A	.agents/skills/qt-canvas2d/references/recipes/static-shapes.qml
A	.agents/skills/qt-canvas2d/references/recipes/text-panel.qml
A	.agents/skills/qt-canvas2d/references/rules.md
A	.agents/skills/qt-cmake-project/LICENSE.txt
A	.agents/skills/qt-cmake-project/SKILL.md
A	.agents/skills/qt-cmake-project/references/common-mistakes.md
A	.agents/skills/qt-cmake-project/references/configure.md
A	.agents/skills/qt-cmake-project/references/modular-architecture.md
A	.agents/skills/qt-cmake-project/references/qml-integration.md
A	.agents/skills/qt-cmake-project/references/resources.md
A	.agents/skills/qt-cmake-project/references/simple-project.md
A	.agents/skills/qt-cpp-docs/LICENSE.txt
A	.agents/skills/qt-cpp-docs/SKILL.md
A	.agents/skills/qt-cpp-review/LICENSE.txt
A	.agents/skills/qt-cpp-review/SKILL.md
A	.agents/skills/qt-cpp-review/platforms/windsurf.md
A	.agents/skills/qt-cpp-review/references/lint-scripts/qt_review_lint.py
A	.agents/skills/qt-cpp-review/references/qt-deprecated-classes.md
A	.agents/skills/qt-cpp-review/references/qt-framework-checklist.md
A	.agents/skills/qt-cpp-review/references/qt-review-checklist.md
A	.agents/skills/qt-figma-component-generation/SKILL.md
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/BadgeLabel.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/BadgeLabelStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/BadgeNotification.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/BadgeNotificationStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/Button.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/ButtonStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/Card.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/CardStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/CheckBox.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/CheckBoxStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/ComboBox.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/ComboBoxStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/Dialog.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/DialogButtonBox.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/DialogButtonBoxStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/DialogStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/IconButton.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/IconButtonStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/Indicator.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/IndicatorStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/ItemDelegate.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/ItemDelegateStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/Label.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/LabelStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/LinkInline.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/LinkInlineStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/LinkList.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/LinkListStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/LinkStandalone.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/LinkStandaloneStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/ListItem.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/ListItemStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/Menu.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/MenuButton.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/MenuButtonStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/MenuItem.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/MenuItemStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/MenuStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/PaginationDot.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/PaginationDotStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/PaginationIndex.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/PaginationIndexStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/PaginationNumber.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/PaginationNumberStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/Popup.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/PopupStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/ProgressBar.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/ProgressBarStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/RadioButton.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/RadioButtonStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/RangeSlider.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/RangeSliderStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/ScrollBar.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/ScrollBarStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/SearchField.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/SearchFieldStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/SearchMenu.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/SearchMenuStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/Slider.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/SliderStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/SortFilterModel.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/SpinBox.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/SpinBoxStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/Switch.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/SwitchStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/TabBar.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/TabBarStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/TabButton.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/TabButtonStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/Tag.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/TagStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/TextField.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/TextFieldStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/ToolTip.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/ToolTipStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/Toolbar.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/ToolbarStyle.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/TreeViewDelegate.qml
A	.agents/skills/qt-figma-component-generation/assets/qt-controls/TreeViewDelegateStyle.qml
A	.agents/skills/qt-figma-component-generation/references/Button.qml
A	.agents/skills/qt-figma-component-generation/references/Checkbox.qml
A	.agents/skills/qt-figma-component-generation/references/Select.qml
A	.agents/skills/qt-figma-component-generation/references/TextField.qml
A	.agents/skills/qt-figma-component-generation/references/Toggle.qml
A	.agents/skills/qt-figma-token-extraction/SKILL.md
A	.agents/skills/qt-figma-token-extraction/examples/FontInterface.qml
A	.agents/skills/qt-figma-token-extraction/examples/Primitives.qml
A	.agents/skills/qt-figma-token-extraction/examples/Spacing.qml
A	.agents/skills/qt-figma-token-extraction/examples/Theme.qml
A	.agents/skills/qt-figma-token-extraction/examples/Typography.qml
A	.agents/skills/qt-figma-token-extraction/references/token-mapping.md
A	.agents/skills/qt-qml-docs/LICENSE.txt
A	.agents/skills/qt-qml-docs/SKILL.md
A	.agents/skills/qt-qml-docs/platforms/windsurf.md
A	.agents/skills/qt-qml-profiler/LICENSE.txt
A	.agents/skills/qt-qml-profiler/SKILL.md
A	.agents/skills/qt-qml-profiler/references/qml-performance-anti-patterns.md
A	.agents/skills/qt-qml-profiler/references/scripts/parse-qmlprofiler-trace.py
A	.agents/skills/qt-qml-review/LICENSE.txt
A	.agents/skills/qt-qml-review/SKILL.md
A	.agents/skills/qt-qml-review/platforms/windsurf.md
A	.agents/skills/qt-qml-review/references/lint-scripts/qt_qml_lint.py
A	.agents/skills/qt-qml-review/references/qt-qml-review-checklist.md
A	.agents/skills/qt-qml-test-run/LICENSE.txt
A	.agents/skills/qt-qml-test-run/SKILL.md
A	.agents/skills/qt-qml-test-run/references/qt-quick-test-cmake.md
A	.agents/skills/qt-qml-test-run/references/qt-quick-test-report-format.md
A	.agents/skills/qt-qml-test-run/references/scripts/parse-qmltestrunner-output.py
A	.agents/skills/qt-qml-test/LICENSE.txt
A	.agents/skills/qt-qml-test/SKILL.md
A	.agents/skills/qt-qml-test/platforms/copilot.prompt.md
A	.agents/skills/qt-qml-test/references/qt-quick-test-controls.md
A	.agents/skills/qt-qml-test/references/qt-quick-test-pitfalls.md
A	.agents/skills/qt-qml-test/references/qt-quick-test-pre-send-scan.md
A	.agents/skills/qt-qml-test/references/qt-quick-test-project-context.md
A	.agents/skills/qt-qml-test/references/qt-quick-test-properties.md
A	.agents/skills/qt-qml-test/references/qt-quick-test-rules.md
A	.agents/skills/qt-qml-test/references/qt-quick-test-source-import.md
A	.agents/skills/qt-qml-test/references/qt-quick-test-template.md
A	.agents/skills/qt-qml/LICENSE.txt
A	.agents/skills/qt-qml/SKILL.md
A	.agents/skills/qt-qml/platforms/windsurf.md
A	.agents/skills/qt-qml/references/qmlpreview.md
A	.agents/skills/qt-ui-design/LICENSE.txt
A	.agents/skills/qt-ui-design/SKILL.md
A	.gitattributes
M	.github/workflows/release.yml
M	.gitignore
A	30-10-work.md
A	31-10-start.md
A	AGENTS.md
A	CHANGELOG_2.0.0.md
A	CODEX_work_31_10.md
A	HANDOFF-31-10-LISTY.md
A	Od-Nov-5.md
M	README.md
M	assets.qrc
A	data/beta/Packs/Athletes/31c2add1-69e3-4b68-8262-81747d86ddd6/EST_KaimarVagul.png
A	data/beta/Packs/Athletes/31c2add1-69e3-4b68-8262-81747d86ddd6/GER_PiusPaschke.png
A	data/beta/Packs/Athletes/31c2add1-69e3-4b68-8262-81747d86ddd6/POL_DawidKubacki.png
A	data/beta/Packs/Athletes/31c2add1-69e3-4b68-8262-81747d86ddd6/ZDJECIA_ZRODLA.md
A	data/beta/Packs/Athletes/31c2add1-69e3-4b68-8262-81747d86ddd6/pack.json
A	data/beta/Packs/Hills/d1759190-45e8-415f-9b48-053256fee702.json
A	data/beta/Packs/catalog.json
A	data/beta/README.md
A	data/beta/default-balance.json
A	data/beta/packs.json
A	data/defaults/Calendars/LGP23.json
A	data/defaults/Calendars/LGP24.json
A	data/defaults/Calendars/LGP26.json
A	data/defaults/Calendars/WC23-24.json
A	data/defaults/Calendars/WC24-25.json
A	data/defaults/Calendars/WC26-27.json
A	data/defaults/Packs/Athletes/700c941a-9e24-4065-8888-56b7c4cd467e/EST_KaimarVagul.png
A	data/defaults/Packs/Athletes/700c941a-9e24-4065-8888-56b7c4cd467e/GER_PiusPaschke.png
A	data/defaults/Packs/Athletes/700c941a-9e24-4065-8888-56b7c4cd467e/POL_DawidKubacki.png
A	data/defaults/Packs/Athletes/700c941a-9e24-4065-8888-56b7c4cd467e/ZDJECIA_ZRODLA.md
A	data/defaults/Packs/Athletes/700c941a-9e24-4065-8888-56b7c4cd467e/pack.json
A	data/defaults/Packs/Hills/d1759190-45e8-415f-9b48-053256fee702.json
A	data/defaults/Packs/Points/Mini World Cup.json
A	"data/defaults/Packs/Points/Puchar Narod\303\263w za Duety.json"
A	"data/defaults/Packs/Points/Puchar Narod\303\263w.json"
A	data/defaults/Packs/Points/Team World Cup.json
A	data/defaults/Packs/Points/World Cup.json
A	data/defaults/Packs/Rules/947c7732-beeb-4dca-b836-91b3b8e0caa6.json
A	data/defaults/Packs/catalog.json
A	data/defaults/Settings/countries.json
A	data/defaults/Tools/FormPresets/Reset.json
A	"data/defaults/Tools/FormPresets/Zmiana mi\304\231dzyweekendowa.json"
A	data/defaults/Tools/FormPresets/Zmiana po jednych zawodach.json
A	"data/defaults/Tools/FormPresets/Zmiana po jednym dniu zawod\303\263w.json"
A	data/defaults/Tools/ImportancePresets/COC (Competition).json
A	data/defaults/Tools/ImportancePresets/COC (Training).json
A	data/defaults/Tools/ImportancePresets/COC (Trial round).json
A	data/defaults/Tools/ImportancePresets/Championships (Competition).json
A	data/defaults/Tools/ImportancePresets/Championships (Qualifications).json
A	data/defaults/Tools/ImportancePresets/Championships (Training).json
A	data/defaults/Tools/ImportancePresets/Championships (Trial).json
A	data/defaults/Tools/ImportancePresets/FC (Competition).json
A	data/defaults/Tools/ImportancePresets/FC (Training).json
A	data/defaults/Tools/ImportancePresets/FC (Trial round).json
A	data/defaults/Tools/ImportancePresets/WC (Competition).json
A	data/defaults/Tools/ImportancePresets/WC (Qualifications).json
A	data/defaults/Tools/ImportancePresets/WC (Training).json
A	data/defaults/Tools/ImportancePresets/WC (Trial round).json
A	data/packs/men-2026/README.md
A	data/packs/men-2026/pack.json
A	data/packs/men-2026/sources.json
A	docs/ATHLETE_LIBRARY_2026-10-27.md
A	docs/ATHLETE_PATCHES_UX_2026-10-26.md
A	docs/ATHLETE_STATISTICS.md
A	docs/ATHLETE_STUDIO_2026-10-26.md
R100	AUDIT_RESPONSIVENESS_REPORT.md	docs/AUDIT_RESPONSIVENESS_REPORT.md
A	docs/AUTHOR_AND_SINGLE_JUMPS_2026-10-22.md
A	docs/BALANCE_LAB.md
A	docs/BALANCE_LAB_DESIGN.md
A	docs/BALANCE_LAB_IMPLEMENTATION.md
A	docs/BUGFIX_AUDIT_2026-10-21.md
A	docs/CALENDAR_COPY_FIXES_2026-10-22.md
A	docs/CALENDAR_DATES.md
A	docs/CALENDAR_PLANNER_AND_STANDINGS_UX_2026-10-25.md
A	docs/CLASSIFICATION_EDITOR_UX_2026-10-25.md
A	docs/CLASSIFICATION_RULES_AND_BREAKDOWN_2026-10-25.md
A	docs/CLASSIFICATION_SCORING.md
A	docs/COMPETITION_ADVANCEMENT_2026-10-22.md
A	docs/COMPETITION_AND_HILL_SEARCH_2026-10-22.md
A	docs/COMPETITION_FINISH_CRASH_2026-10-22.md
A	docs/COMPETITION_REFINEMENTS_2026-10-21.md
A	docs/COMPETITION_UX_2026-10-24.md
A	docs/DATA_PACKS.md
A	docs/DESKTOP_UX_12_11_2026.md
A	docs/DISTRIBUTION.md
A	docs/EDITOR_FIXES.md
A	docs/ENGINE_AUDIT_2026-10-24.md
A	docs/FIVE_DATA_FIXES_2026-10-22.md
A	docs/FORM_ENGINE.md
A	docs/FORM_ENGINE_WORK.md
A	docs/FORM_REGIME_PERKS.md
A	docs/FORM_SYSTEM.md
A	docs/HANDOFF_31_10_EARLY.md
A	docs/JUMPER_CATALOG_RESULT_SCROLL_2026-10-25.md
A	docs/JUMP_HISTORY_2026-10-25.md
A	docs/JUMP_REGULARITY_CALIBRATION.md
A	docs/KO_EDITOR_PERFORMANCE_2026-10-25.md
A	docs/KO_IDLE_REFRESH_2026-10-25.md
A	docs/KO_LL_MANUAL_ROUNDS_2026-10-25.md
A	docs/KO_SETUP_AND_PREJUMP_2026-10-24.md
A	docs/LATEST_JUMP_FEEDBACK_2026-10-24.md
A	docs/LAYOUT_RECURSION_FIX_2026-10-24.md
A	docs/LINUX_QT5_STARTUP_2026-10-27.md
A	docs/LOCALIZATION_AND_EDITORS.md
A	docs/MENU_AND_ANIMATIONS.md
A	docs/NAVIGATION_PERFORMANCE.md
A	docs/NEW_SIMULATION.md
A	docs/PENDING_CLASSIFICATION_RULES_2026-10-25.md
A	docs/PERFORMANCE-12-11-2026.md
A	docs/PERK_LEVELS_4.md
A	docs/PLAYGROUND_IMPLEMENTATION.md
A	docs/PLAYGROUND_V2.md
A	docs/PLAYGROUND_VALIDATION_2026-10-25.md
A	docs/QML_COMPETITION_LIVE.md
A	docs/QML_COMPETITION_PREVIEW.md
A	docs/QUICK_UI_MIGRATION.md
A	docs/RELEASE_NOTES_VS_MAIN.md
A	docs/RESOURCE_LIBRARY.md
A	docs/RESULT_HISTORY_ARCHIVE_DNS_2026-10-25.md
A	docs/RESULT_SELECTORS_TEAM_READABILITY_2026-10-25.md
A	docs/RESULT_VIEW_CONTROLS_COMPACT_TEAMS_2026-10-25.md
A	docs/RESUME-28-10-2026-simulation.MD
A	docs/RESUME_2026-10-22.md
A	docs/RESUME_2026-10-24_COMPETITION_UX.md
A	docs/RESUME_ENGINE_AUDIT_2026-10-24.md
A	docs/RESUME_UI_SCALE.md
A	docs/RESUME_WINDOWS_2026-10-30.md
A	docs/RESUME_WINDOWS_UI_2026-10-23.md
A	docs/ROUND_POINT_WEIGHTS.md
A	docs/SEASON_CHRONOLOGY.md
A	docs/SEASON_WORKSPACE_2026-10-22.md
A	docs/SIMULATION_ONBOARDING_2026-10-24.md
A	docs/SIMULATION_ORDER_AND_SAVE_SIZE_2026-10-25.md
A	docs/SIMULATION_PERFORMANCE_2026-10-22.md
A	docs/SIMULATION_PLAYGROUND.md
A	docs/SIMULATION_V2_ENGINE.md
A	docs/SIMULATION_V2_PROGRESS.md
A	docs/SPACE_AND_LAST_JUMP_2026-10-21.md
A	docs/START_LIST_TOOLS.md
A	docs/SUPER_TEAM_AUTO_SIMULATION_CRASH.md
A	docs/TEAM_KO_UX_2026-10-24.md
A	docs/TEAM_ORDER_LL_METHOD_PREVIEW_2026-10-25.md
A	docs/THREE_SMALL_FIXES_2026-10-22.md
A	docs/TRANSLATION_AUDIT_2026-10-28.md
A	docs/UI_MOTION_2026-10-29.md
A	docs/UX_IMPROVEMENTS_2026-10-21.md
A	docs/UX_REWORK_CHECKPOINT_2026-10-21.md
A	docs/WEATHER_RESEARCH.md
A	docs/WELCOME_FLOW.md
A	docs/WINDOWS_SETUP.MD
A	docs/WINDOWS_UI_AND_FIS_2026-10-23.md
A	docs/WIND_MEASUREMENT_RESEARCH.md
A	docs/WORK_QUEUE_31_10.md
A	docs/calibration/form-defaults-2026-11-10.json
A	docs/calibration/form-defaults-2026-11-11-seven-regimes-perks.json
A	docs/calibration/form-defaults-2026-11-11.json
A	docs/calibration/judges-2026-11-11.csv
A	docs/calibration/tuning-2026-11-11.md
A	docs/data/simulation-v2-rain-calibration.csv
A	docs/development/DEPLOY.md
A	docs/images/baza-danych.png
A	docs/images/menu.png
A	docs/reviews/2026-11-02-production-readiness.md
A	docs/reviews/2026-11-04-next-ten-production-issues.md
A	docs/reviews/2026-11-04-production-fixes.md
A	docs/reviews/2026-11-05-production-fixes.md
A	docs/work/2026-11-04-SJSIMMM.md
A	docs/work/2026-11-04-balance-sources.md
A	docs/work/2026-11-05-roster-board-production.md
A	examples/playground/form.json
A	examples/playground/randomness.json
A	examples/playground/takeoff-flight.json
A	examples/playground/wind-advantage.json
A	examples/playground/wind-physics.json
A	examples/playground/wind.json
A	img/author/konrad.jpg
A	packaging/linux/qmake-runtime-wrapper.py
A	packaging/linux/sj-sim.desktop
A	packaging/linux/sjsim-pack.xml
A	packaging/windows/SjSim.iss
A	scripts/analyze-playground.py
A	scripts/analyze-rain-v2.sh
A	scripts/benchmark-season-performance.py
A	scripts/build-qml-preview.sh
A	scripts/build-quick-ui.sh
A	scripts/check-translations.py
A	scripts/convert-four-perk-levels.py
A	scripts/extract-app-icon.py
A	scripts/fetch-linuxdeploy.py
A	scripts/generate-athlete-patches.py
M	scripts/package-linux.sh
M	scripts/package-windows.ps1
A	scripts/run-qml-preview.sh
A	scripts/run-quick-ui.sh
A	scripts/test-balance-lab-cli.py
A	scripts/test-form-engine.sh
A	scripts/test-four-perk-levels.py
A	scripts/test-playground-cli.py
A	scripts/test-quick-ui-windows.py
A	scripts/test-quick-ui.sh
A	scripts/test-resource-library.py
A	scripts/test-simulation-v2-core.ps1
A	scripts/test-simulation-v2-core.sh
A	scripts/test-windows-installer.ps1
A	scripts/update-release-inventory.py
A	scripts/with-qt-runtime.sh
M	sj-sim.ico
M	sj-sim.png
M	sj-sim.pro
A	skills-lock.json
M	source/UI/AppSettings/AppSettingsWindow.cpp
M	source/UI/AppSettings/AppSettingsWindow.h
M	source/UI/AppSettings/AppSettingsWindow.ui
M	source/UI/AppSettings/CountriesEditorTableModel.cpp
M	source/UI/AppSettings/JumpDataInfoChoiceDialog.cpp
M	source/UI/AppSettings/JumpDataInfoChoiceDialog.ui
A	source/UI/AuthorWindow.cpp
A	source/UI/AuthorWindow.h
M	source/UI/Competition/CompetitionConfigWindow.cpp
M	source/UI/Competition/CompetitionConfigWindow.h
M	source/UI/Competition/CompetitionConfigWindow.ui
M	source/UI/Competition/CompetitionManagerWindow.cpp
M	source/UI/Competition/CompetitionManagerWindow.h
M	source/UI/Competition/CompetitionManagerWindow.ui
M	source/UI/Competition/JumperCompetitionResultsWidget.cpp
M	source/UI/Competition/JumperCompetitionResultsWidget.h
M	source/UI/Competition/JumperCompetitionResultsWidget.ui
A	source/UI/Competition/Quick/ActionButton.qml
A	source/UI/Competition/Quick/CompetitionConfig.qml
A	source/UI/Competition/Quick/CompetitionLive.qml
A	source/UI/Competition/Quick/CompetitionLiveBridge.cpp
A	source/UI/Competition/Quick/CompetitionLiveBridge.h
A	source/UI/Competition/Quick/CompetitionQuickBridge.cpp
A	source/UI/Competition/Quick/CompetitionQuickBridge.h
A	source/UI/Competition/Quick/Field.qml
A	source/UI/Competition/Quick/LiveArena.qml
A	source/UI/Competition/Quick/LiveButton.qml
A	source/UI/Competition/Quick/LiveChoice.qml
A	source/UI/Competition/Quick/LiveDashboard.qml
A	source/UI/Competition/Quick/LiveJumpPreview.qml
A	source/UI/Competition/Quick/LiveNode.qml
A	source/UI/Competition/Quick/LiveTable.qml
A	source/UI/Competition/Quick/competition-quick.qrc
A	source/UI/Competition/Quick/fields.json
M	source/UI/Competition/Results/KOGroupResultsTableModel.cpp
M	source/UI/Competition/Results/ResultsTableModel.cpp
M	source/UI/Competition/Results/TeamResultsTreeModel.cpp
M	source/UI/Competition/StartList/StartListModel.cpp
M	source/UI/Competition/StartList/StartListModel.h
M	source/UI/Competition/TeamsSquadsTreeModel.cpp
M	source/UI/Competition/TeamsSquadsTreeModel.h
M	source/UI/Competition/TeamsSquadsTreeView.cpp
M	source/UI/DatabaseEditor/DatabaseEditorWindow.cpp
M	source/UI/DatabaseEditor/DatabaseEditorWindow.h
M	source/UI/DatabaseEditor/DatabaseItemsListView.cpp
M	source/UI/DatabaseEditor/DatabaseItemsListView.h
M	source/UI/DatabaseEditor/ListModels/CalendarsListModel.cpp
M	source/UI/DatabaseEditor/ListModels/CalendarsListModel.h
M	source/UI/DatabaseEditor/ListModels/CompetitionRulesListModel.cpp
M	source/UI/DatabaseEditor/ListModels/CompetitionRulesListModel.h
M	source/UI/DatabaseEditor/ListModels/JumpersListsListModel.cpp
M	source/UI/DatabaseEditor/ListModels/JumpersListsListModel.h
M	source/UI/DatabaseEditor/ListModels/PointsForPlacesPresetsListModel.cpp
M	source/UI/DatabaseEditor/ListModels/PointsForPlacesPresetsListModel.h
M	source/UI/DatabaseEditor/ListModels/SeasonHillsListModel.cpp
M	source/UI/DatabaseEditor/ListModels/SeasonHillsListModel.h
M	source/UI/DatabaseEditor/ListModels/SeasonJumpersListModel.cpp
M	source/UI/DatabaseEditor/ListModels/SeasonJumpersListModel.h
M	source/UI/EditorWidgets/CharacteristicsEditor.cpp
M	source/UI/EditorWidgets/ClassificationEditorWidget.cpp
M	source/UI/EditorWidgets/ClassificationEditorWidget.h
M	source/UI/EditorWidgets/CompetitionRulesEditorWidget.cpp
M	source/UI/EditorWidgets/CompetitionRulesEditorWidget.h
A	source/UI/EditorWidgets/ConfigSelections.h
M	source/UI/EditorWidgets/Dialogs/CharacteristicInputDialog.cpp
M	source/UI/EditorWidgets/HillEditorWidget.cpp
M	source/UI/EditorWidgets/JumperEditorWidget.cpp
M	source/UI/EditorWidgets/JumperEditorWidget.h
M	source/UI/EditorWidgets/JumpersListEditorWindow.h
M	source/UI/EditorWidgets/JumpersListEditorWindow.ui
M	source/UI/EditorWidgets/KOSystem/KOGroupEditorWidget.cpp
M	source/UI/EditorWidgets/KOSystem/KOGroupsListView.cpp
M	source/UI/EditorWidgets/KOSystem/KOGroupsListView.h
M	source/UI/EditorWidgets/PointsForPlacesPresetEditorWidget.cpp
M	source/UI/EditorWidgets/PointsForPlacesPresetEditorWidget.h
M	source/UI/EditorWidgets/RoundInfoEditorWidget.cpp
M	source/UI/EditorWidgets/RoundInfoEditorWidget.h
M	source/UI/EditorWidgets/RoundInfoEditorWidget.ui
A	source/UI/EditorWidgets/SimulationProfileEditor.cpp
A	source/UI/EditorWidgets/SimulationProfileEditor.h
M	source/UI/EditorWidgets/TeamsEditing/Models/PointsForPlacesTableModel.cpp
M	source/UI/EditorWidgets/TeamsEditing/Models/PointsForPlacesTableModel.h
M	source/UI/EditorWidgets/TeamsEditing/Models/TeamJumpersListModel.cpp
M	source/UI/EditorWidgets/TeamsEditing/TeamEditorWidget.cpp
A	source/UI/EditorWidgets/WeatherSetupEditor.cpp
A	source/UI/EditorWidgets/WeatherSetupEditor.h
A	source/UI/EditorWidgets/WindMeasurementChoice.h
M	source/UI/EditorWidgets/WindsGeneratorSettingsEditorWidget.cpp
M	source/UI/EditorWidgets/WindsGeneratorSettingsWidgetInputItem.cpp
M	source/UI/EditorWidgets/WindsGeneratorSettingsWidgetInputItem.h
M	source/UI/FormGenerator/JumperFormGeneratorsSettingsTableModel.cpp
M	source/UI/FormGenerator/JumpersFormGeneratorConfigWindow.cpp
M	source/UI/FormGenerator/JumpersFormGeneratorConfigWindow.h
M	source/UI/HelpWindow.ui
M	source/UI/JumpManipulation/JumpManipulatorConfigWindow.cpp
M	source/UI/JumpManipulation/JumpManipulatorConfigWindow.h
A	source/UI/Library/ResourceLibraryWindow.cpp
A	source/UI/Library/ResourceLibraryWindow.h
A	source/UI/Quick/ActionBar.qml
A	source/UI/Quick/ActionMenu.qml
A	source/UI/Quick/AnalysisPlot.qml
A	source/UI/Quick/AppFooter.qml
A	source/UI/Quick/AppHeader.qml
A	source/UI/Quick/AppearanceMenu.qml
A	source/UI/Quick/AppearancePanel.qml
A	source/UI/Quick/AthleteBadge.qml
A	source/UI/Quick/AthleteBadgeRack.qml
A	source/UI/Quick/AthleteCharacter.qml
A	source/UI/Quick/AthleteCollector.qml
A	source/UI/Quick/AthleteCopy.qml
A	source/UI/Quick/AthleteDesign.qml
A	source/UI/Quick/AthleteEditor.qml
A	source/UI/Quick/AthleteEquipped.qml
A	source/UI/Quick/AthleteIdentity.qml
A	source/UI/Quick/AthleteInsignia.qml
A	source/UI/Quick/AthleteLanding.qml
A	source/UI/Quick/AthleteLibrary.qml
A	source/UI/Quick/AthleteMedals.qml
A	source/UI/Quick/AthletePatchRack.qml
A	source/UI/Quick/AthletePatchSet.qml
A	source/UI/Quick/AthletePatches.qml
A	source/UI/Quick/AthletePerkArt.qml
A	source/UI/Quick/AthletePerkCatalog.qml
A	source/UI/Quick/AthletePositionPlot.qml
A	source/UI/Quick/AthleteRating.qml
A	source/UI/Quick/AthleteResultsTable.qml
A	source/UI/Quick/AthleteStatistics.qml
A	source/UI/Quick/AthleteStudio.qml
A	source/UI/Quick/AthleteText.qml
A	source/UI/Quick/AthleteTokens.qml
A	source/UI/Quick/AthleteViewChoice.qml
A	source/UI/Quick/AuthorPage.qml
A	source/UI/Quick/AuthorProject.qml
A	source/UI/Quick/AuthorSocial.qml
A	source/UI/Quick/BalanceLabControls.qml
A	source/UI/Quick/BalanceLabGenerator.qml
A	source/UI/Quick/BalanceLabParameter.qml
A	source/UI/Quick/BalanceLabPlot.qml
A	source/UI/Quick/BalanceLabProfiles.qml
A	source/UI/Quick/BalanceLabReplay.qml
A	source/UI/Quick/BalanceLabResults.qml
A	source/UI/Quick/BalanceLabScreen.qml
A	source/UI/Quick/BalanceLabTargets.qml
A	source/UI/Quick/BalanceLabWind.qml
A	source/UI/Quick/CalendarClassificationScoring.qml
A	source/UI/Quick/CalendarClassifications.qml
A	source/UI/Quick/CalendarDateField.qml
A	source/UI/Quick/CalendarDateOverview.qml
A	source/UI/Quick/CalendarDatePlanner.qml
A	source/UI/Quick/CalendarEventRow.qml
A	source/UI/Quick/CalendarFields.qml
A	source/UI/Quick/CalendarInspector.qml
A	source/UI/Quick/CalendarVenueButton.qml
A	source/UI/Quick/CalendarWeekendPlanner.qml
A	source/UI/Quick/CalendarWorkspace.qml
A	source/UI/Quick/CatalogWorkspace.qml
A	source/UI/Quick/ChangeFeedback.qml
A	source/UI/Quick/CheckHelp.qml
A	source/UI/Quick/ClassificationContributionPage.qml
A	source/UI/Quick/ClassificationFilters.qml
A	source/UI/Quick/ClassificationInput.qml
A	source/UI/Quick/ClassificationMethods.qml
A	source/UI/Quick/ClassificationPoints.qml
A	source/UI/Quick/ClassificationSources.qml
A	source/UI/Quick/ClassificationWorkspace.qml
A	source/UI/Quick/ContentHeight.qml
A	source/UI/Quick/CountryOrderDialog.qml
A	source/UI/Quick/DataPackBar.qml
A	source/UI/Quick/DataTable.qml
A	source/UI/Quick/DropdownIndicator.qml
A	source/UI/Quick/EmptyState.qml
A	source/UI/Quick/FormDiagnostics.qml
A	source/UI/Quick/FormGeneratorWorkspace.qml
A	source/UI/Quick/FormLabParameter.qml
A	source/UI/Quick/FormLabRoster.qml
A	source/UI/Quick/FormLabScreen.qml
A	source/UI/Quick/FormPlotChooser.qml
A	source/UI/Quick/FormStatus.qml
A	source/UI/Quick/HelpDot.qml
A	source/UI/Quick/HillEditor.qml
A	source/UI/Quick/HillLibrary.qml
A	source/UI/Quick/HillNumber.qml
A	source/UI/Quick/HillPerkCatalog.qml
A	source/UI/Quick/HillPerkChoice.qml
A	source/UI/Quick/HillStudio.qml
A	source/UI/Quick/HistoryControls.qml
A	source/UI/Quick/Home.qml
A	source/UI/Quick/HomeCard.qml
A	source/UI/Quick/HomeHero.qml
A	source/UI/Quick/HomeTool.qml
A	source/UI/Quick/HomeTools.qml
A	source/UI/Quick/HomeWelcome.qml
A	source/UI/Quick/JudgeMarks.qml
A	source/UI/Quick/JumpAdjustmentFields.qml
A	source/UI/Quick/JumpJudgesEditor.qml
A	source/UI/Quick/JumpManipulationWorkspace.qml
A	source/UI/Quick/JumpMetric.qml
A	source/UI/Quick/JumpWeights.qml
A	source/UI/Quick/JumpWindProfile.qml
A	source/UI/Quick/KoAthletePicker.qml
A	source/UI/Quick/KoDragArea.qml
A	source/UI/Quick/KoGroupsWorkspace.qml
A	source/UI/Quick/KoMemberSlot.qml
A	source/UI/Quick/MenuEntrance.qml
A	source/UI/Quick/ModalScreen.qml
A	source/UI/Quick/NewSimulationPage.qml
A	source/UI/Quick/Node.qml
A	source/UI/Quick/NodeGroup.qml
A	source/UI/Quick/NumberField.qml
A	source/UI/Quick/PerkStrengthSelector.qml
A	source/UI/Quick/PlaygroundAnalysis.qml
A	source/UI/Quick/PlaygroundField.qml
A	source/UI/Quick/PlaygroundHistogram.qml
A	source/UI/Quick/PlaygroundLive.qml
A	source/UI/Quick/PlaygroundMetric.qml
A	source/UI/Quick/PlaygroundNumber.qml
A	source/UI/Quick/PlaygroundReplay.qml
A	source/UI/Quick/PlaygroundResults.qml
A	source/UI/Quick/PlaygroundScreen.qml
A	source/UI/Quick/PlaygroundSetup.qml
A	source/UI/Quick/PlaygroundTargets.qml
A	source/UI/Quick/PlotPanel.qml
A	source/UI/Quick/PointsPresetEditor.qml
A	source/UI/Quick/PopupFade.qml
A	source/UI/Quick/ProjectedControl.qml
A	source/UI/Quick/QuickBridge.cpp
A	source/UI/Quick/QuickCalendar.cpp
A	source/UI/Quick/QuickCalendar.h
A	source/UI/Quick/QuickCalendarDates.cpp
A	source/UI/Quick/QuickPlot.cpp
A	source/UI/Quick/QuickPlot.h
A	source/UI/Quick/QuickRaceStats.h
A	source/UI/Quick/QuickRoster.cpp
A	source/UI/Quick/QuickRoster.h
A	source/UI/Quick/QuickSurface.cpp
A	source/UI/Quick/QuickSurface.h
A	source/UI/Quick/QuickTableModel.cpp
A	source/UI/Quick/QuickUi.cpp
A	source/UI/Quick/QuickUi.h
A	source/UI/Quick/QuickView.h
A	source/UI/Quick/QuickWindProfile.h
A	source/UI/Quick/RaceAthlete.qml
A	source/UI/Quick/RaceContestMenu.qml
A	source/UI/Quick/RaceControls.qml
A	source/UI/Quick/RaceLastJump.qml
A	source/UI/Quick/RaceLiveSettings.qml
A	source/UI/Quick/RaceScreen.qml
A	source/UI/Quick/RaceStatus.qml
A	source/UI/Quick/RaceToolGroup.qml
A	source/UI/Quick/RaceViewChoices.qml
A	source/UI/Quick/RaceViewControls.qml
A	source/UI/Quick/RaceViewSelector.qml
A	source/UI/Quick/ReorderHandle.qml
A	source/UI/Quick/ResourceLibrary.qml
A	source/UI/Quick/ResultsHistoryPage.qml
A	source/UI/Quick/RosterBoard.qml
A	source/UI/Quick/RosterBoardAthlete.qml
A	source/UI/Quick/RosterBoardLane.qml
A	source/UI/Quick/RosterCard.qml
A	source/UI/Quick/RosterDragArea.qml
A	source/UI/Quick/RosterLibrary.qml
A	source/UI/Quick/RosterLimits.qml
A	source/UI/Quick/RosterOrder.qml
A	source/UI/Quick/RosterPresets.qml
A	source/UI/Quick/RosterRow.qml
A	source/UI/Quick/RosterSourceDialog.qml
A	source/UI/Quick/RosterSummary.qml
A	source/UI/Quick/RosterTools.qml
A	source/UI/Quick/RosterWorkspace.qml
A	source/UI/Quick/SButton.qml
A	source/UI/Quick/SCheck.qml
A	source/UI/Quick/SChoice.qml
A	source/UI/Quick/SField.qml
A	source/UI/Quick/SIconButton.qml
A	source/UI/Quick/SToolButton.qml
A	source/UI/Quick/SavedRosterGrip.qml
A	source/UI/Quick/SavedRosterList.qml
A	source/UI/Quick/SavedRosterPane.qml
A	source/UI/Quick/SavedRosterWorkspace.qml
A	source/UI/Quick/Screen.qml
A	source/UI/Quick/SeasonAutomationToggle.qml
A	source/UI/Quick/SeasonAutosave.qml
A	source/UI/Quick/SeasonBar.qml
A	source/UI/Quick/SeasonFooter.qml
A	source/UI/Quick/SeasonOverview.qml
A	source/UI/Quick/SeasonRosterChoice.qml
A	source/UI/Quick/SeasonRostersWorkspace.qml
A	source/UI/Quick/SeasonUpcomingEvent.qml
A	source/UI/Quick/SectionRail.qml
A	source/UI/Quick/SectionTabs.qml
A	source/UI/Quick/SetupFooter.qml
A	source/UI/Quick/SimulationDraftWorkspace.qml
A	source/UI/Quick/SimulationPreferences.qml
A	source/UI/Quick/SingleJumpsResultsScreen.qml
A	source/UI/Quick/SortCountryButton.qml
A	source/UI/Quick/StudioEditActions.qml
A	source/UI/Quick/StudioPackBar.qml
A	source/UI/Quick/Surface.qml
A	source/UI/Quick/TableRow.qml
A	source/UI/Quick/TeamRosterWorkspace.qml
A	source/UI/Quick/Theme.qml
A	source/UI/Quick/TrackMark.qml
A	source/UI/Quick/VersionBadge.qml
A	source/UI/Quick/ViewArrival.qml
A	source/UI/Quick/WeatherAdvanced.qml
A	source/UI/Quick/WeatherNumber.qml
A	source/UI/Quick/WeatherPrecipitation.qml
A	source/UI/Quick/WeatherPresetChoice.qml
A	source/UI/Quick/WeatherPresetGrid.qml
A	source/UI/Quick/WeatherTuning.qml
A	source/UI/Quick/WeatherWorkspace.qml
A	source/UI/Quick/WindDirectionDial.qml
A	source/UI/Quick/WindEditor.qml
A	source/UI/Quick/WindFields.qml
A	source/UI/Quick/WindMeasurementComparison.qml
A	source/UI/Quick/WindSectorStrip.qml
A	source/UI/Quick/WindSectorsCompact.qml
A	source/UI/Quick/WindSlopePreview.qml
A	source/UI/Quick/WindStationEditor.qml
A	source/UI/Quick/Workspace.qml
A	source/UI/Quick/assets/welcome/athletes.png
A	source/UI/Quick/assets/welcome/calendar.png
A	source/UI/Quick/assets/welcome/competition.png
A	source/UI/Quick/assets/welcome/overview.png
A	source/UI/Quick/fonts/Barlow-Medium.ttf
A	source/UI/Quick/fonts/Barlow-Regular.ttf
A	source/UI/Quick/fonts/Barlow-SemiBold.ttf
A	source/UI/Quick/fonts/BarlowCondensed-ExtraBold.ttf
A	source/UI/Quick/fonts/IBMPlexMono-Regular.ttf
A	source/UI/Quick/fonts/OFL-Barlow.txt
A	source/UI/Quick/fonts/OFL-BarlowCondensed.txt
A	source/UI/Quick/fonts/OFL-IBMPlexMono.txt
A	source/UI/Quick/icons/athlete/ai/README.md
A	source/UI/Quick/icons/athlete/ai/flight.png
A	source/UI/Quick/icons/athlete/ai/prompts.json
A	source/UI/Quick/icons/athlete/ai/reserve.png
A	source/UI/Quick/icons/athlete/ai/small.png
A	source/UI/Quick/icons/athlete/ai/steady.png
A	source/UI/Quick/icons/athlete/ai/training.png
A	source/UI/Quick/icons/athlete/ai/volatile.png
A	source/UI/Quick/icons/athlete/athlete.svg
A	source/UI/Quick/icons/athlete/coin-flight-0.svg
A	source/UI/Quick/icons/athlete/coin-flight-1.svg
A	source/UI/Quick/icons/athlete/coin-flight-2.svg
A	source/UI/Quick/icons/athlete/coin-reserve-0.svg
A	source/UI/Quick/icons/athlete/coin-reserve-1.svg
A	source/UI/Quick/icons/athlete/coin-reserve-2.svg
A	source/UI/Quick/icons/athlete/coin-small-0.svg
A	source/UI/Quick/icons/athlete/coin-small-1.svg
A	source/UI/Quick/icons/athlete/coin-small-2.svg
A	source/UI/Quick/icons/athlete/coin-steady-0.svg
A	source/UI/Quick/icons/athlete/coin-steady-1.svg
A	source/UI/Quick/icons/athlete/coin-steady-2.svg
A	source/UI/Quick/icons/athlete/coin-training-0.svg
A	source/UI/Quick/icons/athlete/coin-training-1.svg
A	source/UI/Quick/icons/athlete/coin-training-2.svg
A	source/UI/Quick/icons/athlete/coin-volatile-0.svg
A	source/UI/Quick/icons/athlete/coin-volatile-1.svg
A	source/UI/Quick/icons/athlete/coin-volatile-2.svg
A	source/UI/Quick/icons/athlete/flight.svg
A	source/UI/Quick/icons/athlete/formStable.svg
A	source/UI/Quick/icons/athlete/formVolatile.svg
A	source/UI/Quick/icons/athlete/medal-flight-0.svg
A	source/UI/Quick/icons/athlete/medal-flight-1.svg
A	source/UI/Quick/icons/athlete/medal-flight-2.svg
A	source/UI/Quick/icons/athlete/medal-reserve-0.svg
A	source/UI/Quick/icons/athlete/medal-reserve-1.svg
A	source/UI/Quick/icons/athlete/medal-reserve-2.svg
A	source/UI/Quick/icons/athlete/medal-small-0.svg
A	source/UI/Quick/icons/athlete/medal-small-1.svg
A	source/UI/Quick/icons/athlete/medal-small-2.svg
A	source/UI/Quick/icons/athlete/medal-steady-0.svg
A	source/UI/Quick/icons/athlete/medal-steady-1.svg
A	source/UI/Quick/icons/athlete/medal-steady-2.svg
A	source/UI/Quick/icons/athlete/medal-training-0.svg
A	source/UI/Quick/icons/athlete/medal-training-1.svg
A	source/UI/Quick/icons/athlete/medal-training-2.svg
A	source/UI/Quick/icons/athlete/medal-volatile-0.svg
A	source/UI/Quick/icons/athlete/medal-volatile-1.svg
A	source/UI/Quick/icons/athlete/medal-volatile-2.svg
A	source/UI/Quick/icons/athlete/patch-flight-0.svg
A	source/UI/Quick/icons/athlete/patch-flight-1.svg
A	source/UI/Quick/icons/athlete/patch-flight-2.svg
A	source/UI/Quick/icons/athlete/patch-reserve-0.svg
A	source/UI/Quick/icons/athlete/patch-reserve-1.svg
A	source/UI/Quick/icons/athlete/patch-reserve-2.svg
A	source/UI/Quick/icons/athlete/patch-small-0.svg
A	source/UI/Quick/icons/athlete/patch-small-1.svg
A	source/UI/Quick/icons/athlete/patch-small-2.svg
A	source/UI/Quick/icons/athlete/patch-steady-0.svg
A	source/UI/Quick/icons/athlete/patch-steady-1.svg
A	source/UI/Quick/icons/athlete/patch-steady-2.svg
A	source/UI/Quick/icons/athlete/patch-training-0.svg
A	source/UI/Quick/icons/athlete/patch-training-1.svg
A	source/UI/Quick/icons/athlete/patch-training-2.svg
A	source/UI/Quick/icons/athlete/patch-volatile-0.svg
A	source/UI/Quick/icons/athlete/patch-volatile-1.svg
A	source/UI/Quick/icons/athlete/patch-volatile-2.svg
A	source/UI/Quick/icons/athlete/reserve.svg
A	source/UI/Quick/icons/athlete/small.svg
A	source/UI/Quick/icons/athlete/steady.svg
A	source/UI/Quick/icons/athlete/terrain.svg
A	source/UI/Quick/icons/athlete/training.svg
A	source/UI/Quick/icons/athlete/volatile.svg
A	source/UI/Quick/icons/discord.svg
A	source/UI/Quick/icons/github-dark.svg
A	source/UI/Quick/icons/github-light.svg
A	source/UI/Quick/icons/hill/compact.svg
A	source/UI/Quick/icons/hill/demanding.svg
A	source/UI/Quick/icons/hill/flying.svg
A	source/UI/Quick/icons/hill/forgiving.svg
A	source/UI/Quick/icons/hill/headwind.svg
A	source/UI/Quick/icons/hill/lottery.svg
A	source/UI/Quick/icons/hill/spacious.svg
A	source/UI/Quick/icons/hill/stable.svg
A	source/UI/Quick/icons/hill/tailwind.svg
A	source/UI/Quick/icons/hill/takeoff.svg
A	source/UI/Quick/icons/linkedin-dark.svg
A	source/UI/Quick/icons/linkedin-light.svg
A	source/UI/Quick/icons/tools/adjust.svg
A	source/UI/Quick/icons/tools/auto.svg
A	source/UI/Quick/icons/tools/coach.svg
A	source/UI/Quick/icons/tools/quick-simulate.svg
A	source/UI/Quick/icons/weather/calm.svg
A	source/UI/Quick/icons/weather/intensity-heavy.svg
A	source/UI/Quick/icons/weather/intensity-light.svg
A	source/UI/Quick/icons/weather/intensity-medium.svg
A	source/UI/Quick/icons/weather/lottery.svg
A	source/UI/Quick/icons/weather/none.svg
A	source/UI/Quick/icons/weather/pattern-increasing.svg
A	source/UI/Quick/icons/weather/pattern-steady.svg
A	source/UI/Quick/icons/weather/pattern-variable.svg
A	source/UI/Quick/icons/weather/profile.svg
A	source/UI/Quick/icons/weather/rain.svg
A	source/UI/Quick/icons/weather/skier-top.svg
A	source/UI/Quick/icons/weather/slope.svg
A	source/UI/Quick/icons/weather/snow.svg
A	source/UI/Quick/icons/weather/steady.svg
A	source/UI/Quick/icons/weather/strong.svg
A	source/UI/Quick/icons/weather/variable.svg
A	source/UI/Quick/qmldir
A	source/UI/Quick/quick-ui.qrc
M	source/UI/ResponsiveWindowUtils.h
M	source/UI/ResultsShowing/JumpDataDetailedInfoWindow.cpp
M	source/UI/ResultsShowing/JumpDataDetailedInfoWindow.ui
M	source/UI/ResultsShowing/JumpWindInfoWidgetItem.cpp
A	source/UI/ResultsShowing/ResultsHistoryWidget.cpp
A	source/UI/ResultsShowing/ResultsHistoryWidget.h
M	source/UI/Seasons/CalendarEditor/CalendarEditorTableModel.cpp
M	source/UI/Seasons/CalendarEditor/CalendarEditorTableModel.h
M	source/UI/Seasons/CalendarEditor/CalendarEditorWidget.cpp
M	source/UI/Seasons/CalendarEditor/MultipleTrainingsEditDialog.cpp
M	source/UI/Seasons/CalendarEditor/MultipleTrialRoundsEditDialog.cpp
M	source/UI/Seasons/Classifications/ClassificationResultsTableModel.cpp
M	source/UI/Seasons/Classifications/ClassificationResultsTableModel.h
M	source/UI/Seasons/Classifications/ClassificationResultsTableView.cpp
M	source/UI/Seasons/Classifications/ClassificationResultsTableView.h
M	source/UI/Seasons/Classifications/ClassificationResultsTableView.ui
M	source/UI/Seasons/CompetitionInfoEditorWidget.cpp
M	source/UI/Seasons/ItemModels/ClassificationsArchiveListModel.cpp
M	source/UI/Seasons/ItemModels/CompetitionsArchiveListModel.cpp
M	source/UI/Seasons/ItemModels/CompetitionsArchiveListModel.h
M	source/UI/Seasons/ItemModels/SimulationSavesListModel.cpp
M	source/UI/Seasons/NewSeasonConfiguratorWindow.cpp
M	source/UI/Seasons/NewSeasonConfiguratorWindow.h
M	source/UI/Seasons/NewSimulationSaveConfigurationWindow.cpp
M	source/UI/Seasons/NewSimulationSaveConfigurationWindow.h
M	source/UI/Seasons/SimulationSaveInfoWidget.cpp
M	source/UI/Seasons/SimulationSaveManagerWindow.cpp
M	source/UI/Seasons/SimulationSaveManagerWindow.h
M	source/UI/Seasons/SimulationSaveManagerWindow.ui
M	source/UI/Seasons/SimulationSavesWindow.cpp
M	source/UI/Seasons/Stats/FilteringWidgets/ClassificationsCheckBoxesWidget.cpp
M	source/UI/Seasons/Stats/FilteringWidgets/ClassificationsCheckBoxesWidget.h
M	source/UI/Seasons/Stats/FilteringWidgets/CompetitionsRangeComboBoxesWidget.cpp
M	source/UI/Seasons/Stats/FilteringWidgets/CompetitionsRangeComboBoxesWidget.h
M	source/UI/Seasons/Stats/JumperStatsWindow.cpp
M	source/UI/Seasons/Stats/JumperStatsWindow.h
M	source/UI/Seasons/Stats/SimulationRatingsWindow.cpp
M	source/UI/Seasons/Stats/SimulationRatingsWindow.h
M	source/UI/SingleJumps/SingleJumpsConfigWindow.cpp
M	source/UI/SingleJumps/SingleJumpsConfigWindow.h
M	source/UI/SingleJumps/SingleJumpsResultsTableModel.cpp
M	source/UI/SingleJumps/SingleJumpsResultsWindow.cpp
M	source/UI/ThanksWindow.ui
M	source/UI/mainwindow.cpp
M	source/UI/mainwindow.h
M	source/UI/mainwindow.ui
A	source/athletes/AthleteProfile.cpp
A	source/athletes/AthleteProfile.h
A	source/athletes/AthleteStudioController.cpp
A	source/athletes/AthleteStudioController.h
A	source/balance/BalanceLabController.cpp
A	source/balance/BalanceLabController.h
A	source/balance/BalanceLabModel.cpp
A	source/balance/BalanceLabModel.h
A	source/balance/BalanceRuntime.cpp
A	source/balance/BalanceRuntime.h
M	source/competitions/AbstractCompetitionManager.cpp
M	source/competitions/AbstractCompetitionManager.h
A	source/competitions/CompetitionAdvancement.h
M	source/competitions/CompetitionInfo.cpp
M	source/competitions/CompetitionInfo.h
M	source/competitions/CompetitionManagers/IndividualCompetitionManager.cpp
M	source/competitions/CompetitionManagers/IndividualCompetitionManager.h
M	source/competitions/CompetitionManagers/KORoundManager.cpp
M	source/competitions/CompetitionManagers/KORoundManager.h
M	source/competitions/CompetitionManagers/TeamCompetitionManager.cpp
M	source/competitions/CompetitionResults.cpp
M	source/competitions/CompetitionResults.h
M	source/competitions/CompetitionRules.cpp
M	source/competitions/CompetitionRules.h
M	source/competitions/CompetitionSingleResult.cpp
M	source/competitions/CompetitionSingleResult.h
M	source/competitions/KOSystem/KOGroup.cpp
M	source/competitions/KOSystem/KOGroup.h
M	source/competitions/RoundInfo.cpp
M	source/competitions/RoundInfo.h
A	source/form-engine/DailyForm.h
A	source/form-engine/FormEngine.cpp
A	source/form-engine/FormEngine.h
A	source/form-engine/FormLab.cpp
A	source/form-engine/FormLab.h
A	source/form-engine/FormSession.cpp
A	source/form-engine/FormSession.h
A	source/form-engine/FormSimulation.cpp
A	source/form-engine/FormSimulation.h
M	source/form-generator/JumperFormGenerator.cpp
M	source/form-generator/JumperFormGenerator.h
M	source/form-generator/JumperFormGeneratorSettings.cpp
M	source/form-generator/JumperFormGeneratorSettingsPreset.cpp
A	source/global/AtomicJsonFile.h
M	source/global/Country.cpp
A	source/global/CountryCodes.cpp
A	source/global/CountryCodes.h
M	source/global/CountryFlagsManager.cpp
A	source/global/DataPack.cpp
A	source/global/DataPack.h
M	source/global/GlobalAppSettings.cpp
M	source/global/GlobalAppSettings.h
M	source/global/GlobalDatabase.cpp
M	source/global/GlobalDatabase.h
M	source/global/GlobalSimulationSettings.cpp
M	source/global/GlobalSimulationSettings.h
M	source/global/GlobalTranslators.cpp
M	source/global/GlobalTranslators.h
A	source/global/JsonGraphValidation.h
A	source/global/JsonImportError.h
M	source/global/JumpDataInfoChoice.cpp
M	source/global/JumpDataInfoChoice.h
M	source/global/JumpsImportancePreset.cpp
M	source/global/MyRandom.cpp
M	source/global/MyRandom.h
M	source/global/PointsForPlacesPreset.cpp
A	source/global/ResourceBundle.cpp
A	source/global/ResourceBundle.h
M	source/global/TreeItem.h
A	source/global/UserDataPaths.cpp
A	source/global/UserDataPaths.h
M	source/global/Uuid.h
A	source/hills/HillProfile.cpp
A	source/hills/HillProfile.h
A	source/hills/HillStudioController.cpp
A	source/hills/HillStudioController.h
M	source/main.cpp
A	source/playground/PlaygroundAnalysis.cpp
A	source/playground/PlaygroundConfig.cpp
A	source/playground/PlaygroundController.cpp
A	source/playground/PlaygroundController.h
A	source/playground/PlaygroundExperiment.h
A	source/playground/PlaygroundIo.cpp
A	source/playground/PlaygroundModel.cpp
A	source/playground/PlaygroundModel.h
A	source/playground/PlaygroundRunner.cpp
A	source/seasons/CalendarDateValidation.h
M	source/seasons/Classification.cpp
M	source/seasons/Classification.h
A	source/seasons/ClassificationScoring.cpp
A	source/seasons/ClassificationScoring.h
M	source/seasons/ClassificationSingleResult.cpp
M	source/seasons/ClassificationSingleResult.h
M	source/seasons/SaveJumpersList.cpp
M	source/seasons/SaveJumpersList.h
M	source/seasons/Season.cpp
M	source/seasons/Season.h
M	source/seasons/SeasonCalendar.cpp
M	source/seasons/SeasonCalendar.h
M	source/seasons/SeasonCalendarPreset.cpp
M	source/seasons/SeasonCalendarPreset.h
A	source/seasons/SeasonChronology.h
M	source/seasons/SimulationSave.cpp
M	source/seasons/SimulationSave.h
A	source/seasons/SimulationSaveListOrder.h
A	source/simulation-v2/Engine.cpp
A	source/simulation-v2/Engine.h
A	source/simulation-v2/FieldWindMeasurement.cpp
A	source/simulation-v2/FieldWindMeasurement.h
A	source/simulation-v2/FlightModel.cpp
A	source/simulation-v2/FlightModel.h
A	source/simulation-v2/GameSimulationBridge.cpp
A	source/simulation-v2/HillSkillCurve.cpp
A	source/simulation-v2/HillSkillCurve.h
A	source/simulation-v2/JumpDraft.cpp
A	source/simulation-v2/JumpDraft.h
A	source/simulation-v2/JumperPotential.h
A	source/simulation-v2/LandingModel.cpp
A	source/simulation-v2/LandingModel.h
A	source/simulation-v2/Manipulation.cpp
A	source/simulation-v2/Manipulation.h
A	source/simulation-v2/ModelRevision.h
A	source/simulation-v2/ProfileBridge.cpp
A	source/simulation-v2/ProfileBridge.h
A	source/simulation-v2/ProfileDraft.cpp
A	source/simulation-v2/ProfileDraft.h
A	source/simulation-v2/ProfileMigration.cpp
A	source/simulation-v2/ProfileMigration.h
A	source/simulation-v2/ProfilePackRepository.cpp
A	source/simulation-v2/ProfilePackRepository.h
A	source/simulation-v2/QualityModel.cpp
A	source/simulation-v2/QualityModel.h
A	source/simulation-v2/RandomSource.cpp
A	source/simulation-v2/RandomSource.h
A	source/simulation-v2/Scoring.cpp
A	source/simulation-v2/Scoring.h
A	source/simulation-v2/SimulationCatalog.cpp
A	source/simulation-v2/SimulationCatalog.h
A	source/simulation-v2/Weather.cpp
A	source/simulation-v2/Weather.h
A	source/simulation-v2/WeatherCalibration.cpp
A	source/simulation-v2/WeatherCalibration.h
A	source/simulation-v2/WeatherPresets.cpp
A	source/simulation-v2/WeatherPresets.h
A	source/simulation-v2/WeatherSetup.cpp
A	source/simulation-v2/WeatherSetup.h
M	source/simulator/Characteristic.cpp
M	source/simulator/Characteristic.h
A	source/simulator/ExperimentalSimulation.cpp
A	source/simulator/ExperimentalSimulation.h
M	source/simulator/Hill.cpp
M	source/simulator/Hill.h
M	source/simulator/JumpData.cpp
M	source/simulator/JumpData.h
M	source/simulator/JumpManipulator.cpp
M	source/simulator/JumpManipulator.h
M	source/simulator/JumpSimulator.cpp
M	source/simulator/JumpSimulator.h
M	source/simulator/Jumper.cpp
M	source/simulator/Jumper.h
M	source/simulator/Team.cpp
M	source/simulator/Team.h
M	source/simulator/Wind.cpp
M	source/simulator/WindsCalculator.cpp
M	source/simulator/WindsCalculator.h
M	source/simulator/wind-generation/WindsGenerator.cpp
M	source/simulator/wind-generation/WindsGenerator.h
M	source/single-jumps/SingleJumpsManager.cpp
M	source/single-jumps/SingleJumpsManager.h
M	source/utilities/ClassWithCharacteristics.cpp
M	source/utilities/ClassWithCharacteristics.h
M	source/utilities/functions.h
A	tests/appimage-package.py
A	tests/athlete-studio-regressions.h
A	tests/autoform-onboarding.cpp
A	tests/balance-lab-regressions.h
A	tests/calendar-date-validation.h
A	tests/calendar-dates-regressions.h
A	tests/calendar-inspector-space-regressions.h
A	tests/calendar-workspace-ux-regressions.h
A	tests/choice-popup-regressions.h
A	tests/classification-popup-regressions.h
A	tests/classification-regressions.h
A	tests/competition-advancement-regressions.h
A	tests/competition-quick-smoke.cpp
A	tests/competition-ties-gate-regressions.h
A	tests/config-defaults-regressions.h
A	tests/engine-audit-regressions.h
A	tests/form-engine-ui-regressions.h
A	tests/form-engine.cpp
A	tests/gate-preview-performance.cpp
A	tests/hill-studio-regressions.h
A	tests/ko-round-limits-regressions.h
A	tests/library-removal-editor.cpp
A	tests/live-presentation-regressions.h
A	tests/motion-feedback-regressions.h
A	tests/navigation-performance.h
A	tests/packaging-windows-script.ps1
A	tests/playground-regressions.h
A	tests/production-persistence-regressions.h
A	tests/qt-runtime-launcher.py
A	tests/quick-season-regressions.h
A	tests/quick-ui-smoke.cpp
A	tests/race-auto-gate-regressions.h
A	tests/resource-library.cpp
A	tests/resource-removal.cpp
A	tests/roster-drag-regressions.h
A	tests/roster-performance-regressions.h
A	tests/roster-translation-regressions.h
A	tests/roster-workspace-regressions.h
A	tests/rule-catalog-regressions.h
A	tests/save-load-performance.cpp
A	tests/season-auto-performance.cpp
A	tests/season-chronology-regressions.h
A	tests/season-library-sync.cpp
A	tests/season-performance-regressions.cpp
A	tests/season-presentation-regressions.h
A	tests/simulation-onboarding-regressions.h
A	tests/simulation-v2-bridge-regressions.h
A	tests/simulation-v2-catalog-regressions.h
A	tests/simulation-v2-competition-regressions.h
A	tests/simulation-v2-contest-weather-regressions.h
A	tests/simulation-v2-engine.cpp
A	tests/simulation-v2-hill-spread.cpp
A	tests/simulation-v2-manipulation-regressions.h
A	tests/simulation-v2-measurement-regressions.h
A	tests/simulation-v2-quality.cpp
A	tests/simulation-v2-rain-analysis.cpp
A	tests/simulation-v2-random.cpp
A	tests/simulation-v2-repository-regressions.h
A	tests/simulation-v2-season-regressions.h
A	tests/simulation-v2-setup-regressions.h
A	tests/simulation-v2-weather-setup-regressions.h
A	tests/simulation-v2-weather.cpp
A	tests/svg-assets.cpp
A	tests/team-eligibility.cpp
A	tests/team-roster-layout.cpp
A	tests/team-weekend-regressions.h
A	tests/ui-duplication-regressions.h
A	tests/ux-roster-input-regressions.h
A	tests/weather-customization.cpp
A	tests/welcome-layout.cpp
A	transfer-30-10/README.md
A	transfer-30-10/skoczkowie/EST_KaimarVagul.png
A	transfer-30-10/skoczkowie/GER_PiusPaschke.png
A	transfer-30-10/skoczkowie/POL_DawidKubacki.png
A	transfer-30-10/skoczkowie/ZDJECIA_ZRODLA.md
A	transfer-30-10/skoczkowie/pack.json
A	transfer-30-10/skocznie/pack.json
A	transfer-30-10/snapshot.json
A	translations/translation_cs.ts
M	translations/translation_en.ts
A	userData/GlobalDatabase/calendarPresets/Official-LGP.json
A	userData/GlobalDatabase/calendarPresets/Official-WC.json
```

Statystyka Git: `1121 files changed, 202388 insertions(+), 11707 deletions(-)`.
