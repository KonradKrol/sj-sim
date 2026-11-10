# Praca od 5 listopada — zakończone zakresy

Gałąź `feature/simulation-v2`. Zmiany odwoływania, pogody i interfejsu zostały zaimplementowane i sprawdzone przez analizę kodu, build oraz kontrolę tłumaczeń. Na prośbę właściciela nie uruchamiano testów dla tego zakresu.

- Wyjście z niezakończonego wydarzenia przywraca stan sprzed startu; osobna akcja odwołuje wydarzenie po potwierdzeniu. Odwołane kwalifikacje przepuszczają wszystkich. Przyszły konkurs pozwala nadal zmienić zasady i źródło kwalifikacji.
- Podgląd sektorów po skoku jest dokowany na szerokim ekranie, a na mniejszym otwierany z pola wiatru. Można go wyłączyć w ustawieniach i w konkursie. Diagnostykę pomiaru odblokowuje Ctrl+Shift+D w ustawieniach aplikacji.
- Przejście do kolejnej serii przesuwa czas pogody o 15 minut. Odczekanie 30 sekund jest dostępne bezpośrednio w konkursie.
- Karta następnego zawodnika dostosowuje wysokość do zawartości również między seriami.

Przy dalszej pracy warto ręcznie sprawdzić szczególnie wyjście i odwołanie w indywidualnym, drużynowym i KO oraz wygląd sektorów na małym laptopie. Automatyczne testy nie były uruchamiane.

## Dodatkowa poprawka — odrzucanie zmian bazy danych

„Nie zapisuj” przywraca teraz także pliki paczek skoczków/skoczni i katalogu bazy danych z początku sesji. Nowo utworzone pliki JSON są usuwane, a Escape korzysta z tego samego potwierdzenia. Dotychczasowe zapisy na bieżąco pozostają zabezpieczeniem przed awarią; jawne odrzucenie zmian cofa je. Build i kontrola tłumaczeń przeszły, bez uruchamiania testów.

## Kalibracja not sędziowskich

- Zwykłe odchylenie pojedynczego sędziego: 0,28 zamiast 0,25.
- Szansa rzadkiej dodatkowej odchyłki -3..+2: 0,002 na notę; regulacja `landing.judgeOutlierChance`.
- Oceny powyżej 19,25 są łagodnie ściskane współczynnikiem 0,75 przed zaokrągleniem (`landing.highMarkCompression`).
- Dwie nogi: losowa kara 2..4, średnia 2,6 (rozkład beta); podpórka: 4..7,2, średnia 5,6 (trójkątny); upadek: 7..9, średnia 8 (trójkątny). Losowanie kary jest wspólne dla skoku, a rozrzut sędziów zwiększony odpowiednio o 30%, 65%, 50%. Parametry `landing.landingPenalty.N` sterują średnią i proporcjonalnie skalują rozkład.
- Różnice bazowych not klas lądowania zmniejszone o 20% wokół klasy „Poprawne”.

## Łagodniejsza zmienność i podgląd prognozy

Ręczne ustawienie zmian w czasie używa odcinkowej krzywej zamiast kwadratu: dla poziomów 0/25/50/75/100 amplituda skali wynosi 0/0,065/0,25/0,40/0,55. Na górnym końcu ograniczono także częstość podmuchów i szybkość zmian. Presety i bazowe różnice przestrzenne nie zostały zmienione. Panel sektorów pozwala przełączać prognozę przed skokiem i zapis po skoku; kliknięcie wiatru w górnym pasku otwiera prognozę.

## Trzy metody pomiaru wiatru

Wybór zasad i lab mają tylko Domyślny (ID 3), Ulepszony (ID 4), Idealny (ID 5). Domyślny zachowuje dawną ważoną metodę „FIS — przybliżenie”. Ulepszony używa tej samej sieci, wag i okna czasu, ale odrzuca czujniki za lądowaniem. Idealny to dokładna średnia z faktycznej trasy, z normalnymi przelicznikami — właściciel jawnie wybrał to zamiast kompensacji porównującej lot z wiatrem i bez niego. Dawne ID 0/1/2 przy odczycie zasad przechodzą na Domyślny; ID 4 oznacza teraz Ulepszony. Dodano pomoc przy polu w zasadach, pojedynczych skokach i labie. Nie obiecywać stuprocentowego wyrównania wpływu wiatru przez Idealny: to dokładny pomiar, a nie idealna rekompensata punktowa.

