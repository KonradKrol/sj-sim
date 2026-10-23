# Super Team: crash po ręcznych skokach i auto-symulacji

Odtworzenie: pojedynczy konkurs z presetem Super Team (dwóch zawodników w drużynie, trzy serie), ręczny skok w pierwszej serii, następnie auto-symulacja reszty serii. Odświeżenie podglądu po zakończeniu serii kończyło się naruszeniem pamięci.

Wyniki zawodników drużyny są przebudowywane przez `updateTeamJumpersResults()`. Otwarty podgląd ostatniego ręcznego skoku przechowywał wskaźniki do poprzedniej zawartości tych wyników. Operacje automatyczne pokazywały ten sam podgląd po przebudowie danych, bez ponownego powiązania. Późniejsze odświeżenie QML odczytywało zwolniony `JumpData`.

Dziennik Windows wskazywał `0xc0000005`, RVA `0x9777a` w oryginalnym EXE. Mapa linkera wskazała konstruktor kopiujący `CompetitionRules`; zgodność sekcji `.text` oryginalnego EXE i wariantu z mapą została sprawdzona. Debugger odtworzonego scenariusza również zatrzymał się w tym konstruktorze podczas odroczonego odświeżenia UI.

Podczas auto-symulacji stary podgląd jest teraz ukryty. Po zakończeniu zmian wyników i odświeżeniu modeli jest ponownie wiązany z aktualnym wynikiem ostatniego symulowanego zawodnika. Poprawka obejmuje serię, grupę, określoną liczbę skoków i cały konkurs.

Przejście do kolejnej serii oraz odwołanie serii również przebudowują te bufory. W tych operacjach podgląd zachowuje tożsamość wybranego zawodnika, jest ukrywany przed zmianami i ponownie wiązany po ich zakończeniu. Jeżeli po odwołaniu nie ma już wyniku zawodnika, pozostaje ukryty.

Test `sj-sim-ui-smoke --super-team-only` używa prawdziwego presetu i drużyn z bazy. Przechodzi przez trzy serie z redukcją drużyn, miesza ręczne skoki ze wszystkimi czterema operacjami automatycznymi, sprawdza granice serii oraz kompletność wyników. Po operacjach sprawdza, że podgląd wskazuje element aktualnego bufora wyników, i wymusza odświeżenie QML. Wariant ręczny skok + auto-seria przed poprawką odtwarzał crash.

Windows, Qt 6.8.3/MSVC 2022, Release: test Super Team przeszedł z renderowaniem programowym i natywnym. Cały przebieg z przejściami między seriami przeszedł także pod debuggerem, kończąc się kodem `0x0`. Sprawdzono normalne uruchomienie i zamknięcie wdrożonego EXE.

Regresje natywne `--bulk-only`, `--round-points-bulk-only` i `--navigation-only` przeszły: indywidualny, KO, klasyczny drużynowy, auto-grupa/seria/liczba skoków/konkurs, reset i wagi punktów oraz odwołanie serii.
