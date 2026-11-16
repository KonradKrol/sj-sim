# Sj.Sim 2.0.0

## Tekst do GitHub Release

Sj.Sim 2.0.0 to przebudowany interfejs, nowy silnik skoków i znacznie więcej możliwości prowadzenia sezonów. Poniżej zmiany względem 1.3.0-beta2.

### Nowy wygląd i wygodniejsza obsługa

- Nowy interfejs Qt Quick obejmujący menu, edytory, przygotowanie zawodów, konkursy, sezony i archiwum.
- Bardziej czytelna hierarchia informacji, kompaktowe panele i lepsze wykorzystanie miejsca na mniejszych ekranach.
- Jasny i ciemny wygląd oraz trzy motywy: standardowy, różowy i prosty. Regulowana wielkość interfejsu i tekstu oraz ograniczanie animacji.
- Przebudowane dialogi, nawigacja i komunikaty; więcej pomocy przy mniej oczywistych opcjach.
- Nowe logo, sekcja „O autorze” i dostęp do społeczności Discord bezpośrednio z gry.
- Czteroczęściowe powitanie oraz możliwość rozpoczęcia gotowej przykładowej symulacji z letnim i zimowym kalendarzem, datami, autoformą i autozapisem.
- „Pojedynczy konkurs” nazywa się teraz „Szybki konkurs”.

### Skoczkowie, skocznie i nowy silnik

- Nowy model skoczka: OVR, forma, pięć klas lądowania, rekord życiowy, zdjęcie i opcjonalny rok urodzenia.
- Cztery poziomy perków, przeciwstawne cechy i nowy edytor z naszywkami. Perki opisują podejście do treningów, specjalizację, regularność skoków i stabilność formy.
- Przebudowany edytor skoczni i perki wpływające m.in. na charakter lotny, różnice odległości, lądowanie i losowane warunki.
- Automatyczne wyliczanie punktów za metr i punkt K.
- Nowe obliczenia jakości skoku, wpływu OVR i formy, specjalizacji oraz zależności od rozmiaru skoczni.
- Przebudowana geometria lotu i dalekiego lądowania, rozrzut odległości oraz wpływ skoczni kompaktowych i rozległych.
- Zmienione noty sędziowskie: wpływ klasy lądowania i odległości, różnice między sędziami, kary za lądowanie na dwie nogi, podpórkę i upadek. Trudniej otrzymać komplet dwudziestek.
- Bardzo dobra forma daje mniejszą dodatkową premię zawodnikom o najwyższym OVR; słaba forma nadal pogarsza wynik bez tego ograniczenia.
- Proste ustawienia pozwalają dostosować m.in. losowość skoków, różnice w odległościach i zmienność formy sezonowej.

### Wiatr

- Nowy model warunków zmieniających się w czasie i wzdłuż zeskoku. Znaczenie ma również miejsce wystąpienia podmuchu.
- Automatyczne losowanie warunków uwzględniające charakterystykę skoczni, ponowne losowanie i dodatkowe scenariusze wiatrowe.
- Płynny wybór kierunku, niezależne ustawianie siły, zmienności w czasie i różnic na zeskoku oraz lokalne poprawki na podglądzie.
- Silny, stabilny wiatr i słaby, loteryjny wiatr mogą być odrębnymi sytuacjami.
- Prognoza przed skokiem z kompaktowym, przesuwanym podglądem. Pomiar po skoku uwzględnia upływ czasu, dlatego może różnić się od prognozy.
- Pomiar do rekompensaty pomija czujniki za miejscem lądowania. Nadal jest przybliżeniem, a nie idealnym wyrównaniem wszystkich skutków wiatru.
- Łagodniejsze automatycznie proponowane warunki i rzadsze skrajne loterie.

### Ekran konkursu