## Zabezpieczenia sortowania, belka i wybór presetu

- Wyłączono wybór pustych klasyfikacji do kolejności startowej, zachowano blokadę po stronie handlera. Funkcje sortowania indywidualnego/drużynowego obsługują brak źródła i wyników. Dodatkowe zabezpieczenia: brak drużyny w wyniku, null w katalogu klasyfikacji, nieprawidłowy indeks podglądu klasyfikacji i wyszukiwanie identyfikatora null.
- Most Qt Quick respektuje teraz wyłączone pozycje comboboxów; wcześniej pozwalał wybierać je mimo blokady w modelu Widgets.
- W nowym silniku obniżenie przez trenera już wpływało na fizyczną belkę. Poprawiono zapis numeru belki w karcie skoku na rzeczywiście użyty, zgodnie ze starszym silnikiem.
- Okna wyboru z dropdownem rezerwują większą wysokość, aby lista presetów i inne wybory miały miejsce.
- Przycisk Sugeruj przy belce dobiera ją do aktualnych warunków i najmocniejszego z pozostałych zawodników, celując w real HS. Pokazuje zmianę i Cofnij; cofnięcie wygasa po skoku lub ręcznej zmianie. Uwzględnia i przywraca wcześniejsze ustawienie trenera.
- Bazowy mnożnik tylnego wiatru: 1,55 -> 1,6275 (+5%; skorygowane z początkowych +10%).

Dodatkowy nerf ręcznej zmienności: amplitudy dla 0/25/50/75/100 wynoszą teraz 0/0,06175/0,225/0,332/0,4125 skali bazowej, z interpolacją między punktami. Daje to kolejne -25% na maksimum i łagodniejsze redukcje niżej.

## Czytelniejszy podgląd wiatru i mniejszy wpływ perków

Podgląd sektorów i popup używają teraz wspólnego kompaktowego wykresu z linią zera, podpisanymi wartościami i przyciskami Przed/Po zamiast dropdowna. Wspólna skala obejmuje oba zestawy. Boczny wiatr jest w tooltipie sektora. Po nowym skoku panel przechodzi na zapis skoku; zamknięcie panelu nie wyłącza globalnej opcji, a kolejny skok przywraca panel. Prognoza dotyczy następnego skoku, co jest jawnie podpisane. Popup nie ma pustej przestrzeni o stałej wysokości ani zagnieżdżonego scrolla.

Automatyczny generator: wpływ perków kierunku i loteryjności osłabiony o 30% względem neutralnej skoczni. Nie zmienia ręcznej konfiguracji, stałych presetów ani innych efektów perków.

## Wspólne pola liczbowe i ponowna korekta podglądu

NumberField przechowuje wpis w lokalnym tekście, bez wiązania nadpisującego go przy odświeżeniach. Aktualizacje źródła są ignorowane podczas edycji; dane nowego rekordu nadal resetują pole. Walidacja następuje po zatwierdzeniu, a nie blokuje etapów wpisywania, takich jak pusty tekst, minus lub separator. Obsługiwane są kropka i przecinek, a zatwierdzane wartości respektują precyzję pola i zakres. Podgląd wiatru używa teraz dokładnie WindSlopePreview z konfiguracji przed konkursem, z prostym przełączaniem Przed/Po.

## Wspólny pasek akcji edytorów

Skoczkowie i skocznie używają StudioEditActions: duży kontrastowy przycisk z samym plusem, wspólne położenie Cofnij/Ponów/Duplikuj/Usuń, separator grup oraz czerwona ikona usuwania. Usunięto osobny przycisk Usuń z listy skoczków; skrót Delete pozostaje.

## Pływający podgląd przed skokiem

