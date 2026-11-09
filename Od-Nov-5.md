# Praca od 5 listopada — zakończone zakresy

Gałąź `feature/simulation-v2`. Zmiany odwoływania, pogody i interfejsu zostały zaimplementowane i sprawdzone przez analizę kodu, build oraz kontrolę tłumaczeń. Na prośbę właściciela nie uruchamiano testów dla tego zakresu.

- Wyjście z niezakończonego wydarzenia przywraca stan sprzed startu; osobna akcja odwołuje wydarzenie po potwierdzeniu. Odwołane kwalifikacje przepuszczają wszystkich. Przyszły konkurs pozwala nadal zmienić zasady i źródło kwalifikacji.
- Podgląd sektorów po skoku jest dokowany na szerokim ekranie, a na mniejszym otwierany z pola wiatru. Można go wyłączyć w ustawieniach i w konkursie. Diagnostykę pomiaru odblokowuje Ctrl+Shift+D w ustawieniach aplikacji.
- Przejście do kolejnej serii przesuwa czas pogody o 15 minut. Odczekanie 30 sekund jest dostępne bezpośrednio w konkursie.
- Karta następnego zawodnika dostosowuje wysokość do zawartości również między seriami.

Przy dalszej pracy warto ręcznie sprawdzić szczególnie wyjście i odwołanie w indywidualnym, drużynowym i KO oraz wygląd sektorów na małym laptopie. Automatyczne testy nie były uruchamiane.

## Dodatkowa poprawka — odrzucanie zmian bazy danych

„Nie zapisuj” przywraca teraz także pliki paczek skoczków/skoczni i katalogu bazy danych z początku sesji. Nowo utworzone pliki JSON są usuwane, a Escape korzysta z tego samego potwierdzenia. Dotychczasowe zapisy na bieżąco pozostają zabezpieczeniem przed awarią; jawne odrzucenie zmian cofa je. Build i kontrola tłumaczeń przeszły, bez uruchamiania testów.
