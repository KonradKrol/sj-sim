# Opcjonalne daty zawodów

Daty włącza się przy tworzeniu zapisu albo w **Ustawieniach zapisu symulacji → Używaj dat zawodów w bieżącym sezonie**. Przycisk **Włącz daty zawodów** jest też dostępny w kalendarzu. Ustawienie dotyczy bieżącego sezonu i wszystkich jego kalendarzy. Nowy sezon dziedziczy wybór, który można zmienić przed jego rozpoczęciem.

Daty można wyłączyć w każdej chwili. Można je włączyć tylko przed rozpoczęciem sezonu: uruchomieniem pierwszych zawodów, rozegraniem lub odwołaniem wydarzenia. Blokada obejmuje wszystkie kalendarze sezonu, a nie tylko aktywny. Wyłączenie ukrywa daty i zachowuje zaplanowane terminy. Rozpoczęty sezon nie pozwala ponownie włączyć dat po ich wyłączeniu. Stan rozpoczęcia jest zapisywany niezależnie od listy zawodów.

Stare zapisy otwierają się bez dat. Można nadal grać całkowicie bez kalendarza dat. W trybie z datami nieuzupełnione terminy są oznaczone jako **Bez daty** i można zaplanować je później.

## Szybkie planowanie

1. Zaznacz konkursy na liście (Ctrl, Shift lub Ctrl+A), wybierz **Zaplanuj terminy**. Opcja **Wszystkie przyszłe konkursy** pomija rozegrane i rozpoczęte bloki, niezależnie od zaznaczenia.
2. Wybierz pierwszy termin z kalendarza lub wpisz datę. Pola przyjmują ISO `RRRR-MM-DD` i lokalny krótki format daty; wyświetlanie używa lokalnego formatu systemu.
3. Wybierz schemat:
   - **Weekendy** — najbliższa sobota nie wcześniejsza od daty początkowej, niedziela, kolejna sobota itd.
   - **Co tydzień** — zawsze ten sam dzień tygodnia.
   - **Co określoną liczbę dni** — odstęp 1–366 dni.
   - **Wszystkie tego samego dnia** — np. kwalifikacje i konkurs w jednym dniu.
   - **Nowy początek — zachowaj odstępy** — przenosi istniejący plan na nowy termin, zachowując wszystkie odstępy. Przydatne po wczytaniu presetu z innego sezonu. Wszystkie wybrane konkursy muszą mieć daty.
4. Opcjonalnie ustaw treningi 0–14 dni przed zawodami. Serie próbne odbywają się w dniu konkursu. Schemat zachowujący odstępy zachowuje także istniejące przesunięcia treningów i serii próbnych.
5. **Pokaż plan** wyświetla stare i nowe terminy bez zmiany danych. Dopiero **Zastosuj plan** zapisuje wszystkie daty w jednej operacji.

Każdy plan, przesunięcie, usunięcie dat i sortowanie można cofnąć i ponowić. Inna edycja kalendarza lub zmiana zaznaczenia unieważnia stary podgląd. Rozegrane wydarzenia i ich dołączone sesje pozostają zablokowane.

Przyciski **−7 / −1 / +1 / +7 dni** przesuwają zaznaczone bloki z zachowaniem istniejących odstępów. **Zaznacz konkursy bez daty** ułatwia uzupełnienie braków. Datę pojedynczego konkursu można też ustawić w jego podstawowych polach; ten wybór ustawia wspólny dzień konkursu, treningów i serii próbnej. Nowo dodane treningi i serie próbne dziedziczą dzień zawodów. Kopiowanie konkursu zachowuje daty.

## Widok sezonu

**Widok kalendarza** przełącza listę na miesiąc z tygodniami od poniedziałku. Widać weekendy, zawody, liczbę wydarzeń oraz podgląd wybranego dnia. Kliknięcie zawodów otwiera ich edycję; wybrany dzień można przypisać zaznaczonym konkursom. Nawigacja miesiącami, skok do konkretnej daty i przycisk pierwszych zawodów pozwalają poruszać się po wieloletnim planie. Przy wąskim oknie widok zmienia się w agendę miesięczną.

Kalendarz sygnalizuje brak dat, cofające się terminy, zawody w różnych miejscowościach jednego dnia oraz kwalifikacje lub treningi późniejsze od konkursu. Dwa konkursy w tej samej miejscowości jednego dnia są dozwolone. **Kolejność symulacji nadal wynika z listy**; daty nie przestawiają jej automatycznie. Polecenie **Uporządkuj przyszłe według dat** robi to jawnie, zachowuje kolejność remisujących dat i nie przenosi historii. Po nim należy sprawdzić powiązania kwalifikacji.

## Zapis i zakres

`scheduled-date` zawodów przechowuje opcjonalną datę ISO bez godziny i strefy czasowej. `dates-enabled` i `started` należą do sezonu. Daty treningów i serii próbnych są zapisywane przy tych wydarzeniach. Presety kalendarzy zachowują terminy, bez przenoszenia rozegranych wyników. Wybór grania z datami pozostaje ustawieniem sezonu, więc wczytanie presetu nie włącza go automatycznie.

Obliczenia używają `QDate`, dzięki czemu zmiana czasu nie pomija dni, a lata przestępne i granice miesięcy są uwzględniane. Daty na tym etapie służą planowaniu i podglądowi; odstępy między konkursami nie zmieniają rozwoju skoczków.

## Weryfikacja

Windows, Qt 6.8.3 / MSVC 2022, Release: `--backend-only` oraz natywny `--dates-only` zakończyły się kodem 0. Testy obejmują podgląd bez modyfikacji danych, weekendy na przełomie lat, rok przestępny, zmianę czasu, przesunięcia sesji, cofanie i ponawianie, blokady historii, zapis sezonu i presetów, sterowanie widocznym planerem i wybierakiem dat. Widok sprawdzono przy szerokościach 1440, 1024, 520 i 360 px, w jasnym i ciemnym motywie.