Nowsza decyzja właściciela: zlikwidowano automatyczny kafelek sektorów i przełącznik Przed/Po. Prognoza to niemodalny, przeciągany za nagłówek panel, pozostający otwarty przy klikaniu konkursu, z pozycją pamiętaną w ramach okna konkursu. Panel jest ograniczony do obszaru aplikacji i dostępny też z klawiatury. Zapis po skoku otwiera się wyłącznie z karty skoku. Oba widoki zachowują wykres ze zeskokiem z konfiguracji.

## Korekta prognozy i opcji konkursu

Nowsze wymagania: pasek konkursu pokazuje Prognoza wiatru, karta następnego zawodnika ma Podgląd wiatru. Przeciągana prognoza ma jeden nagłówek z kolorową średnią oraz zmniejszony widok zeskoku, bez dodatkowych opisów. Całkowicie usunięto otwieranie zapisu wiatru z karty skoku. Nowe menu Opcje konkursu na górze zawiera symulację konkursu/grupy/serii/N skoków oraz czerwone odwołanie rundy i konkursu; dostępność grupy/serii zależy od zasad. Warunki i Skok pozostają pełnymi przyciskami, bez ściskania do starego limitu szerokości.

## Pasek narzędzi konkursu

Skok jako panel opcji został usunięty. Nowe akcje mają wspólny styl SToolButton: DNS, trener -1 (natychmiastowe obniżenie tylko dla następnego zawodnika, wielokrotne kliknięcia kumulują obniżenie) i manipulacja. Manipulacja oraz belka trenera mają zaznaczony aktywny stan. Dobór belki jest ikoną przy inpucie, cofnięcie również jest małe. Usunięto tooltip największego awansu oraz pozostały panel opcji (webhooki, szczegóły skoków i przełącznik sektorów). Nieaktualne globalne przełączniki sektorów/diagnostyki usunięto z ustawień.

## Wysokość górnego paska sezonu

SeasonBar ma wysokość wyliczaną wyłącznie z zawartości; Loader nie pozwala jej rozciągać. Zmniejszono padding i wysokość akcji, Narzędzia używają kompaktowego przycisku. Dwa rzędy pojawiają się dopiero przy faktycznym braku szerokości na tekst i akcje; data, nazwa sezonu i kalendarz pozostają widoczne.

## Hotfix składni i układu pogody

Usunięto nieprawidłowy średnik po blokowym handlerze w RaceScreen, który uniemożliwiał załadowanie ekranu konkursu. WeatherWorkspace używa teraz zwykłego Item z wysokością zawartości zamiast Control/contentItem, eliminując cykl negocjacji implicitHeight. Brakujące zdjęcia i flagi w CompetitionManagerWindow nie są skalowane jako pusty QPixmap.

## Stabilniejszy pasek konkursu

Automatyczny dobór belki tylko zmienia numer — bez etykiety różnicy i cofania w UI. Pasek ma cztery grupy o naturalnej szerokości i wspólne delikatne obramowania zamiast osobnych ramek każdego przycisku. Jedno stałe miejsce na główną akcję (skok/przejście dalej), pozostałe akcje są wyszarzane zamiast znikać.

## Hotfix Flow i czytelne przyciski

Usunięto przypisanie do tylko do odczytu implicitHeight w Flow (blokowało RaceControls). WeatherWorkspace używa teraz bezpośredniego Flickable, bez automatycznego negocjowania rozmiaru przez ScrollView. Sprawdzono kompilację RaceScreen i WeatherWorkspace przez QQmlComponent z zasobami aktualnego buildu. Przycisk trenera ma napis TRENER. Każda akcja narzędziowa ma własne tło i obrys; usunięto dekoracyjne tła grup. Stan Na żywo jest kontrastowy.

## Pięć prostych ustawień symulacji

