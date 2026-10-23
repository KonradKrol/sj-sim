# Reset punktów i wagi poprzednich serii

W **Zasadach konkursu**, w ustawieniach każdej serii od drugiej wzwyż, dostępne jest pole **Resetuj punkty przed tą serią**. Zawodnicy, którzy awansowali, rozpoczynają tę serię z zerową notą łączną. Dotychczasowe skoki i ich oryginalne noty pozostają w historii. Odpadnięci zachowują swoje wyniki i miejsca za uczestnikami późniejszej serii.

Po rozwinięciu **Zaawansowane: własne wagi wcześniejszych serii** można ustawić osobny mnożnik dla każdej wcześniejszej serii. `0` pomija jej punkty, `1` uwzględnia całość, `0,5` połowę. Bieżąca seria zawsze liczy się w całości. Łączna nota jest zaokrąglana do 0,1 pkt.

Przykłady:

- Przed serią 3: wagi `0` dla serii 1 i `0,5` dla serii 2 dają `0 × P1 + 0,5 × P2 + P3`.
- Przed serią 4: wagi `0,2`, `0,2`, `0,2` dają `0,2 × P1 + 0,2 × P2 + 0,2 × P3 + P4`.
- Reset przed serią 2, a potem zwykła seria 3: wynik to `P2 + P3`. Punkty z pierwszej serii nie wracają.

Własne wagi odnoszą się do **oryginalnych punktów z konkretnych serii**, a nie do wcześniej przeważonej sumy. Jawne ustawienie ich w późniejszej serii może więc ponownie uwzględnić serię pominiętą wcześniej. Bez własnych wag następna seria dziedziczy dotychczasowe zasady sumowania. Reset ma pierwszeństwo nad wagami; włączenie resetu blokuje ich edycję, zachowując wartości na wypadek wyłączenia resetu.

Zasady obejmują konkursy indywidualne, KO i drużynowe. W drużynach waga dotyczy całej serii, obejmującej wszystkie grupy zawodników. Awans i kolejność startowa następnej serii są ustalane na podstawie zakończonej serii, przed przeliczeniem nowej noty.

Odwołanie bieżącej serii usuwa jej skoki i przywraca noty obliczone według ostatniej zakończonej serii, również wtedy, gdy bieżąca seria resetowała punkty lub zmieniała wagi.

Ustawienia są częścią `CompetitionRules`, więc są zapisywane również w presetach, kalendarzach i sezonach. Starsze dane bez nowych pól nadal sumują wszystkie serie normalnie. Nowe pola JSON w każdej serii to `reset-points` i `previous-rounds-weights`; wynik zapisuje dodatkowo `scoring-round`, aby zachować stan również przed pierwszym skokiem po resecie.

Testy: `sj-sim-ui-smoke --backend-only` sprawdza obliczenia, ranking, przejście serii, linię lidera, drużyny, edytor i serializację; `--round-points-only` sprawdza widoczny reset oraz rozwijane pola wag w interfejsie Quick i zapisuje obrazy dla różnych rozmiarów okna. `--round-points-bulk-only` symuluje rzeczywiste konkursy indywidualne, KO i drużynowe z resetem lub wagą `0,5`, a następnie odwołuje ostatnią serię i sprawdza przywrócone wyniki.

Weryfikacja 23.10.2026, Windows x64, Qt 6.8.3 / MSVC 2022, Release: regresje backendu i nawigacji zaliczone; natywne `--round-points-only` oraz `--round-points-bulk-only` zakończone kodem 0. Test interfejsu obejmuje oba motywy, szerokości od 360 do 1440, większy tekst, przewinięcie do pola, wpisanie ułamka klawiaturą i zatwierdzenie Tabem. Zwykłe symulacje bez modyfikacji punktów (`--bulk-only`) również zakończyły się kodem 0. Wdrożone exe uruchomiono bez deweloperskiego PATH Qt i zamknięto z kodem 0. Nie nadpisano istniejących baz i zapisów użytkownika.