- Nowa karta skoku z wyraźnym miejscem, flagą, odległością, notami, rodzajem lądowania i składnikami wyniku.
- Podgląd skoków z poprzednich serii, także przez Shift+klik w odpowiednią kolumnę wyników, oraz informacje o punktach przenoszonych wagami.
- Ustawienia informacji wyświetlanych przy skoku; ukrywanie składników, których nie przewidują zasady konkursu.
- Wybór wyników po konkretnej serii, z możliwością powrotu do bieżącego konkursu.
- Subtelna kolumna strat do lidera w wynikach i klasyfikacjach, również w archiwum.
- Poprawione oznaczenia pewnego awansu, odpadnięcia, kwalifikacji do kolejnego konkursu i lucky loserów w aktualnej serii KO.
- Trzy zapamiętywane sposoby prezentowania wyników drużynowych oraz dodatkowy ranking indywidualny sumujący punkty skoczków.
- Oddzielne tryby stopniowego pokazywania informacji i automatycznego rozgrywania zawodów. Stopniowo ujawniane odległości, lądowanie, wiatr i noty; noty mają stałe miejsca i losową kolejność ujawniania.
- Przebudowane narzędzia: symulowanie wybranej liczby skoków, grupy, serii lub konkursu, czekanie 30 sekund, zmiana warunków i manipulowanie pojedynczym skokiem.
- Automatyczna belka, ręczny dobór sugerowanej belki i obniżanie przez trenera z potwierdzeniem wymogu 95% HS.
- Poprawione linie do prowadzenia i awansu, także po zmianie belki przez trenera; ukrywanie nierealistycznych odległości.
- Pominięcie skoku jako DNS. DSQ jest pokazywane jawnie; dyskwalifikacja w drużynówce obejmuje drużynę.
- Jawne odwoływanie konkursów i serii, oddzielone od zwykłego opuszczenia rozpoczętego konkursu.
- Podium dopiero po zakończeniu konkursu, z uwzględnieniem remisów.

### Listy startowe, drużyny i KO

- Przebudowany wybór uczestników, listy zawodników i widoki krajów.
- Narzędzia kopiowania list i sortowania według wyników zawodów, klasyfikacji, OVR, aktualnego potencjału, alfabetu i krajów, a także losowania i odwracania kolejności.
- Wygodniejszy wybór źródłowego konkursu lub klasyfikacji, również z wcześniejszych sezonów.
- Losowanie kolejności w obrębie remisów i zawodników bez przypisanego wyniku źródłowego.
- W drużynówkach wybór zawodników dostępnych do startu, respektowany przez ręczny i automatyczny dobór składów.
- Ukrywanie krajów, z których nie można skompletować drużyny, i zamiana miejsc zawodników przez przeciąganie.
- Edytor grup KO z wyszukiwaniem, przeciąganiem i możliwością ręcznego doboru także między seriami.
- Dodatkowe sposoby tworzenia grup, w tym losowanie koszykami bez rodaków oraz wyrównywanie potencjału grup.

### Sezony, kalendarze i klasyfikacje

- Przebudowany przegląd sezonu: najbliższe zawody, powiązane treningi i próbne, postęp rozgrywek oraz podium poprzedniego konkursu ze zdjęciami, jeśli są dostępne.
- Oddzielenie kalendarza edytowanego od aktywnego, prostsze dodawanie kalendarzy i lepsze puste stany.
- Przeciąganie konkursów i całych bloków, operacje na wielu wydarzeniach oraz Cofnij/Ponów.
- Planowanie terminów i całych weekendów piątek–sobota–niedziela z wariantami kwalifikacji, treningów i serii próbnych.
- Kontrola chronologii również pomiędzy kalendarzami przy aktywnych datach. Wydarzenia z tego samego dnia można rozgrywać w wybranej kolejności.
- Trwałe powiązania kwalifikacji po przesuwaniu konkursów i wyraźne blokowanie niedozwolonych działań.
- Wybór źródła sortowania listy startowej w kalendarzu. Serie próbne mogą dziedziczyć kolejność konkursu głównego, także KO.
- Automatyczny wybór domyślnych zasad po zmianie rodzaju wydarzenia na kwalifikacje, trening lub próbną.
- Odwołane kwalifikacje nie eliminują uczestników z konkursu głównego.
- Przebudowany edytor klasyfikacji: punkty za miejsca lub sumowanie not z zawodów, różne sposoby naliczania i filtry zaawansowane.
- Podgląd sposobów punktacji także po rozegraniu konkursów, z ochroną przed zmianą zasad już naliczonych wyników.
- Podgląd stanu klasyfikacji po wybranych zawodach oraz wkładu poszczególnych skoczków w punkty kraju.
- Nowe statystyki zawodnika z wykresem miejsc, szczegółami pod kursorem i filtrami treningów, próbnych, kwalifikacji i konkursów.
- Ujednolicony format dat, odległości i punktów oraz poprawiona synchronizacja archiwum.

### Automatyczna forma

- Forma może samodzielnie zmieniać się wraz z datami sezonu, także przy odwoływaniu wydarzeń.
- Siedem stanów dyspozycji i różne przebiegi formy poszczególnych zawodników.
- Perki stabilności i chwiejności zmieniają skalę wahań, czas trwania i prawdopodobieństwa stanów.
- Opisy formy od głębokiego kryzysu do absolutnego szczytu.
- Forma dnia dodaje krótkotrwałą zmianę dyspozycji; można ją wyłączyć w szybkim konkursie.
- Przy aktywnej autoformie ręczna edycja jest blokowana. Opcja „Pokaż formę” pozwala ukryć dyspozycję w edytorze; bez autoformy pozostaje ona zawsze widoczna.
- Osobne laboratorium formy do prób, wykresów i symulacji wielu przebiegów.

