# Automatyczna forma i laboratorium

## W sezonie

Włącz daty wydarzeń, nadaj wydarzeniom daty i włącz **Autoformę** w przeglądzie sezonu, obok **Autozapisu**. Funkcja jest domyślnie wyłączona, również w dotychczasowych zapisach.

Pierwsze rozpoczęcie przygotowań zachowuje formę z bazy. Przy kolejnych datach silnik przelicza wszystkie dni przerwy dla wszystkich zawodników. Kilka wydarzeń tego samego dnia nie nalicza czasu ponownie. OVR nie zmienia się automatycznie.

Wyjście z przygotowań lub z nierozstrzygniętego konkursu przywraca wcześniejsze formy, bazy, okresy i stan losowania. Po rozegraniu wydarzenia nowy stan pozostaje w zapisie. Wyłączenie funkcji zatrzymuje zmiany; ponowne włączenie zaczyna od bieżącej daty bez nadrabiania przerwy.

Przy aktywnej funkcji edytor blokuje ręczne F. Pokazuje diagnostykę: dokładne F, bazę μ, okres dyspozycji i datę ostatniego przeliczenia. Wewnętrzne F zachowuje ułamki; profil wykorzystywany przez grę otrzymuje zaokrągloną wartość 1–100. Wyłączenie funkcji pozwala ponownie edytować formę ręcznie.

**Równa forma ↔ Chwiejna forma** to czwarta para perków, z czterema poziomami po obu stronach. Nie ogranicza pozostałych par i nie zastępuje regularności pojedynczych skoków. Bez automatycznej formy ten perk nie oddziałuje na skoki. Starsze profile otrzymują neutralną nową parę.

Zmiana kalendarza nigdy nie cofa już przetworzonej historii formy. Wydarzenie z wcześniejszą datą nie przelicza jej ponownie. Początek formalnego kolejnego sezonu nie resetuje automatycznie stanu.

## Laboratorium

Otwórz **Ustawienia → Symulacja → Laboratorium formy**.

1. Ustaw liczbę osób, formę początkową, perk, dni i seed albo wybierz paczkę. Paczka zachowuje indywidualne formy i perki.
   W zakładce **Zawodnicy** wybierzesz konkretne osoby, wyszukasz je po nazwisku/kraju i zmienisz startową formę oraz perk formy każdej osoby. Zmiany dotyczą tylko laba; przycisk ↶ przywraca dane z paczki. Próba wykorzystuje wyłącznie zaznaczonych zawodników. Pusta lista blokuje uruchomienie. Wybory i edycje pozostają po przełączeniu paczek w tym samym oknie.
2. Ustaw liczbę powtórzeń. Jedno to pojedyncza historia populacji; batch korzysta z niezależnych, deterministycznie wyprowadzonych seedów.
3. Uruchom. Wykresy pokazują trzy pierwsze osoby i średnią populacji z pierwszego powtórzenia. Statystyki i rozkład formy obejmują cały batch. ± przy wynikach oznacza przybliżony 95% przedział średniej z powtórzeń, z poprawką Studenta przy małych próbach.
4. W **Parametrach** dostroisz cały model. Wyszukiwanie obejmuje wszystkie grupy. Pomoc przy polu wyjaśnia działanie i jednostki. Pola akceptują przecinek i kropkę dziesiętną; zmiany zatwierdzasz Enterem lub opuszczeniem pola.

Porównanie: **Zachowaj A**, zmień parametry bieżącej konfiguracji B, zaznacz porównanie i uruchom. Obie konfiguracje używają tej samej populacji i wyjściowych seedów. Parametry początkowe próby są wspólne dla A i B. Wykres średniej populacji i rozkład pokazują obie konfiguracje; pozostałe wyniki przełączasz A/B.

Profil matematyczny można zapisać i wczytać jako JSON. Eksport wyników zawiera również konfiguracje A/B, seed, długość próby, liczbę powtórzeń oraz początkowe formy i perki całej populacji.

**Używaj w nowych rozgrywkach** zapisuje domyślny profil w katalogu danych aplikacji, w `form-engine/default.json`. Nowy zapis symulacji przejmuje jego kopię. Istniejący zapis zachowuje własną konfigurację, więc kalibracja w labie nie zmienia go w tle.

Obliczenia laba działają poza wątkiem interfejsu. Batch używa wybranej liczby wątków i pokazuje postęp osobodni. Limit próby to 50 mln osobodni; przerwanie zachowuje poprzednie wyniki.

## Zakres i kalibracja

Parametry domyślne są **robocze**. Nie wdrożono automatycznej oceny celu „3–6 zawodników ze 100 osiąga formę 90+”. Wskaźnik 90+ jest zwykłą statystyką wyników.

Nie ma generatora dla sezonów bez dat, masowych ręcznych zmian, zmęczenia, rozwoju OVR ani resetowania formy na początek sezonu. Nowe etykiety mają mechanizmy tłumaczeń, ale katalogi EN/CS nie zostały rozszerzone w ramach tego MVP.

Pełna specyfikacja: [FORM_SYSTEM.md](FORM_SYSTEM.md). Mapa implementacji i kontrole: [FORM_ENGINE_WORK.md](FORM_ENGINE_WORK.md).