Nowa sekcja w ustawieniach gry: rozrzut wyników, znaczenie formy, trudność lądowania, siła perków i różnice między zawodnikami. Pięć poziomów, standard na środku, wspólne przywracanie standardu. Zapis w QSettings simulation/simple/*. BalanceRuntime::gameCapture nakłada wybory na aktywną kalibrację przy rozpoczynaniu rozgrywki; capture pozostaje surowym profilem laboratorium, więc jego zapis nie kumuluje mnożników. Rozpoczęte konkursy zachowują zapisany snapshot. Rozrzut skaluje odchylenia jakości; forma zmienia udział formy; lądowanie przesuwa krzywe ryzyka; perki skalują efekty i odchylenia względem neutralnych; różnice zawodników zmieniają tylko wpływ różnic OVR względem 70 (quality.overallSpread), nie losowość. Ekstremalne profile z laboratorium są ograniczane do poprawnych zakresów silnika.

## Zbiorcze usuwanie zapisów symulacji

Ekran zapisów obsługuje Ctrl+klik, zakres Shift+klik i zaznaczanie wszystkich z menu listy. Przycisk Usuń pokazuje liczbę wybranych zapisów; Delete wykonuje tę samą akcję. Jedno potwierdzenie z listą nazw w szczegółach, domyślnie anulowane. Otwarcie dostępne tylko dla jednego zaznaczenia. Usuwanie od końca modelu, czyszczenie wskaźnika szczegółów przed zwalnianiem obiektów i zachowanie pozycji, których plików nie udało się usunąć, z komunikatem zbiorczym. Dla niezaładowanych zapisów używana faktyczna ścieżka pliku. W Qt Quick usunięto zdublowane Otwórz z paska listy, wyłączono sortowanie i schowano wyszukiwanie pod przyciskiem; główne akcje to Nowa symulacja, Otwórz, Usuń. Pomoc obok akcji wyjaśnia zaznaczanie.

## 30 nowych miejsc pomocy i opis laboratorium balansu

Dodano kontekstowe „?” w 30 miejscach (bez pojedynczych skoków): rodzaj konkursu; rekompensata wiatru; rekompensata belki; noty sędziowskie; reguła 95% HS; dopuszczenie DSQ; aktualizacja rekordu; wielkość drużyny; liczba serii; zmienność generatora formy; limit uczestników serii; sortowanie startu; sortowanie po grupach drużyn; KO; wielkość grupy KO; bezpośredni awans z grupy; dobór grup; belka startowa; częstość DSQ; kolejność według klasyfikacji; lista według poprzednich zawodów; rok odniesienia paczki; rekord życiowy; klasa lądowania; zakres edycji kopii w rozgrywce; rodzaj klasyfikacji; nieznany wiek; aktywna lista; wyłączność we wspólnym edytorze list; data kalendarza. Teksty są krótkie, opisują efekt lub przykład. Dodatkowy większy „?” przy laboratorium balansu w ustawieniach wyjaśnia, że narzędzie powstało z pomocą AI głównie dla autora, jest rozbudowane i opcjonalne. Przycisk pomocy działa również po kliknięciu i z klawiatury. Dodano EN/CS. Build i kompilacja komponentów QML przeszły; bez uruchamiania zestawów testów.

## Trzy motywy kolorystyczne, sześć wariantów

W Wygląd i dostępność są trzy bezpośrednio wybierane motywy z próbkami kolorów i oznaczeniem wyboru: Aktualny (dotychczasowy), Różowy (intensywna fuksja, śliwkowe panele) i Czysty (stonowane szarości, morski akcent). Niezależny przełącznik jasnego/ciemnego daje łącznie sześć wariantów. QuickAppearance.colorTheme zapisuje wybór w quickUi/colorTheme (0–2); dotychczasowi użytkownicy zachowują wariant 0. Theme.qml jest centralnym źródłem palet: tła, tekst, akcenty, zaznaczenie, nagłówki i wykresy; kolory znaczenia wyników i ostrzeżeń pozostają semantyczne. Domyślny kształt Surface i promień narożników są lekko dopasowane do motywu. Zmiany działają od razu bez restartu. Sprawdzono podstawowe pary tekst/tło we wszystkich sześciu paletach (minimum 4,5:1), build, kompilację QML i kompletność EN/CS.

## Motywy obejmują formę, nie tylko paletę

Rozszerzono Theme.qml o treatments — wspólne definicje geometrii paneli, promieni kontrolek, obrysów i fokusu, wagi i rozstrzelenia tekstu akcji oraz reakcji na naciśnięcie. Różowy: miękkie asymetryczne panele, mocniejsze obrysy, zaokrąglone kontrolki i aktywne zakładki, cięższa typografia akcji, lekko sprężysty feedback. Czysty: małe symetryczne narożniki, proste ramki, spokojniejsze przejścia. Aktualny zachowuje dotychczasową geometrię. QuickSurface przyjmuje ogólne parametry zaokrąglenia, asymetrii i limitu narożników z Theme; dzięki temu także panele z własnym corner/sweep reagują na motyw. Podłączone wspólne Surface, SButton, SToolButton, SField, SChoice, SCheck, SectionTabs, ActionMenu, HomeCard oraz panele lądowania i perków. Nie zwiększono odstępów ani gęstości tabel. Ograniczenie animacji nadal wyłącza przejścia. Build i kompilacja QML przeszły. Obejrzano render rzeczywistych wspólnych komponentów w tymczasowym podglądzie: Różowy jasny/ciemny i Czysty jasny; bez modyfikowania danych użytkownika.

## 10 listopada — kalendarz i przegląd

Punkty 1–4: wyszukiwanie w wyborze skoczni weekendu i playgroundu oraz natywnych pełnych listach skoczni/skoczków; dwa osobne przyciski × ustawiają Brak przy źródle awansu z konkursu/klasyfikacji (bez otwierania dropdowna). Zmniejszono wybory trybu szybkiego planowania i dnia kwalifikacji. Przegląd siedmiu zawodów używa SeasonUpcomingEvent: jeden wyróżniony następny konkurs, spokojniejsze dalsze wiersze, czytelna data i krótkie statusy treningów/próbnej. Build i kompilacja QML przeszły; EN/CS uzupełnione.

Pozostała bieżąca prośba 10 listopada: 5–6 ukrywanie informacji wyłączonych zasadami i pełne respektowanie unowocześnionych preferencji karty skoku; 7 rozdzielenie relacji stopniowej od automatycznego oddawania skoków, małe modalne ustawienia w konkursie (czas lotu i oczekiwanie); 8 pięć not ujawnianych kolejno z losowymi przerwami przez losowe 4–7 sekund; 9 wiatr i rekompensaty chwilę po odległości.

## 10 listopada — karta skoku i dwa tryby relacji (punkty 5–9 zakończone)

Karta skoku respektuje reguły i preferencje osobno: brak not, sumy not i rekompensat wyłączonych w zasadach, bez pozostawiania pustych etykiet tych pól. Dane pobierane z właściwego JumpData, więc ponowne włączenie opcji nie wymaga nowego skoku. Ustawienia → Baza danych i narzędzia → Informacje o skoku: nowy zestaw opcji (wynik, miejsce, wiatr, belka, lądowanie, poszczególne rekompensaty, suma zawodnika, strata do lidera, wagi/reset, jakość nowego silnika, natężenie opadów). Usunięto z tego UI stare wybicie/lot/losowość i szczegółowe sektory. Podstawowe pola domyślnie włączone dla nowych konfiguracji; jawne istniejące wybory użytkownika zachowane. Przywracanie ustawień resetuje też nowe preferencje i czasy relacji.

Pasek konkursu: Relacja ▾ (Automat ▾ podczas automatycznych skoków) otwiera małe modalne ustawienia. Stopniowe pokazywanie skoku jest niezależne od automatycznego oddawania kolejnych skoków. Czas lotu w UI: 3/5/7/10 s; przerwa automatu: 1/3/5/10/15/30 s. Czasy w QSettings quickUi/liveFlightSeconds i liveWaitSeconds; automat uruchamiany dla bieżącego konkursu i zatrzymywany na końcu grupy/serii. Otwarte ustawienia oraz podgląd wyników wstrzymują automat. Zbiorcza symulacja, odwołanie i zamknięcie zatrzymują go; blokady zapobiegają wejściu w inną symulację podczas trwającego skoku. Stary mnożnik szybkości ukryto.

Relacja: odległość, po 300 ms lądowanie, po kolejnych 400 ms wiatr, po kolejnych 400 ms rekompensaty. Następnie pięć not ujawnia się na stałych pięciu miejscach, w losowej kolejności i z nierównymi odstępami w oknie 4–7 sekund. Losowanie prezentacji nie zmienia samych not. Po ostatniej nocie 250 ms pauzy, po czym jednocześnie animowane przekreślenie dwóch odrzuconych not i końcowy wynik. DSQ (oraz pomijany DNS) bez stopniowej prezentacji. JudgeMarks używa stabilnego modelu i rezerwuje szerokość, zamiast dopisywać kolejne pola.

Dodatkowe uwagi właściciela: usunięto widoczne Teraz z przeglądu (pozostaje w opisie dostępności); duży Konfiguruj konkurs zastąpiono klikalnym nagłówkiem kafla i dyskretnym Przygotuj start ↗. Menu odwołania jest osobną kontrolką. Build, kompilacja QML, EN/CS i krótki przebieg kontrolny komponentów UI (losowe pozycje not, stała szerokość, przekreślenia, otwarcie/zamknięcie konfiguracji) przeszły. Nie jest to pełny test sezonu od początku do końca.

## Korekta zakresu szybkiego planowania

Na prośbę właściciela przywrócono dotychczasowy wygląd głównych wyborów szybkiego planowania (Dodaj kwalifikacje / Dodaj weekend / Ustal wiele terminów). Uspokojenie dotyczy wyłącznie dwóch opcji dnia w Dodaj kwalifikacje: małe klasyczne przyciski radiowe zamiast dużych kontrastowych zakładek. Wyszukiwanie skoczni i pozostałe niezależne poprawki zachowane. Build i kompilacja QML przeszły.

## Bieżące korekty właściciela po e3a91df — do realizacji

1. Zwykłe poziome przekreślenie not, prosta animacja. Nowa relacja: dla 10 s pomiar pośredni wiatru w 2,5 s, odległość i lądowanie w 5 s, 4 s not, finalny pomiar wiatru, na końcu stała nieskalowana 1 s na domknięcie wyniku i tabeli. Pozostałe etapy proporcjonalnie skalowane. DSQ nadal od razu. Uwidocznić Informacje o skoku w ustawieniach aplikacji.
2. Kalendarz: mocne, estetyczne rozróżnienie rozegranych, oczekujących oraz rozpoczętych przygotowań (trening/próbna). Nie tylko etykieta.
3. Przegląd sezonu: przywrócić ważną akcję jako przycisk Rozegraj, poprawić layout/puste wysokości po wejściu i zmianie skali, balans/kontrast/hierarchię bez dokładania opisów. Zastąpić wadliwe/brzydkie trzy kropki inną działającą akcją. Usunąć Pozostałe kraje; pokazać liczbę pozostałych konkursów bez kwalifikacji obok Rozegrano X/Y.
4. Usunąć wszystkie pozostałe przyciski/akcje webhooków z UI całej aplikacji.
5. Ukryć nagłówki tabeli klasyfikacji (poz./zawodnik/punkty) w przeglądzie sezonu.

## STOP na prośbę właściciela — 10 listopada

Powyższe korekty po e3a91df są zaimplementowane w bieżącym kodzie. Zatrzymanie nastąpiło po poprawnym buildzie, sprawdzeniu tłumaczeń EN/CS, kompilacji komponentów QML i krótkim przebiegu komponentów UI (stałe miejsca not, modal konfiguracji, kalendarz oraz brak rozciągania kart po zmianie skali). Nie wykonywano pełnego sezonu ani pełnego ręcznego przejścia konkursu.

Najważniejsze pliki do dalszej kontroli:
- CompetitionManagerWindow.cpp / QuickBridge.cpp / JudgeMarks.qml / RaceLiveSettings.qml: nowa relacja. Czas T obejmuje całość; etapy przed zamknięciem skalowane przez (T−1)/9, ostatnia sekunda stała. Dla 10 s: pośredni pomiar wiatru 2,5 s, odległość i lądowanie 5 s, noty do 9 s, finalny wiatr i rekompensaty, wynik/miejsce/tabela w 10 s. Pośredni pomiar pochodzi z pierwszej połowy fizycznego lotu i bieżącej metody pomiaru; przy braku pola pogodowego używa średniej prognozy i finalnego wiatru. DSQ nadal od razu. Przekreślenia poziome, pojedyncza animacja szerokości.
- QuickBridge.cpp: naprawiona liczba stron ustawień (wcześniej trzy sekcje przy dwóch zakładkach); Informacje o skoku dostępne w Symulacja. Ukryte nagłówki klasyfikacji w przeglądzie sezonu.
- SeasonOverview.qml / SeasonUpcomingEvent.qml: naturalne wysokości na Column zamiast rozciągających ColumnLayout, przycisk Rozegraj i bezpośrednie Odwołaj z istniejącym potwierdzeniem. Bez Pozostałe kraje; liczba pozostałych konkursów bez kwalifikacji. Siedem kolejnych wydarzeń. Skasowane odwołane wydarzenia nie wracają do kolejki.
- QuickCalendar.cpp / CalendarEventRow.qml: ukończone zawody — zielone tło, ✓ i status; przygotowania — osobny akcent z konkretnym etapem treningu/próbnej.
- UI webhooków: przyciski i etykiety ukryte w formularzach Widgets oraz odfiltrowane w QuickBridge, usunięty przycisk archiwum, usunięte wzmianki z pomocy/podziękowań. Wewnętrzne dawne metody i pola zapisu pozostają; nie są wystawione w UI.

Przy wznowieniu: przede wszystkim ręcznie sprawdzić nową sekwencję relacji w konkursie (także bez sędziów, DSQ i automat), działanie Rozegraj/Odwołaj oraz wygląd przeglądu i kalendarza na rzeczywistym zapisie i przy zmianie skali. Właściciel zatrzymał pracę z powodu limitu tokenów — nie rozpoczynać teraz kolejnych zmian.

## Wznowienie — 10 listopada: stabilne wymiary paska sezonu

Kontrola rzeczywistych okien przez QTest, na odizolowanych danych, ujawniła pętlę `SeasonBar.implicitHeight` po zmianie rozmiaru tekstu. Pasek otrzymał niezależne od przydzielonej wysokości wymiary grupy informacji i akcji. Przełącza je w jeden/dwa wiersze bez negocjowania wysokości przez nadrzędny GridLayout. W SToolButton zastąpiono układ z rozciąganymi pustymi elementami naturalnie wymiarowanym, wyśrodkowanym rzędem ikony i tekstu; usunięto kolejne sprzężenie szerokości przy zmianie skali.

Istniejący test workflow wybiera teraz widoczną kontrolkę, jeśli responsywny ekran ma jednocześnie ukrytą i widoczną kopię inspektora. Zachowano możliwość znalezienia ukrytej kontrolki, gdy nie ma widocznej kopii.

Sprawdzone: pełny `--workflow-only` (edycja kalendarza, narzędzia sezonu, zaznaczanie, cofanie, ochrona historii, rozmiary okna i duży tekst) bez ostrzeżeń o pętlach layoutu. Dodatkowy przebieg rzeczywistego przeglądu sezonu: 1440×900, 1024×640, 800×600, skala UI 50/70/100%, tekst do 140%, naturalne wysokości kart. Nie jest to ręczny sezon obejmujący rozegranie wszystkich zawodów.

### Działające odwołanie z przeglądu

Przycisk Odwołaj był widoczny, ale nieaktywny: QuickBridge szukał dynamicznej akcji w przeniesionym kontenerze formularza, mimo że przycisk należał bezpośrednio do okna sezonu. Wyszukiwanie odbywa się teraz w oknie właściciela. Dopisano `tests/season-presentation-regressions.h`, uruchamiany przez `scripts/test-quick-ui.sh --season-presentation-only`.

Test używa nowego modelu sezonu, czterech zawodników i ośmiu konkursów. Przez rzeczywiste kontrolki QML sprawdza: siedem pozycji przeglądu, dziewięć kombinacji rozmiaru/skali, otwarcie właściwych przygotowań przez Rozegraj, zachowanie nierozpoczętego wydarzenia po wyjściu, anulowanie potwierdzenia odwołania oraz zatwierdzenie odwołania z przejściem do następnego wydarzenia. Przebieg przeszedł bez ostrzeżeń layoutu i bez używania danych właściciela.
