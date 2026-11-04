# Poprawki przed wydaniem — 4 listopada 2026

Wprowadzone po ręcznym sprawdzeniu ścieżek odczytu, zapisu i pakowania. Bez zmiany reguł symulacji.

| Problem | Skutek | Rozwiązanie |
| --- | --- | --- |
| Wyniki drużynowe rozwiązywały identyfikator składu przed rejestracją drużyn bieżącego wydarzenia | Brak składu lub wskazanie drużyny z innego konkursu po wczytaniu sezonu | Osobny kontekst drużyn podczas odczytu oraz przypięcie wyników do docelowych obiektów wydarzenia |
| Walidator odwołań odrzucał błędny typ, lecz przepuszczał nieistniejący obiekt | Ciche gubienie uczestników i aktywnego sezonu przy uszkodzonym zapisie | Sprawdzenie istnienia celów przed deserializacją, także list treningów i klasyfikacji |
| Dwie instancje GUI mogły zapisywać do tych samych katalogów danych | Nadpisywanie zmian drugiej instancji | Dwie blokady `QLockFile`, dla przenośnego `userData` i wspólnych profili w AppData, utrzymywane przez cały czas pracy GUI |
| Nieprawidłowy zapisany balans konkursu był pomijany | Odtworzenie rozpoczętego konkursu z innymi współczynnikami | Odrzucenie nieprawidłowego profilu w kontroli struktury zapisu; stare zapisy bez tego pola nadal są obsługiwane |
| Linux sprawdzał tekstowy prefiks ścieżki przed `rm -rf` | `dist/../…` mogło usunąć katalog poza paczką | Rozwiązanie ścieżek, odrzucenie samego `dist`, wyjścia poza katalog oraz przekierowania `dist` przez symlink; wymagane narzędzia sprawdzane przed usuwaniem |
| Stare biblioteki i paczki używały zerowych identyfikatorów wielu obiektów | Nowy sezon mógł być nie do zapisania po włączeniu poprawnej walidacji UUID | Nadawanie identyfikatorów starym wpisom podczas odczytu niezależnej biblioteki/paczki; nowe paczki z powtórzonym identyfikatorem są odrzucane |

Importy paczek dodatkowo wskazują nazwę pliku, błąd odczytu, limit rozmiaru lub miejsce błędu JSON. Nieudany import nie zastępuje otwartej paczki.

Testy regresji obejmują zapis i ponowny odczyt dwóch wydarzeń z tym samym ID drużyny, brakujące odwołania, nieprawidłowy balans, wyłączność blokady i diagnostykę JSON. Próby skryptu Linux sprawdzają składnię oraz zachowanie pliku kontrolnego przy próbie wyjścia poza `dist`.

Dodatkowy test dwóch działających kopii Windows potwierdził blokadę wspólnego AppData nawet przy różnych katalogach `userData`. Pełne buildy i gotowe paczki Windows/Linux przeszły lokalny test utworzenia widoków QML. Szczegóły środowisk: [deploy](../development/DEPLOY.md).

Pozostałe ograniczenia: blokada nie obejmuje starej, już uruchomionej wersji programu; walidator nie sprawdza wszystkich zależności semantycznych pomiędzy kalendarzami; uszkodzony aktywny profil laboratorium wymaga odrębnego rozwiązania. Nie są objęte deklaracją napraw powyżej.