### Paczki i udostępnianie

- Uporządkowana biblioteka skoczków, skoczni, zasad, punktacji, kalendarzy i zapisów, domyślnie w folderze Dokumenty/SjSimData.
- Import i eksport paczek `.sjsim-pack`, również zawierających wiele zasobów i zdjęcia zawodników.
- Zdjęcia należą do paczki i nie wymagają dostępu do prywatnych folderów jej autora.
- Usuwanie zasobów z biblioteki z potwierdzeniem i możliwością odzyskania przez kosz systemowy, gdy jest dostępny.
- Zasoby startowe na nowej instalacji oraz gotowe kalendarze LGP 2023, 2024 i 2026 oraz PŚ 2023/24, 2024/25 i 2026/27.
- Aktualizacja programu zachowuje istniejącą bibliotekę i zapisy użytkownika.

### Narzędzia, stabilność i instalacja

- Eksperymentalny Balance Lab dostępny w ustawieniach: symulacje wielu konkursów, rozkłady wyników, porównania wariantów i obserwacja pojedynczych zawodników.
- Obliczenia eksperymentów w tle, postęp, anulowanie i podglądy liczone dopiero po włączeniu.
- Liczne poprawki crashy, m.in. przy pustych klasyfikacjach, sortowaniu, tworzeniu grup KO, zmianie zasad, zatwierdzaniu pól edytora i kończeniu konkursu.
- Poprawki zapisywania, przywracania danych i niezapisanych zmian, ochrony paczek, zaznaczeń oraz pozycji przewijania.
- Ograniczenie zbędnego przeliczania i przebudowywania ekranów; usprawnienia automatycznej symulacji, wczytywania i obsługi archiwum.
- Prostsza instalacja: instalator na Windows i pojedynczy AppImage na Linux. Biblioteki aplikacji są dołączone.
- Uzupełnienia tłumaczeń angielskich i czeskich oraz poprawki polskich komunikatów.

### Ważne przed aktualizacją

- **Nowy silnik wymaga nowych sezonów. Zapisy z 1.3.0-beta2 nie są w pełni kompatybilne z 2.0.0. Zachowaj kopię poprzedniej biblioteki.**
- Starsze paczki mogą wymagać migracji. Przybliżone przeliczenie profilu nie zachowuje dokładnie zachowania starego silnika; do udostępniania nowych zasobów używaj eksportu `.sjsim-pack`.
- Nowe zasoby startowe nie nadpisują paczek w już istniejącej bibliotece.
- Opady są tymczasowo wyłączone. Nie ma obecnie wyboru metody pomiaru wiatru — gra korzysta z jednego wariantu, pomijającego czujniki za miejscem lądowania.
- Laboratoria są narzędziami eksperymentalnymi. Tłumaczenia mogą jeszcze zawierać braki.
- Windows może ostrzec o nieznanym wydawcy, ponieważ aplikacja nie ma skonfigurowanego podpisu certyfikatem.

**Społeczność i paczki:** https://discord.gg/PsumKAqDBt

---

## Krótsza wersja na Discord

**Sj.Sim 2.0.0**

Nowa wersja to przebudowany interfejs i nowy silnik symulacji. Najważniejsze zmiany:

- **Nowy wygląd całej gry** — bardziej czytelne ekrany, kompaktowe panele, motywy i regulacja wielkości interfejsu.
- **Nowy model skoczków i skoczni** — OVR, forma, klasy lądowania, czteropoziomowe perki i przebudowane edytory.
- **Nowy wiatr i przebieg skoków** — zmienne warunki na zeskoku, prognoza, poprawiony pomiar i automatyczna belka.
- **Wygodniejsze zawody** — nowe wyniki, podgląd poprzednich serii, poprawki KO i drużynówek oraz stopniowe ujawnianie skoku.
- **Rozbudowane sezony** — wygodniejsze kalendarze, planowanie weekendów, klasyfikacje, archiwum i statystyki zawodników.
- **Automatyczna forma i forma dnia**, zmieniające dyspozycję w sezonie.
- **Łatwiejsze udostępnianie paczek** wraz ze zdjęciami oraz prostsza instalacja na Windows i Linux.
- Gotowy przykładowy sezon i wiele poprawek stabilności oraz wydajności.

**Uwaga: sezony z 1.3.0-beta2 nie są w pełni kompatybilne z nowym silnikiem. Zrób kopię danych i rozpocznij nową rozgrywkę.**

Gotowe kalendarze obejmują m.in. LGP 2026 i PŚ 2026/27. Własna biblioteka zostaje zachowana po aktualizacji.
