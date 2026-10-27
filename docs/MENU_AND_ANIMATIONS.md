# Menu, języki i animacje

Dolna sekcja menu ma dwa kafle o wspólnej wysokości i szerokości: bazę danych oraz ustawienia. Wąskie okno układa je pionowo. Wysokość wynika z treści obu kafli, również przy powiększonym tekście. Nie ma stałej obwódki ani bocznego paska; obrys pojawia się przy nawigacji klawiaturą. Menu nie pokazuje numerów 01–03 ani skrótów Ctrl+1/2/3 i nie obsługuje tych skrótów.

Panel „Wygląd” zawiera wybór Polski / English / Čeština. Zmiana aktualizuje otwarte widoki i zapisuje ustawienie języka.

W obu edytorach siłę perka wybiera się trzema kompaktowymi, kwadratowymi polami. Jeden, dwa lub trzy podświetlone słupki oznaczają poziom; ciemne tło wskazuje aktualny wybór. Nazwy poziomów są dostępne w podpowiedziach i dla czytników ekranu. Ponowne wybranie aktywnego pola usuwa perk.

Przed pojedynczymi skokami wybrane są pierwszy dostępny skoczek i skocznia, a przed pojedynczym konkursem pierwsza skocznia oraz zestaw zasad. Te listy nie zawierają „BRAK” i nie pozostają bez wyboru, jeśli mają elementy. Pierwszy zapisany preset ważności skoków jest od razu stosowany; ręczna wartość ma osobną pozycję „Własne ustawienia”. Filtry statystyk opisują brak ograniczenia jako „Wszystkie skocznie”, a pola kalendarza jasno nazywają własne zasady i domyślną skocznię.

Wyszukiwane dropdowny przyjmują tekst już podczas animacji otwierania. Enter jest obsługiwany przy puszczeniu klawisza, a fokus wraca do kontrolki po zamknięciu popupu. Dzięki temu animowane zamykanie nie powoduje ponownego zatwierdzenia poprzedniej pozycji.

Pięć animacji wspierających obsługę:

1. Przesuwanie tła aktywnej zakładki w `SectionTabs`.
2. Zmiana koloru zaznaczenia na listach skoczków i skoczni.
3. Pojawianie się i zamykanie popupów list rozwijanych.
4. Krótkie podbicie ikonki po wybraniu siły perka skoczka.
5. Rozjaśnienie pola języka po udanej zmianie.

Dwa mocniejsze akcenty:

1. Kolejne wejście kafli menu z przesunięciem, a głównych kafli także z lekkim obrotem.
2. Złoty pierścień i promienie przy wybraniu trzeciej siły perka skoczka.

Efekty nie opóźniają zastosowania wyboru i nie blokują kolejnych kliknięć. Opcja „Ogranicz animacje” zatrzymuje efekty dekoracyjne i przywraca ich końcowy stan.

## Kolejne 15 animacji zmian stanu

1. Wjazd i rozjaśnienie nazwiska nowego skoczka na belce.
2. Odsłonięcie właśnie uzyskanego wyniku skoku.
3. Krótkie wyróżnienie zmiany serii lub grupy.
4. Rozjaśnienie zmienionej wartości w metrykach skoku, np. rekompensaty.
5. Kolejne odsłonięcie not sędziowskich po nowym skoku.
6. Płynne wskazanie dostępności przejścia do kolejnej serii.
7. Wygaszenie nieaktywnych części konkursu po wejściu do historii i ich przywrócenie po wyjściu.
8. Płynne wypełnienie określonego postępu symulacji.
9. Wyróżnienie nazwiska i numeru zawodnika po zmianie miejsca na liście startowej, z nieruchomymi obszarami kliknięcia.
10. Przesunięcia konkursów po zmianie kolejności kalendarza.
11. Rozwinięcie i zwinięcie szczegółów sektora wiatru.
12. Wstawienie i usunięcie pozycji na liście przypisanych perków.
13. Krótkie wyróżnienie pola liczbowego po odrzuceniu nieprawidłowego wpisu.
14. Przejście formularza przy zmianie edytowanego skoczka.
15. Przejście listy biblioteki przy zmianie paczki skoczków.

Wyniki, kolejność i wartości pól zmieniają się natychmiast w modelu; animacje pokazują już zatwierdzony stan. Informacje o skoku pozostają czytelne również podczas przejść. Nie ma zapętlonych efektów ozdobnych ani opóźnień przed obsługą kolejnego kliknięcia.
