# Źródła balansu gry i labu — diagnoza 4.11.2026

Na komputerze właściciela aktywny profil w `balance-lab/active.json` nosił nazwę
„Aktualne”. Nakładał 528 wartości na wbudowany profil w
`data/beta/default-balance.json`. Zapisy „Main” z 2.11, godz. 15:45 miały
identyczne zestawy 543 parametrów. Nie nadpisano aktywnego profilu ani zapisów.

Zapisany workspace odtwarzał również dawny wariant „Balans z gry”. Warianty
„Twoje zmiany” i „Balans z gry” różniły się następującymi wartościami:
- `landing.supportCentre`: Main 1,065; aktywne „Aktualne” 1,06;
- `landing.supportWidth`: Main 0,011; aktywne „Aktualne” 0,012.

Ponadto profil „Aktualne” nadpisywał nowszą kalibrację wbudowaną:
- `flight.windHeadAdverseLoss`: aktywne 0,5; wbudowane 0,7;
- `flight.windTailAdverseLoss`: aktywne 0,7; wbudowane 1,0;
- `flight.windCrossAdverseLoss`: aktywne 0,65; wbudowane 0,9.

Poprawka synchronizuje współczynniki wariantu „Balans z gry” z
`BalanceRuntime::capture()` przy odtwarzaniu workspace i zatwierdzaniu konfiguracji.
Własne i zapamiętane warianty pozostają zachowane. Przywrócenie standardowego
profilu nie dziedziczy już starych defaults eksperymentu.
UI pokazuje aktywną nazwę i liczbę różnic, a w zapisach jest akcja
„Kopiuj balans gry do zmian”. Sama ta akcja nie zmienia aktywnego profilu gry.

Aby użyć najnowszego balansu wbudowanego: „Przywróć domyślny balans gry”,
następnie opcjonalnie „Kopiuj balans gry do zmian”, aby porównać identyczne warianty.
Aby wrócić dokładnie do zapisanego Main: wczytać Main i wybrać
„Używaj tych ustawień w grze”. Rozpoczęte konkursy zachowują swoje snapshoty.
