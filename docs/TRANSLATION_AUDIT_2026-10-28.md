# Kontrola tłumaczeń — 28.10.2026

## Naprawione i sprawdzone

„Zakończ konkurs” już miał wpisy EN/CS w katalogach TS. Brakowało jednak
odświeżania dynamicznego tekstu przy zmianie języka w otwartych zawodach.
`CompetitionManagerWindow::changeEvent` ponownie ustawia odpowiedni przycisk
końca konkursu / następnej serii / następnej grupy. Nie uruchamia pełnego
`retranslateUi`, który mógłby wyzerować wyświetlane bieżące wartości.

Regresja kończy rzeczywisty konkurs i sprawdza kolejno „Finish competition”,
„Ukončit závod”, „Zakończ konkurs”, z niezmienionym zapisem wyników. PASS Qt5.
Dodano po 40 brakujących tłumaczeń EN/CS dla nowych metod pomiaru, ich pomocy,
podglądu przed skokiem i zapisanych faz wiatru oraz przywracania profilu.
Skrypt budowy generuje nowe QM; Windowsowy skrypt pakowania i WINDOWS_SETUP.MD
również zawierają `lrelease` — same pliki TS obok starego EXE nie wystarczą.

Test zmiany języka ujawnił też pętlę wysokości stopki. Jej wysokość wynika teraz
z metryk czcionek, bez zależności od geometrii zagnieżdżonego layoutu naszywki
wersji. Dopasowanie wysokości chrome jest kolejkowane i chronione QPointer.
`--translation-only` PASS bez ostrzeżenia o pętli bindingów.

## Nadal do sprawdzenia — pełny audyt odłożony

Użytkownik pozwolił odłożyć szerszą kontrolę, aby nie zatrzymywać integracji silnika.
Nie deklarować, że cała aplikacja ma już kompletne tłumaczenia. Do sprawdzenia:

- synchronizacja `lupdate` z najnowszym QML/C++, brakujące i unfinished EN/CS;
- napisy i menu tworzone jednorazowo w C++, ich odświeżanie przy otwartym ekranie;
- nowe edytory profili, pogoda, nazwy scenariuszy, walidacja i komunikaty błędów;
- formatowanie liczb/daty, atrybuty dostępności i tooltipy po zmianie języka;
- świeży start EN/CS oraz przełączanie w trakcie całego podstawowego workflow.

Nie zastępować tego kontrolą samych katalogów TS: zgłoszony błąd miał tłumaczenie
w pliku, ale nie odświeżał wartości ustawionej w runtime.

Dodatkowe zabezpieczenie: przełączanie języka zmienia **wyłącznie podpis**, według
zapamiętanego rodzaju akcji. Nie wywołuje funkcji konfigurujących przycisk,
które podpinają obsługę kliknięcia i zmieniają stan sterowania. Regresja sprawdza
jedno zakończenie po trzech zmianach języka oraz przejście dokładnie o jedną serię
lub jedną grupę. Zachowany jest też odrębny podpis przejścia z kwalifikacji do
konkursu. Testy PASS.

Nowy edytor manipulacji: dopisano EN/CS dla Quick, draftu walidacji i natywnej
prezentacji. `scripts/check-translations.py` przechodzi (kompletność obecnych
katalogów i argumentów). Nadal nie jest to audyt wszystkich źródeł aplikacji.

Dalszy etap: uzupełniono teksty Quick modułu konfiguracji wiatru (scenariusze,
dial, cztery parametry, lokalny edytor, opady i komunikaty). Kontrola katalogów
oraz test zmiany języka PASS. Pełny audyt innych modułów pozostaje w kolejce.

## Aktualizacja — pełna synchronizacja źródeł i playground v2

Uruchomiono lupdate na całym `source`, zachowując aktualne lokalizacje wpisów.
Uzupełniono brakujące EN/CS z wcześniejszych prac v2 (paczki, migracja, profile,
konfiguracja i pogoda) oraz wszystkie nowe widoki/metadane playgroundu.
Katalogi końcowe: 3367 wpisów na język, bez unfinished i rozbieżności argumentów.
Przy ponownym wejściu do języka odświeżają się również metadane parametrów
playgroundu i jego pozycje syntetycznych źródeł. Liczby wykresów i wyników używają
locale aplikacji; dane JSON/CSV pozostają maszynowym formatem z kropką dziesiętną.

Windows/Qt6: runtime PL/EN/CS PASS, również regresja języka w otwartym konkursie
w zestawie pogody. Nowy playground przeszedł rendery obu motywów, 360/520/1024/1440
oraz powiększony tekst. Szczegóły: PLAYGROUND_V2.md i SIMULATION_V2_PROGRESS.md.
