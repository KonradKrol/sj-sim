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
